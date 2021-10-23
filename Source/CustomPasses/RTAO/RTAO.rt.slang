import Scene.Raytracing;
import Scene.intersection;

cbuffer RayTraceCB
{
    gFrameCount;
}

Texture2D<float4> 

struct RayPayload
{
    float aoVal;
}

[shader("raygeneration")]
void rayGen()
{
}

[shader("miss")]
void miss(inout RayPayload rayData)
{

}

[shader("anyhit")]
void anyHit(inout RayPayload rayData, BuiltinIntersectionAttribs attribs)
{
    if (alphaTestFails(attribs))
        IgnoreHit();
}

[shader("closesthit")]
void closestHit(inout RayPayload rayData, BuiltinIntersectionAttribs attribs)
{
}
