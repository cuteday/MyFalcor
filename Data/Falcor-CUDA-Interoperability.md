# Falcor-CUDA Interoperability

## Mapping Falcor resource onto CUDA device

Mapping a Falcor resource (eg. `Texture` or `Buffer`) onto a CUDA device actually maps the D3D 12 resource. There are two types of resources, i.e. the commited D3D resource and the synchronization objects, via the CUDA external memory mechanism. We will focus on the D3D commited resources for now.

#### Importing resource to CUDA external memory

D3D 12 resources can be imported to CUDA external memory via NT handles in Windows. The typical process are following: 

~~~c++
HANDLE sharedHandle = pBuffer->getSharedApiHandle();

cudaExternalMemoryHandleDesc externalMemoryHandleDesc = {};
externalMemoryHandleDesc.type = cudaExternalMemoryHandleTypeD3D12Resource;
externalMemoryHandleDesc.handle.win32.handle = sharedHandle;
externalMemoryHandleDesc.size = pBuffer->getSize();
externalMemoryHandleDesc.flags = cudaExternalMemoryDedicated;

cudaExternalMemory_t externalMemory;
cudaImportExternalMemory(&externalMemory, &externalMemoryHandleDesc);

~~~

The `Falcor::Resource::getSharedApiHandle()` routine retuens NT handle for every SHARED resource in Falcor. Remember to pass the `Falcor::ResourceBindFlags::Shared` bind flag when creating a shared resource in Falcor. 

#### Accessing CUDA external memory

There are two ways to access the imported resource in CUDA external memory: (1) directly mapping the external memory to a device pointer, or (2) mapping it onto a CUDA mipmapped array. 

The following process maps the external memory onto a device pointer: 

~~~c++
void* ptr = nullptr;
cudaExternalMemoryBufferDesc desc = {};
desc.offset = 0;
desc.size = size;
cudaExternalMemoryGetMappedBuffer(&ptr, externalMemory, &desc));
~~~

And the  following process maps the external memory onto a cuda mipmapped array (which can be turned into a cuda array later): 

~~~c++
cudaExternalMemoryMipmappedArrayDesc mipDesc;
memset(&mipDesc, 0, sizeof(mipDesc));
auto format = pTex->getFormat();
mipDesc.formatDesc.x = getNumChannelBits(format, 0);
mipDesc.formatDesc.y = getNumChannelBits(format, 1);
mipDesc.formatDesc.z = getNumChannelBits(format, 2);
mipDesc.formatDesc.w = getNumChannelBits(format, 3);
mipDesc.formatDesc.f = (getFormatType(format) == FormatType::Float) ? cudaChannelFormatKindFloat : cudaChannelFormatKindUnsigned;
mipDesc.extent.depth = 1;
mipDesc.extent.width = pTex->getWidth();
mipDesc.extent.height = pTex->getHeight();
mipDesc.flags = cudaUsageFlags;
mipDesc.numLevels = 1;
    cudaExternalMemoryGetMappedMipmappedArray(&mipmappedArray, externalMemory, &mipDesc);
~~~

#### Some observations

Mapping a CUDA external memory to device pointer consumes little frame time (likely $\leq0.1 \mathrm{ms}$), while mapping (`memcpy`ing) from device memoy to host  memory leads to a huge time cost, even mapping a single UAV counter to host memory costs $\approx 3\mathrm{ms}$. So try our best to avoid host-device memory transfer.



