# MyFalcor

Here are some code for learning purpose. Again, DXR raytracing is interesting! ヾ(≧▽≦*)o

The Falcor starter template removing needless components in the official version is in the [main branch](https://github.com/cuteday/MyFalcor/tree/main).

#### Ray-traced ambient occlusion
[code](https://github.com/cuteday/MyFalcor/tree/practice/Source/RenderPasses/RTAO)

<img src="Data\images\rtao_zeroday.png" alt="rtao_zeroday" style="zoom:33%;" />

#### Ray-traced pbr shading

[code](https://github.com/cuteday/MyFalcor/tree/practice/Source/RenderPasses/RTLightingPass)

<img src="Data\images\pbr_shading_arcade.png" alt="rtao_zeroday" style="zoom:33%;" />

### Be aware of...

In Falcor, a pass will be executed every frame only if it has a channel which is marked as output, or the channel is the dependency of other channel. For render passes which have no output channel, like `PixelInspectorPass`, a fake output is needed if you want to use it.

If a project in the solution needs CUDA interop, the `cuda.lib` needs to be added to the linker's additional inputs in addition to following the guidance in the official document. For more about Falcor-cuda interop, go [this note](Data/Falcor-CUDA Interoperability.md).

If  the shader of `PixelInspectorPass` doesn't compile (Falcor 4.4): [PixelInspectorPass crashing from HitInfo polymorphism? · Issue #281 · NVIDIAGameWorks/Falcor (github.com)](https://github.com/NVIDIAGameWorks/Falcor/issues/281).

