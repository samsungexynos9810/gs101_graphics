/*
 * Copyright (C) 2019 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef EXYNOS_RESOURCE_RESTRICTION_H_
#define EXYNOS_RESOURCE_RESTRICTION_H_

#include "ExynosHWCModule.h"
/*******************************************************************
 * Structures for restrictions
 * ****************************************************************/
#define RESTRICTION_NONE 0

#define USE_MODULE_ATTR

/* Basic supported features */
static feature_support_t feature_table[] =
{
    {MPP_DPP_GF,
        MPP_ATTR_AFBC | MPP_ATTR_BLOCK_MODE | MPP_ATTR_WINDOW_UPDATE |
        MPP_ATTR_FLIP_H | MPP_ATTR_FLIP_V |
        MPP_ATTR_DIM |
        MPP_ATTR_WCG | MPP_ATTR_HDR10 |
        MPP_ATTR_LAYER_TRANSFORM
    },

    {MPP_DPP_VGRFS,
        MPP_ATTR_AFBC | MPP_ATTR_BLOCK_MODE | MPP_ATTR_WINDOW_UPDATE | MPP_ATTR_SCALE |
        MPP_ATTR_FLIP_H | MPP_ATTR_FLIP_V | MPP_ATTR_ROT_90 |
        MPP_ATTR_DIM |
        MPP_ATTR_WCG | MPP_ATTR_HDR10 | MPP_ATTR_HDR10PLUS |
        MPP_ATTR_LAYER_TRANSFORM
    },

    {MPP_G2D,
        MPP_ATTR_AFBC | MPP_ATTR_SCALE |
        MPP_ATTR_FLIP_H | MPP_ATTR_FLIP_V | MPP_ATTR_ROT_90 |
        MPP_ATTR_WCG | MPP_ATTR_HDR10 | MPP_ATTR_HDR10PLUS | MPP_ATTR_USE_CAPA |
        MPP_ATTR_LAYER_TRANSFORM
    }
};

/**************************************************************************************
 * HAL_PIXEL_FORMATs
enum {
    HAL_PIXEL_FORMAT_RGBA_8888          = 1,
    HAL_PIXEL_FORMAT_RGBX_8888          = 2,
    HAL_PIXEL_FORMAT_RGB_888            = 3,
    HAL_PIXEL_FORMAT_RGB_565            = 4,
    HAL_PIXEL_FORMAT_BGRA_8888          = 5,
    HAL_PIXEL_FORMAT_YV12   = 0x32315659, // YCrCb 4:2:0 Planar
    HAL_PIXEL_FORMAT_YCbCr_422_SP       = 0x10, // NV16
    HAL_PIXEL_FORMAT_YCrCb_420_SP       = 0x11, // NV21
    HAL_PIXEL_FORMAT_YCbCr_422_I        = 0x14, // YUY2
    HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_P_M       = 0x101,
    HAL_PIXEL_FORMAT_EXYNOS_CbYCrY_422_I        = 0x103,
    HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M      = 0x105,
    HAL_PIXEL_FORMAT_EXYNOS_YCrCb_422_SP        = 0x106,
    HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M_TILED= 0x107,
    HAL_PIXEL_FORMAT_EXYNOS_ARGB_8888           = 0x108,
    HAL_PIXEL_FORMAT_EXYNOS_YCrCb_422_I         = 0x116,
    HAL_PIXEL_FORMAT_EXYNOS_CrYCbY_422_I        = 0x118,
    HAL_PIXEL_FORMAT_EXYNOS_YV12_M              = 0x11C,
    HAL_PIXEL_FORMAT_EXYNOS_YCrCb_420_SP_M      = 0x11D,
    HAL_PIXEL_FORMAT_EXYNOS_YCrCb_420_SP_M_FULL = 0x11E,
    HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_P         = 0x11F,
    HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP        = 0x120,
    HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M_PRIV = 0x121,
    // contiguous(single fd) custom formats
    HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_PN        = 0x122,
    HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SPN       = 0x123,
    HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SPN_TILED = 0x124,
    // 10-bit format (8bit + separated 2bit)
    HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M_S10B = 0x125,
    // 10-bit contiguous(single fd, 8bit + separated 2bit) custom formats
    HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SPN_S10B  = 0x126,
};
*************************************************************************************/

const restriction_key_t restriction_format_table[] =
{
    {MPP_DPP_GF, NODE_NONE, HAL_PIXEL_FORMAT_RGB_565, 0},
    {MPP_DPP_GF, NODE_NONE, HAL_PIXEL_FORMAT_RGBA_8888, 0},
    {MPP_DPP_GF, NODE_NONE, HAL_PIXEL_FORMAT_RGBX_8888, 0},
    {MPP_DPP_GF, NODE_NONE, HAL_PIXEL_FORMAT_BGRA_8888, 0},
    {MPP_DPP_GF, NODE_NONE, HAL_PIXEL_FORMAT_RGBA_1010102, 0},
    {MPP_DPP_VG, NODE_NONE, HAL_PIXEL_FORMAT_RGB_565, 0},
    {MPP_DPP_VG, NODE_NONE, HAL_PIXEL_FORMAT_RGBA_8888, 0},
    {MPP_DPP_VG, NODE_NONE, HAL_PIXEL_FORMAT_RGBX_8888, 0},
    {MPP_DPP_VG, NODE_NONE, HAL_PIXEL_FORMAT_BGRA_8888, 0},
    {MPP_DPP_VG, NODE_NONE, HAL_PIXEL_FORMAT_RGBA_1010102, 0},
    {MPP_DPP_VG, NODE_NONE, HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M, 0},
    {MPP_DPP_VG, NODE_NONE, HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M_PRIV, 0},
    {MPP_DPP_VG, NODE_NONE, HAL_PIXEL_FORMAT_EXYNOS_YCrCb_420_SP_M, 0},
    {MPP_DPP_VG, NODE_NONE, HAL_PIXEL_FORMAT_EXYNOS_YCrCb_420_SP_M_FULL, 0},
    {MPP_DPP_VG, NODE_NONE, HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SPN, 0},
    {MPP_DPP_VG, NODE_NONE, HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M_S10B, 0},
    {MPP_DPP_VG, NODE_NONE, HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SPN_S10B, 0},
    {MPP_DPP_VG, NODE_NONE, HAL_PIXEL_FORMAT_EXYNOS_YCbCr_P010_M, 0},
    {MPP_DPP_VG, NODE_NONE, HAL_PIXEL_FORMAT_YCBCR_P010, 0},
    {MPP_DPP_VG, NODE_NONE, HAL_PIXEL_FORMAT_GOOGLE_NV12_SP, 0},
    {MPP_DPP_VG, NODE_NONE, HAL_PIXEL_FORMAT_GOOGLE_NV12_SP_10B, 0},
    {MPP_DPP_VGS, NODE_NONE, HAL_PIXEL_FORMAT_RGB_565, 0},
    {MPP_DPP_VGS, NODE_NONE, HAL_PIXEL_FORMAT_RGBA_8888, 0},
    {MPP_DPP_VGS, NODE_NONE, HAL_PIXEL_FORMAT_RGBX_8888, 0},
    {MPP_DPP_VGS, NODE_NONE, HAL_PIXEL_FORMAT_BGRA_8888, 0},
    {MPP_DPP_VGS, NODE_NONE, HAL_PIXEL_FORMAT_RGBA_1010102, 0},
    {MPP_DPP_VGS, NODE_NONE, HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M, 0},
    {MPP_DPP_VGS, NODE_NONE, HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M_PRIV, 0},
    {MPP_DPP_VGS, NODE_NONE, HAL_PIXEL_FORMAT_EXYNOS_YCrCb_420_SP_M, 0},
    {MPP_DPP_VGS, NODE_NONE, HAL_PIXEL_FORMAT_EXYNOS_YCrCb_420_SP_M_FULL, 0},
    {MPP_DPP_VGS, NODE_NONE, HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SPN, 0},
    {MPP_DPP_VGS, NODE_NONE, HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M_S10B, 0},
    {MPP_DPP_VGS, NODE_NONE, HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SPN_S10B, 0},
    {MPP_DPP_VGS, NODE_NONE, HAL_PIXEL_FORMAT_EXYNOS_YCbCr_P010_M, 0},
    {MPP_DPP_VGS, NODE_NONE, HAL_PIXEL_FORMAT_YCBCR_P010, 0},
    {MPP_DPP_VGS, NODE_NONE, HAL_PIXEL_FORMAT_GOOGLE_NV12_SP, 0},
    {MPP_DPP_VGS, NODE_NONE, HAL_PIXEL_FORMAT_GOOGLE_NV12_SP_10B, 0},
    {MPP_DPP_VGF, NODE_NONE, HAL_PIXEL_FORMAT_RGB_565, 0},
    {MPP_DPP_VGF, NODE_NONE, HAL_PIXEL_FORMAT_RGBA_8888, 0},
    {MPP_DPP_VGF, NODE_NONE, HAL_PIXEL_FORMAT_RGBX_8888, 0},
    {MPP_DPP_VGF, NODE_NONE, HAL_PIXEL_FORMAT_BGRA_8888, 0},
    {MPP_DPP_VGF, NODE_NONE, HAL_PIXEL_FORMAT_RGBA_1010102, 0},
    {MPP_DPP_VGF, NODE_NONE, HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M, 0},
    {MPP_DPP_VGF, NODE_NONE, HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M_PRIV, 0},
    {MPP_DPP_VGF, NODE_NONE, HAL_PIXEL_FORMAT_EXYNOS_YCrCb_420_SP_M, 0},
    {MPP_DPP_VGF, NODE_NONE, HAL_PIXEL_FORMAT_EXYNOS_YCrCb_420_SP_M_FULL, 0},
    {MPP_DPP_VGF, NODE_NONE, HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SPN, 0},
    {MPP_DPP_VGF, NODE_NONE, HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M_S10B, 0},
    {MPP_DPP_VGF, NODE_NONE, HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SPN_S10B, 0},
    {MPP_DPP_VGF, NODE_NONE, HAL_PIXEL_FORMAT_EXYNOS_YCbCr_P010_M, 0},
    {MPP_DPP_VGF, NODE_NONE, HAL_PIXEL_FORMAT_YCBCR_P010, 0},
    {MPP_DPP_VGF, NODE_NONE, HAL_PIXEL_FORMAT_GOOGLE_NV12_SP, 0},
    {MPP_DPP_VGF, NODE_NONE, HAL_PIXEL_FORMAT_GOOGLE_NV12_SP_10B, 0},
    {MPP_DPP_VGRFS, NODE_NONE, HAL_PIXEL_FORMAT_RGB_565, 0},
    {MPP_DPP_VGRFS, NODE_NONE, HAL_PIXEL_FORMAT_RGBA_8888, 0},
    {MPP_DPP_VGRFS, NODE_NONE, HAL_PIXEL_FORMAT_RGBX_8888, 0},
    {MPP_DPP_VGRFS, NODE_NONE, HAL_PIXEL_FORMAT_BGRA_8888, 0},
    {MPP_DPP_VGRFS, NODE_NONE, HAL_PIXEL_FORMAT_RGBA_1010102, 0},
    {MPP_DPP_VGRFS, NODE_NONE, HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M, 0},
    {MPP_DPP_VGRFS, NODE_NONE, HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M_PRIV, 0},
    {MPP_DPP_VGRFS, NODE_NONE, HAL_PIXEL_FORMAT_EXYNOS_YCrCb_420_SP_M, 0},
    {MPP_DPP_VGRFS, NODE_NONE, HAL_PIXEL_FORMAT_EXYNOS_YCrCb_420_SP_M_FULL, 0},
    {MPP_DPP_VGRFS, NODE_NONE, HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SPN, 0},
    {MPP_DPP_VGRFS, NODE_NONE, HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M_S10B, 0},
    {MPP_DPP_VGRFS, NODE_NONE, HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SPN_S10B, 0},
    {MPP_DPP_VGRFS, NODE_NONE, HAL_PIXEL_FORMAT_EXYNOS_YCbCr_P010_M, 0},
    {MPP_DPP_VGRFS, NODE_NONE, HAL_PIXEL_FORMAT_YCBCR_P010, 0},
    {MPP_DPP_VGRFS, NODE_NONE, HAL_PIXEL_FORMAT_GOOGLE_NV12_SP, 0},
    {MPP_DPP_VGRFS, NODE_NONE, HAL_PIXEL_FORMAT_GOOGLE_NV12_SP_10B, 0},
    {MPP_G2D, NODE_NONE, HAL_PIXEL_FORMAT_RGB_565, 0},
    {MPP_G2D, NODE_NONE, HAL_PIXEL_FORMAT_RGB_888, 0},
    {MPP_G2D, NODE_NONE, HAL_PIXEL_FORMAT_RGBA_8888, 0},
    {MPP_G2D, NODE_NONE, HAL_PIXEL_FORMAT_RGBX_8888, 0},
    {MPP_G2D, NODE_NONE, HAL_PIXEL_FORMAT_BGRA_8888, 0},
    {MPP_G2D, NODE_NONE, HAL_PIXEL_FORMAT_RGBA_1010102, 0},
    {MPP_G2D, NODE_NONE, HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M, 0},
    {MPP_G2D, NODE_NONE, HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M_PRIV, 0},
    {MPP_G2D, NODE_NONE, HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M_TILED, 0},
    {MPP_G2D, NODE_NONE, HAL_PIXEL_FORMAT_EXYNOS_YCrCb_420_SP_M, 0},
    {MPP_G2D, NODE_NONE, HAL_PIXEL_FORMAT_EXYNOS_YCrCb_420_SP_M_FULL, 0},
    {MPP_G2D, NODE_NONE, HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SPN, 0},
    {MPP_G2D, NODE_NONE, HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M_S10B, 0},
    {MPP_G2D, NODE_NONE, HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SPN_S10B, 0},
    {MPP_G2D, NODE_NONE, HAL_PIXEL_FORMAT_EXYNOS_YCbCr_P010_M, 0},
    {MPP_G2D, NODE_NONE, HAL_PIXEL_FORMAT_YCrCb_420_SP, 0},
    {MPP_G2D, NODE_NONE, HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SPN_TILED, 0},
    {MPP_G2D, NODE_NONE, HAL_PIXEL_FORMAT_GOOGLE_NV12_SP, 0},
    {MPP_G2D, NODE_NONE, HAL_PIXEL_FORMAT_GOOGLE_NV12_SP_10B, 0},
};

const restriction_size_element restriction_size_table_rgb[] =
        {{{MPP_DPP_GF, NODE_SRC, HAL_PIXEL_FORMAT_NONE, 0},
          {{1, 1, 65535, 8191, 16, 16, 1, 1, 4096, 4096, 16, 16, 1, 1, 1, 1}}},
         {{MPP_DPP_VG, NODE_SRC, HAL_PIXEL_FORMAT_NONE, 0},
          {{1, 1, 65535, 8191, 16, 16, 1, 1, 4096, 4096, 16, 16, 1, 1, 1, 1}}},
         {{MPP_DPP_VGS, NODE_SRC, HAL_PIXEL_FORMAT_NONE, 0},
          {{2, 8, 65535, 8191, 16, 16, 1, 1, 4096, 4096, 16, 16, 1, 1, 1, 1}}},
         {{MPP_DPP_VGF, NODE_SRC, HAL_PIXEL_FORMAT_NONE, 0},
          {{1, 1, 65535, 8191, 16, 16, 1, 1, 4096, 4096, 16, 16, 1, 1, 1, 1}}},
         {{MPP_DPP_VGRFS, NODE_SRC, HAL_PIXEL_FORMAT_NONE, 0},
          {{2, 8, 65535, 8191, 16, 16, 1, 1, 4096, 4096, 16, 16, 1, 1, 1, 1}}},
         {{MPP_DPP_GF, NODE_DST, HAL_PIXEL_FORMAT_NONE, 0},
          {{1, 1, 65535, 8191, 16, 16, 1, 1, 4096, 4096, 16, 16, 1, 1, 1, 1}}},
         {{MPP_DPP_VG, NODE_DST, HAL_PIXEL_FORMAT_NONE, 0},
          {{1, 1, 65535, 8191, 16, 16, 1, 1, 4096, 4096, 16, 16, 1, 1, 1, 1}}},
         {{MPP_DPP_VGS, NODE_DST, HAL_PIXEL_FORMAT_NONE, 0},
          {{2, 8, 65535, 8191, 16, 16, 1, 1, 4096, 4096, 16, 16, 1, 1, 1, 1}}},
         {{MPP_DPP_VGF, NODE_DST, HAL_PIXEL_FORMAT_NONE, 0},
          {{1, 1, 65535, 8191, 16, 16, 1, 1, 4096, 4096, 16, 16, 1, 1, 1, 1}}},
         {{MPP_DPP_VGRFS, NODE_DST, HAL_PIXEL_FORMAT_NONE, 0},
          {{2, 8, 65535, 8191, 16, 16, 1, 1, 4096, 4096, 16, 16, 1, 1, 1, 1}}},
         /* MPP_G2D maxUpScale = max crop size / min crop size */
         {{MPP_G2D, NODE_NONE, HAL_PIXEL_FORMAT_NONE, 0},
          {{8192, 8192, 8192, 8192, 1, 1, 1, 1, 8192, 8192, 1, 1, 1, 1, 1, 1}}}};

const restriction_size_element restriction_size_table_yuv[] =
        {{{MPP_DPP_GF, NODE_SRC, HAL_PIXEL_FORMAT_NONE, 0},
          {{1, 1, 65534, 8190, 32, 32, 2, 2, 4096, 4096, 32, 32, 2, 2, 2, 2}}},
         {{MPP_DPP_VG, NODE_SRC, HAL_PIXEL_FORMAT_NONE, 0},
          {{1, 1, 65534, 8190, 32, 32, 2, 2, 4096, 4096, 32, 32, 2, 2, 2, 2}}},
         {{MPP_DPP_VGS, NODE_SRC, HAL_PIXEL_FORMAT_NONE, 0},
          {{2, 8, 65534, 8190, 32, 32, 2, 2, 4096, 4096, 32, 32, 2, 2, 2, 2}}},
         {{MPP_DPP_VGF, NODE_SRC, HAL_PIXEL_FORMAT_NONE, 0},
          {{1, 1, 65534, 8190, 32, 32, 2, 2, 4096, 4096, 32, 32, 2, 2, 2, 2}}},
         {{MPP_DPP_VGRFS, NODE_SRC, HAL_PIXEL_FORMAT_NONE, 0},
          {{2, 8, 65534, 8190, 32, 32, 2, 2, 4096, 4096, 32, 32, 2, 2, 2, 2}}},
         /* MPP_G2D maxUpScale = max crop size / min crop size */
         {{MPP_G2D, NODE_SRC, HAL_PIXEL_FORMAT_NONE, 0},
          {{4, 8192, 8192, 8192, 2, 2, 2, 2, 8192, 8192, 1, 1, 1, 1, 1, 1}}},
         {{MPP_DPP_GF, NODE_DST, HAL_PIXEL_FORMAT_NONE, 0},
          {{1, 1, 65535, 8191, 16, 16, 1, 1, 4096, 4096, 16, 16, 1, 1, 1, 1}}},
         {{MPP_DPP_VG, NODE_DST, HAL_PIXEL_FORMAT_NONE, 0},
          {{1, 1, 65535, 8191, 16, 16, 1, 1, 4096, 4096, 16, 16, 1, 1, 1, 1}}},
         {{MPP_DPP_VGS, NODE_DST, HAL_PIXEL_FORMAT_NONE, 0},
          {{2, 8, 65535, 8191, 16, 16, 1, 1, 4096, 4096, 16, 16, 1, 1, 1, 1}}},
         {{MPP_DPP_VGF, NODE_DST, HAL_PIXEL_FORMAT_NONE, 0},
          {{1, 1, 65535, 8191, 16, 16, 1, 1, 4096, 4096, 16, 16, 1, 1, 1, 1}}},
         {{MPP_DPP_VGRFS, NODE_DST, HAL_PIXEL_FORMAT_NONE, 0},
          {{2, 8, 65535, 8191, 16, 16, 1, 1, 4096, 4096, 16, 16, 1, 1, 1, 1}}},
         /* MPP_G2D maxUpScale = max crop size / min crop size */
         {{MPP_G2D, NODE_DST, HAL_PIXEL_FORMAT_NONE, 0},
          {{8192, 8192, 8192, 8192, 2, 2, 2, 2, 8192, 8192, 2, 2, 2, 2, 2, 2}}}};

const restriction_table_element restriction_tables[RESTRICTION_MAX] =
{
    {RESTRICTION_RGB, restriction_size_table_rgb, sizeof(restriction_size_table_rgb)/sizeof(restriction_size_element)},
    {RESTRICTION_YUV, restriction_size_table_yuv, sizeof(restriction_size_table_yuv)/sizeof(restriction_size_element)}
};

#define USE_MODULE_DPU_ATTR_MAP
const dpu_attr_map_t dpu_attr_map_table [] =
{
    {DPP_ATTR_AFBC, MPP_ATTR_AFBC},
    {DPP_ATTR_BLOCK, MPP_ATTR_BLOCK_MODE},
    {DPP_ATTR_FLIP, MPP_ATTR_FLIP_H | MPP_ATTR_FLIP_V},
    {DPP_ATTR_ROT, MPP_ATTR_ROT_90},
    {DPP_ATTR_SCALE, MPP_ATTR_SCALE},
    {DPP_ATTR_HDR, MPP_ATTR_HDR10 | MPP_ATTR_WCG | MPP_ATTR_LAYER_TRANSFORM},
    {DPP_ATTR_HDR10_PLUS, MPP_ATTR_HDR10PLUS | MPP_ATTR_HDR10 | MPP_ATTR_WCG | MPP_ATTR_LAYER_TRANSFORM},
    {DPP_ATTR_C_HDR, MPP_ATTR_HDR10},
    {DPP_ATTR_C_HDR10_PLUS, MPP_ATTR_HDR10PLUS},
    {DPP_ATTR_WCG, MPP_ATTR_WCG},
};

static ppc_table ppc_table_map = {
    /* G2D support only 2 plane YUV, so all YUV format should use YUV2P PPC table */
    /* In case of Scale-Up, G2D should use same PPC table */
    {PPC_IDX(MPP_G2D,PPC_FORMAT_YUV420,PPC_ROT_NO),   {3.5, 3.6, 4.3, 4.3, 3.5, 3.7, 3.7}},
    {PPC_IDX(MPP_G2D,PPC_FORMAT_YUV420,PPC_ROT),      {2.8, 3.2, 3.9, 4.3, 3.6, 2.6, 2.6}},

    {PPC_IDX(MPP_G2D,PPC_FORMAT_YUV422,PPC_ROT_NO),   {3.5, 3.6, 4.3, 4.3, 3.5, 3.7, 3.7}},
    {PPC_IDX(MPP_G2D,PPC_FORMAT_YUV422,PPC_ROT),      {2.8, 3.2, 3.9, 4.3, 3.6, 2.6, 2.6}},

    {PPC_IDX(MPP_G2D,PPC_FORMAT_P010,PPC_ROT_NO),     {3.5, 3.6, 4.3, 4.3, 3.5, 3.7, 3.7}},
    {PPC_IDX(MPP_G2D,PPC_FORMAT_P010,PPC_ROT),        {2.8, 3.2, 3.9, 4.3, 3.6, 2.6, 2.6}},

    {PPC_IDX(MPP_G2D,PPC_FORMAT_RGB32,PPC_ROT_NO),    {3.2, 2.1, 2.6, 3.3, 3.6, 3.8, 3.8}},
    {PPC_IDX(MPP_G2D,PPC_FORMAT_RGB32,PPC_ROT),       {3.6, 2.1, 2.7, 3.3, 3.8, 3.5, 3.5}},

    {PPC_IDX(MPP_G2D,PPC_FORMAT_SBWC,PPC_ROT_NO),     {2.6, 1.4, 0.9, 1.0, 1.0, 3.6, 3.6}},
    {PPC_IDX(MPP_G2D,PPC_FORMAT_SBWC,PPC_ROT),        {2.6, 1.4, 0.9, 1.0, 1.0, 2.6, 2.6}},

    {PPC_IDX(MPP_G2D,PPC_FORMAT_AFBC_RGB,PPC_ROT_NO), {3.4, 0.3, 0.5, 0.8, 0.7, 1.6, 1.6}},
    {PPC_IDX(MPP_G2D,PPC_FORMAT_AFBC_RGB,PPC_ROT),    {3.6, 0.3, 0.8, 0.9, 0.9, 1.4, 1.4}},

    {PPC_IDX(MPP_G2D,PPC_FORMAT_AFBC_YUV,PPC_ROT_NO), {2.0, 0.8, 0.3, 0.3, 0.4, 2.9, 2.9}},
    {PPC_IDX(MPP_G2D,PPC_FORMAT_AFBC_YUV,PPC_ROT),    {2.0, 0.8, 0.3, 0.3, 0.4, 2.6, 2.6}},
};

#endif
