
#ifndef ZIPLAB_BASIC_EXPORT_H
#define ZIPLAB_BASIC_EXPORT_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include "ziplab/config/config.h"
#include "ziplab/basic/platform.h"
#include "ziplab/basic/compiler.h"

#if defined(ZIPLAB_BUILD_DLL) || defined(BUILD_DLL) || defined(_USRDLL) \
    || defined(ZIPLAB_SHARED) || defined(ZIPLAB_BUILD_SHARED)
    || defined(ZIPLAB_ENABLE_SHARED) || defined(ENABLE_SHARED)
    /* build a dll library */
    #undef ZIPLAB_BUILD_STATIC
    #undef ZIPLAB_USE_SHARED
    #undef ZIPLAB_USE_STATIC

    #ifndef ZIPLAB_BUILD_SHARED
        #define ZIPLAB_BUILD_SHARED
    #endif
#elif defined(ZIPLAB_USE_DLL) || defined(USE_DLL) \
    || defined(ZIPLAB_USE_SHARED) || defined(USE_SHARED)
    /* use a dll library */
    #undef ZIPLAB_BUILD_SHARED
    #undef ZIPLAB_BUILD_STATIC
    #undef ZIPLAB_USE_STATIC

    #ifndef ZIPLAB_USE_SHARED
        #define ZIPLAB_USE_SHARED
    #endif
#elif defined(ZIPLAB_BUILD_LIB) || defined(BUILD_LIB) || defined(_LIB) \
    || defined(ZIPLAB_LIB) || defined(ZIPLAB_IS_LIB) \
    || defined(ZIPLAB_STATIC) || defined(ZIPLAB_BUILD_STATIC) \
    || defined(ZIPLAB_ENABLED_STATIC) || defined(ENABLED_STATIC)
    /* build a static library */
    #undef ZIPLAB_BUILD_SHARED
    #undef ZIPLAB_USE_SHARED
    #undef ZIPLAB_USE_STATIC

    #ifndef ZIPLAB_BUILD_STATIC
        #define ZIPLAB_BUILD_STATIC
    #endif
#else /* defined(USE_LIB) \
    || defined(ZIPLAB_USE_LIB) || defined(USE_LIB) \
    || defined(ZIPLAB_USE_STATIC) || defined(USE_STATIC) */
    /* use a static library */
    #undef ZIPLAB_BUILD_SHARED
    #undef ZIPLAB_BUILD_STATIC
    #undef ZIPLAB_USE_SHARED

    #ifndef ZIPLAB_USE_STATIC
        #define ZIPLAB_USE_STATIC
    #endif
#endif

////////////////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER) || __has_declspec_attribute(dllexport)
  #ifndef ZIPLAB_DECL_EXPORT
    #define ZIPLAB_DECL_EXPORT    __declspec(dllexport)
  #endif
#else
  #if defined(__GNUC__) || defined(__clang__) || defined(__linux__)
    #ifndef ZIPLAB_DECL_EXPORT
      #define ZIPLAB_DECL_EXPORT  __attribute__((visibility("default")))
    #endif
  #endif
#endif

#if defined(_MSC_VER) || __has_declspec_attribute(dllimport)
  #ifndef ZIPLAB_DECL_EXPORT
    #define ZIPLAB_DECL_IMPORT    __declspec(dllimport)
  #endif
#else
  #if defined(__GNUC__) || defined(__clang__) || defined(__linux__)
    #ifndef ZIPLAB_DECL_IMPORT
      #define ZIPLAB_DECL_IMPORT  __attribute__((visibility("default")))
    #endif
  #endif
#endif

////////////////////////////////////////////////////////////////////////////////

/**************************************************************************************
   Dynamic Library import / export / static control conditional
   (Define ZIPLAB_DECLARE_EXPORT to export symbols, else they are imported or static)
**************************************************************************************/

#ifdef ZIPLAB_DLL
#undef ZIPLAB_DLL
#endif

#if defined(ZIPLAB_IS_MSVC)  /* is microsoft visual studio ? */
  #if defined(ZIPLAB_BUILD_SHARED)    /* build a dll library */
    #define ZIPLAB_DLL                ZIPLAB_DECL_EXPORT
    #define ZIPLAB_DLL_TPL            ZIPLAB_DECL_EXPORT
    #define ZIPLAB_PRIVATE            ZIPLAB_DECL_HIDDEN
    #define ZIPLAB_EXPIMP_TEMPLATE
  #elif defined(ZIPLAB_USE_SHARED)    /* use a dll library */
    #define ZIPLAB_DLL                ZIPLAB_DECL_IMPORT
    #define ZIPLAB_DLL_TPL            ZIPLAB_DECL_IMPORT   // or don't defined it!
    #define ZIPLAB_PRIVATE            ZIPLAB_DECL_HIDDEN
    #define ZIPLAB_EXPIMP_TEMPLATE    extern
  #elif defined(ZIPLAB_BUILD_STATIC)  /* build a static library */
    #define ZIPLAB_DLL
    #define ZIPLAB_DLL_TPL
    #define ZIPLAB_PRIVATE
    #define ZIPLAB_EXPIMP_TEMPLATE
  #else                             /* use a static library */
    #define ZIPLAB_DLL
    #define ZIPLAB_DLL_TPL
    #define ZIPLAB_PRIVATE
    #define ZIPLAB_EXPIMP_TEMPLATE
  #endif
#elif defined(ZIPLAB_IS_GNUC) || defined(ZIPLAB_IS_CLANG)
  #if defined(ZIPLAB_BUILD_SHARED) || defined(ZIPLAB_BUILD_STATIC)
                                    /* build a dll library */
                                    /* build a static library */
    #define ZIPLAB_DLL                ZIPLAB_DECL_EXPORT
    #define ZIPLAB_DLL_TPL            ZIPLAB_DECL_EXPORT
    #define ZIPLAB_PRIVATE            ZIPLAB_DECL_HIDDEN
    #define ZIPLAB_EXPIMP_TEMPLATE
  #else                             /* use a static library or dll library */
    #define ZIPLAB_DLL                ZIPLAB_DECL_IMPORT
    #define ZIPLAB_DLL_TPL            ZIPLAB_DECL_IMPORT   // or don't defined it!
    #define ZIPLAB_PRIVATE            ZIPLAB_DECL_HIDDEN
    #define ZIPLAB_EXPIMP_TEMPLATE    extern
  #endif
#else  /* not is msvc and not is gunc, clang! */
  #if defined(ZIPLAB_BUILD_SHARED) || defined(ZIPLAB_BUILD_STATIC)
                                    /* build a dll library */
                                    /* build a static library */
    #define ZIPLAB_DLL                ZIPLAB_DECL_EXPORT
    #define ZIPLAB_DLL_TPL            ZIPLAB_DECL_EXPORT
    #define ZIPLAB_PRIVATE            ZIPLAB_DECL_HIDDEN
    #define ZIPLAB_EXPIMP_TEMPLATE
  #else                             /* use a static library or dll library */
    #define ZIPLAB_DLL                ZIPLAB_DECL_IMPORT
    #define ZIPLAB_DLL_TPL            ZIPLAB_DECL_IMPORT   // or don't defined it!
    #define ZIPLAB_PRIVATE            ZIPLAB_DECL_HIDDEN
    #define ZIPLAB_EXPIMP_TEMPLATE    extern
  #endif
#endif // ZIPLAB_IS_MSVC

////////////////////////////////////////////////////////////////////////////////

/**
 * For exported func
 */
#if defined(_MSC_VER) && (_MSC_VER >= 1400)
  #define ZIPLAB_EXPORTED_FUNC        __cdecl
  #define ZIPLAB_EXPORTED_METHOD      __thiscall
#else
  #define ZIPLAB_EXPORTED_FUNC
  #define ZIPLAB_EXPORTED_METHOD
#endif

////////////////////////////////////////////////////////////////////////////////

#endif // ZIPLAB_BASIC_EXPORT_H
