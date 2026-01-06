// engine/render/d3d12/RenderDeviceD3D12.cpp
std::unique_ptr<IRenderDevice> createRenderDevice() {
  return std::make_unique<RenderDeviceD3D12>();
}
