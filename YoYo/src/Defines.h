#pragma once

#ifdef Y_EXPORT
    #ifdef Y_PLATFORM_WINDOWS
        #define YAPI __declspec(dllexport)
    #elif Y_PLATFORM_APPLE
        #define YAPI 
    #endif
#else
    #define YAPI
#endif