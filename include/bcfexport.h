#pragma once

#if defined(BUILD_BCF_LIB)
    #define BCF_EXPORT __declspec(dllexport)
#else
    #define BCF_EXPORT __declspec(dllimport)
#endif
