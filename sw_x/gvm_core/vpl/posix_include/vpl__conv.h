//
//  Copyright 2010 iGware Inc.
//  All Rights Reserved.
//
//  THIS SOFTWARE CONTAINS CONFIDENTIAL INFORMATION AND
//  TRADE SECRETS OF IGWARE INC.
//  USE, DISCLOSURE OR REPRODUCTION IS PROHIBITED WITHOUT
//  THE PRIOR EXPRESS WRITTEN PERMISSION OF IGWARE INC.
//

#ifndef __VPL__CONV_H__
#define __VPL__CONV_H__

/// @file
/// Platform-private definitions, please do not include this header directly.

#include "vpl_types.h"
#include <endian.h>

#ifdef __cplusplus
extern "C" {
#endif

#if __BYTE_ORDER == __BIG_ENDIAN
# define VPL_HOST_IS_BIG_ENDIAN 1
# define VPL_HOST_IS_LITTLE_ENDIAN 0
#elif __BYTE_ORDER == __LITTLE_ENDIAN
# define VPL_HOST_IS_LITTLE_ENDIAN 1
# define VPL_HOST_IS_BIG_ENDIAN 0
#else
# error Unknown host byte order.
#endif

#ifdef __cplusplus
}
#endif

#endif // include guard
