#pragma once
//no platform agnostic debug, so declare our own
#ifdef __APPLE_CC__
#ifdef DEBUG
#define PLATFORM_DEBUG 1
#endif
#endif

#ifdef _WIN32
#ifdef _DEBUG
#define PLATFORM_DEBUG 1
#endif
#endif