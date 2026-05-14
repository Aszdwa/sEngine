#ifndef VK_INCLUDE_H
#define VK_INCLUDE_H
//
#if defined(__INTELLISENSE__) || !defined(USE_CPP20_MODULES)
#include <vulkan/vulkan_raii.hpp>
#else
import vulkan_hpp;
#endif // if def INTELLISENSE or def USE_CPP20_MODULES
#endif // VK_INCLUDE_H