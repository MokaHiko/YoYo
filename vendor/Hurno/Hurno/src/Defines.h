#ifdef H_EXPORT
    #ifdef H_PLATFORM_WIN32
        #define HAPI __declspec(dllexport)
    #elif H_PLATFORM_APPLE
        #define HAPI 
    #endif
#else
    #define HAPI 
#endif