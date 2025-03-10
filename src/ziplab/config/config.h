
#ifndef ZIPLAB_CONFIG_CONFIG_H
#define ZIPLAB_CONFIG_CONFIG_H

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

// Minimum requirements: gcc/clang C++ 11 or MSVC 2015 Update 3.
#if (!defined(_MSC_VER) && defined(__cplusplus) && (__cplusplus < 201103L)) \
 || (defined(_MSVC_LANG) && (_MSVC_LANG < 201103L)) \
 || (defined(_MSC_VER) && (_MSC_FULL_VER < 190024210))
#error "jstd requires C++ 11 support."
#endif

#include "ziplab/config/config_pre.h"

#include "ziplab/version.h"

// Could not change their order.
#include "ziplab/basic/platform.h"
#include "ziplab/basic/compiler.h"
#include "ziplab/basic/export.h"

#include "ziplab/config/config_ziplab.h"
#include "ziplab/config/config_hw.h"
#include "ziplab/config/config_cxx.h"
#include "ziplab/config/config_post.h"

#endif // ZIPLAB_CONFIG_CONFIG_H
