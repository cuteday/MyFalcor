#include "RTAO.h"

namespace
{
    const char kDesc[] = "Ambient occlusion with ray tracing";

    const std::string kColorIn = "colorIn";
    const std::string kColorOut = "colorOut";
    const std::string kDepth = "depth";
    const std::string kNormals = "normals";
    const std::string kAoMap = "aoMap";

    const std::string kProgramRaytraceFile = "RTAO.rt.slang";
}

// Don't remove this. it's required for hot-reload to function properly
extern "C" __declspec(dllexport) const char* getProjDir()
{
    return PROJECT_DIR;
}

extern "C" __declspec(dllexport) void getPasses(Falcor::RenderPassLibrary& lib)
{
    lib.registerClass("RTAO", kDesc, RTAO::create);
}

RTAO::SharedPtr RTAO::create(RenderContext* pRenderContext, const Dictionary& dict)
{
    SharedPtr pPass = SharedPtr(new RTAO);

    return pPass;
}

std::string RTAO::getDesc() { return kDesc; }

Dictionary RTAO::getScriptingDictionary()
{
    return Dictionary();
}

RenderPassReflection RTAO::reflect(const CompileData& compileData)
{
    // Define the required resources here
    RenderPassReflection reflector;
    reflector.addOutput(kColorOut, "Color buffer with AO");
    reflector.addOutput(kAoMap, "AO factor");

    reflector.addInput(kColorIn, "Color buffer");
    reflector.addInput(kNormals, "World space normals in [0, 1]");
    
    return reflector;
}

void RTAO::execute(RenderContext* pRenderContext, const RenderData& renderData)
{

    // renderData holds the requested resources
    // auto& pTexture = renderData["src"]->asTexture();
    const auto& pColorIn = renderData[kColorIn]->asTexture();
    const auto& pColorOut = renderData[kColorOut]->asTexture();
    const auto& pNormals = renderData[kNormals]->asTexture();
    const auto& pAoMap = renderData[kAoMap]->asTexture();

    // get screen dimensions for ray dispatch
    const uint2 targetDim = renderData.getDefaultTextureDims();

    // set constant buffer variables
    auto var = mpProgramVars->getRootVar());
    var["RayTraceCB"]["gFrameCount"] = mFrameCount;

    // this call to raytrace builds the accel structures for the scene, then spawn the rays
    mpScene->raytrace(pRenderContext, mpProgram.get(), mpProgramVars, uint3(targetDim, 1));

    mFrameCount++;
}

void RTAO::renderUI(Gui::Widgets& widget)
{
    widget.text("Hello DXR!");
}

void RTAO::setScene(RenderContext* pRenderContext, const Scene::SharedPtr& pScene)
{
    // clear data for previous scene is a good convention > <
    mpProgram = nullptr;
    mpProgramVars = nullptr;

    mpScene = pScene;
    // RenderPass::setScene(pRenderContext, pScene); // inherited nothing here

    if (mpScene) {
        // usually create programe on resetting scene!
        RtProgram::Desc desc;
        desc.addShaderLibrary(kProgramRaytraceFile);
        desc.addDefines(mpScene->getSceneDefines());

        RtBindingTable::SharedPtr sbt = RtBindingTable::create(1, 1, mpScene->getGeometryCount());
        sbt->setRayGen(desc.addRayGen("rayGen"));
        sbt->setMiss(0, desc.addMiss("miss"));
        sbt->setHitGroupByType(0, mpScene, Scene::GeometryType::TriangleMesh, desc.addHitGroup("closestHit", "anyHit"));

        mpProgram = RtProgram::create(desc);
        mpProgramVars = RtProgramVars::create(mpProgram, sbt);
    }
}
