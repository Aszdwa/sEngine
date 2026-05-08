#include "VkInterface.h"
#include <chrono>
#include <thread>

int main() {
  // Both window and vulkan were initialized and stored in heap
  Window *window = new Window();
  if (int result = window->Create(); result != 0)
    return result;
  VulkanContext *vulkan = new VulkanContext();
  ModelLoader model;
  TextureImage texture;
  vulkan->SetWindowPtr(window);
  vulkan->SetModelPtr(&model);
  vulkan->SetTexturePtr(&texture);
  window->SetFbResize(vulkan->GetFbResize());
  if (int result = vulkan->Initialize(); result != 0)
    return result;

  model.context = vulkan;
  model.Load("Models/cube_mod.obj");

  texture.context = vulkan;
  texture.Load("Textures/emoosaka.jpg");

  // window->SetWindowStyle(WINDOW_STYLE::FULLSCREEN);

  window->ShowWindow();
  while (window->IsRunning()) {
    window->WindowEvents();
    vulkan->DrawFrame();
    std::this_thread::sleep_for(std::chrono::milliseconds(16));
  }

  // model.Destroy();
  // texture.Destroy();

  delete vulkan;
  delete window;
  return 0;
}
// TODO REWRITE LIST
//
// MOST IMPORTANT : DECREASE THE PROCESS MEMORY USAGE
//
// Fix the size of descriptor pool
// Fix the pointer reference on vulkan related classes
// Map uniforms buffers explicitly during buffer creation
// PS: Modified: CreateUniformBuffers and UpdateUniformBuffers

// TODO LIST:
// Fix image flickering on resize
// Separate console logging for debug only
// Fix anisotropic filter
// Correct textures over-saturated colors
// Correct rasterizer cullmode and frontface on pipeline creation section
// Use push constants instead of UBO
// Add support for 3D vertices
// Improve the file handler script
// Add aspect ratio
// Handle vulkan pair returns on object creations gracefully
// Improve the getter functions
// Reduce the number of copies of window pointers
// Make 'isRunning' variable accessible from all important files
// Optimize window events
// Make a custom vulkan allocator
// Create a suitable loader in order to define VK_NO_PROTOTYPES :: Manually load
// vulkan functions at runtime Make a suitable environment to define
// VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1 Make an error handling system for
// 'Release' mode
//

// Add TextureImage and ModelLoader as 'friends' of VulkanContext or do it in a
// way that has access