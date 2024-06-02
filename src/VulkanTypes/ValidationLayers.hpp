#pragma once
#include <vector>
#include "PlatformDebug.hpp"


#ifdef PLATFORM_DEBUG
constexpr bool cEnableValidationLayers = true;
#else
constexpr bool cEnableValidationLayers = false;
#endif

const std::vector<const char*> validationLayers = {
"VK_LAYER_KHRONOS_validation"
};
