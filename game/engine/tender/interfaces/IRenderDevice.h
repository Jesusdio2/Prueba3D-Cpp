// engine/render/interface/IRenderDevice.h
struct RenderInit {
  int width, height;
  bool vsync;
};

class IRenderDevice {
public:
  virtual ~IRenderDevice() = default;
  virtual void init(const RenderInit&) = 0;
  virtual void resize(int w, int h) = 0;
  virtual void beginFrame() = 0;
  virtual void draw() = 0;
  virtual void endFrame() = 0;
};

// src/main.cpp
std::unique_ptr<IRenderDevice> createRenderDevice();

int main() {
  RenderInit cfg{1280, 720, true};
  auto device = createRenderDevice();
  device->init(cfg);
  while (running()) {
    device->beginFrame();
    device->draw();
    device->endFrame();
  }
}
