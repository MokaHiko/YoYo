#pragma once

#ifdef Y_EXPORT
    #ifdef Y_PLATFORM_WIN32
        #define YAPI __declspec(dllexport)
    #else // Assuming Y_PLATFORM_APPLE or other Unix-like platforms
        #define YAPI __attribute__((visibility("default")))
    #endif
#else // If Y_EXPORT is not defined, assume importing from the DLL
    #ifdef Y_PLATFORM_WIN32
        #define YAPI 
    #else // Assuming Y_PLATFORM_APPLE or other Unix-like platforms
        #define YAPI __attribute__((visibility("default")))
    #endif
#endif

