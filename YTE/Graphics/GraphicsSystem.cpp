#include "Windows.h"

#include "YTE/Core/Engine.hpp"

#include "YTE/Graphics/GraphicsSystem.hpp"

#include "vulkan/vkel.h"
#include "vulkan/vk_cpp.hpp"


namespace YTE
{
  class vulkan_context
  {
    public:
    u32 mWidth;
    u32 mHeight;

    vk::Instance mInstance;
  };


  void vulkan_assert(bool flag, char *msg = "")
  {
    if (!flag)
    {
      printf("ASSERT: ");
      printf("%s", msg);
      printf("\n");

      int *base = 0;
      *base = 1;
    }
  }

  void checkVulkanResult(vk::Result &result, char *msg) 
  {
    vulkan_assert(result == vk::Result::eSuccess, msg);
  }

  GraphicsSystem::GraphicsSystem(Engine *aEngine) : mEngine(aEngine)
  {
    vkelInit();

    auto self = mPlatformSpecificData.ConstructAndGet<vulkan_context>();

    

    auto appInfo = vk::ApplicationInfo()
                     .setPApplicationName("First Test")
                     .setEngineVersion(1)
                     .setApiVersion(VK_MAKE_VERSION(1,0,0));

    auto instanceInfo = vk::InstanceCreateInfo()
                        .setPApplicationInfo(&appInfo);

    auto result = vk::createInstance(&instanceInfo, NULL, &self->mInstance);
    checkVulkanResult(result, "Failed to create vulkan instance.");
  }

  GraphicsSystem::~GraphicsSystem()
  {
    vkelUninit();
  }

  void GraphicsSystem::SetUpWindow(Window *aWindow)
  {
    aWindow->CreateOpenGLContext();
    aWindow->MakeOpenGLContextCurrent();
  }

  void GraphicsSystem::Initialize()
  {
    glbinding::Binding::initialize();
  }

  void GraphicsSystem::Update(float aDt)
  {
    for (auto &window : mEngine->mWindows)
    {
      gl::glViewport(0, 0, window->mWidth, window->mHeight);

      gl::glClearColor(0.5f, 1.0f, 0.5f, 1.0f);
      gl::glClear(gl::GL_COLOR_BUFFER_BIT);

      window->SwapBuffers();
    }
  }
}
