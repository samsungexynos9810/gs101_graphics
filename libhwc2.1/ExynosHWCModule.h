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

#ifndef ANDROID_EXYNOS_HWC_MODULE_H_
#define ANDROID_EXYNOS_HWC_MODULE_H_

#include "ExynosHWC.h"
#include "DeconHeader.h"

#define HWC_VERSION     HWC_DEVICE_API_VERSION_1_5

#define VSYNC_DEV_PREFIX "/sys/devices/"
#define VSYNC_DEV_MIDDLE "platform/"
#define VSYNC_DEV_NAME  "19030000.decon_f/vsync"
#define VSYNC_DEV_NAME_EXT  "19050000.decon_t/vsync"
#define PSR_DEV_NAME  "19030000.decon_f/psr_info"
#define DP_LINK_NAME	"130b0000.displayport"
#define DP_UEVENT_NAME	"change@/devices/platform/%s/extcon/extcon0"
#define DP_CABLE_STATE_NAME "/sys/devices/platform/%s/extcon/extcon0/cable.0/state"

#define IDMA(x) static_cast<decon_idma_type>(x)

enum {
    HWC_DISPLAY_NONE_BIT = 0
};

/*
 * pre_assign_info: all display_descriptors that want to reserve
 */
struct exynos_mpp_t {
    int physicalType;
    int logicalType;
    char name[16];
    uint32_t physical_index;
    uint32_t logical_index;
    uint32_t pre_assign_info;
};

const dpp_channel_map_t IDMA_CHANNEL_MAP[] = {
    /* GF physical index is switched to change assign order */
    {MPP_DPP_GF,     1, IDMA_GF0,   IDMA(0)},
    {MPP_DPP_VGRFS,  0, IDMA_VGRFS0,IDMA(1)},
    {MPP_DPP_GF,     0, IDMA_GF1,   IDMA(2)},
    {MPP_DPP_VGF,    0, IDMA_VGF0,  IDMA(3)},
    {MPP_DPP_VG,     0, IDMA_VG0,   IDMA(4)},
    {MPP_DPP_VGS,    0, IDMA_VGS0,  IDMA(5)},
    {MPP_P_TYPE_MAX, 0, ODMA_WB,    IDMA(6)}, // not idma but..
    {static_cast<mpp_phycal_type_t>(MAX_DECON_DMA_TYPE), 0, MAX_DECON_DMA_TYPE, IDMA(7)}
};

#define DECON_PRIMARY_DEV_NAME  "/dev/graphics/fb0"
#define DECON_EXTERNAL_DEV_NAME "/dev/graphics/fb1"
#define DECON_EXT_BASE_WINDOW   0
#define PRIMARY_MAIN_BASE_WIN   2
#define EXTERNAL_MAIN_BASE_WIN  4

#define DISPLAY_MODE_MASK_LEN    8
#define DISPLAY_MODE_MASK_BIT    0xff
enum {
    DISPLAY_MODE_PRIMARY_MAIN = 0,  /* This is default mode */
    DISPLAY_MODE_EXTERNAL_MAIN,
    DISPLAY_MODE_NUM
};

/*
 * This is base window index of primary display for each display mode.
 * External display base window is always 0
 */
const uint32_t PRIMARY_DISP_BASE_WIN[] = {PRIMARY_MAIN_BASE_WIN, EXTERNAL_MAIN_BASE_WIN};

#define EXTERNAL_MAIN_DISPLAY_START_BIT (DISPLAY_MODE_MASK_LEN * DISPLAY_MODE_EXTERNAL_MAIN)
enum {
    EXTERNAL_MAIN_DISPLAY_PRIMARY_BIT = 1 << (EXTERNAL_MAIN_DISPLAY_START_BIT + HWC_DISPLAY_PRIMARY),
    EXTERNAL_MAIN_DISPLAY_EXTERNAL_BIT = 1 << (EXTERNAL_MAIN_DISPLAY_START_BIT + HWC_DISPLAY_EXTERNAL),
    EXTERNAL_MAIN_DISPLAY_VIRTUAL_BIT = 1 << (EXTERNAL_MAIN_DISPLAY_START_BIT + HWC_DISPLAY_VIRTUAL),
};

const exynos_mpp_t AVAILABLE_OTF_MPP_UNITS[] = {
    {MPP_DPP_GF, MPP_LOGICAL_DPP_GF, "DPP_GF1", 0, 0, HWC_DISPLAY_EXTERNAL_BIT|EXTERNAL_MAIN_DISPLAY_EXTERNAL_BIT},
    {MPP_DPP_GF, MPP_LOGICAL_DPP_GF, "DPP_GF0", 1, 0, HWC_DISPLAY_PRIMARY_BIT|EXTERNAL_MAIN_DISPLAY_PRIMARY_BIT},
    {MPP_DPP_VG, MPP_LOGICAL_DPP_VG, "DPP_VG0", 0, 0, HWC_DISPLAY_PRIMARY_BIT|EXTERNAL_MAIN_DISPLAY_EXTERNAL_BIT},
    {MPP_DPP_VGS, MPP_LOGICAL_DPP_VGS, "DPP_VGS0", 0, 0, HWC_DISPLAY_PRIMARY_BIT|EXTERNAL_MAIN_DISPLAY_EXTERNAL_BIT},
    {MPP_DPP_VGF, MPP_LOGICAL_DPP_VGF, "DPP_VGF0", 0, 0, HWC_DISPLAY_EXTERNAL_BIT|EXTERNAL_MAIN_DISPLAY_EXTERNAL_BIT},
    {MPP_DPP_VGRFS, MPP_LOGICAL_DPP_VGRFS, "DPP_VGRFS0", 0, 0, HWC_DISPLAY_PRIMARY_BIT|EXTERNAL_MAIN_DISPLAY_EXTERNAL_BIT}
};

const exynos_mpp_t AVAILABLE_M2M_MPP_UNITS[] = {
    {MPP_MSC, MPP_LOGICAL_MSC, "MSC0_PRI", 0, 0, HWC_DISPLAY_PRIMARY_BIT|EXTERNAL_MAIN_DISPLAY_PRIMARY_BIT},
    {MPP_G2D, MPP_LOGICAL_G2D_YUV, "G2D0-YUV_PRI", 0, 0, HWC_DISPLAY_PRIMARY_BIT|EXTERNAL_MAIN_DISPLAY_PRIMARY_BIT},
    {MPP_G2D, MPP_LOGICAL_G2D_YUV, "G2D0-YUV_PRI", 0, 1, HWC_DISPLAY_PRIMARY_BIT|EXTERNAL_MAIN_DISPLAY_PRIMARY_BIT},
    {MPP_G2D, MPP_LOGICAL_G2D_YUV, "G2D0-YUV_EXT", 0, 2, HWC_DISPLAY_EXTERNAL_BIT|EXTERNAL_MAIN_DISPLAY_EXTERNAL_BIT},
    {MPP_G2D, MPP_LOGICAL_G2D_RGB, "G2D0-RGB_PRI", 0, 3, HWC_DISPLAY_PRIMARY_BIT|EXTERNAL_MAIN_DISPLAY_PRIMARY_BIT},
    {MPP_G2D, MPP_LOGICAL_G2D_RGB, "G2D0-RGB_EXT", 0, 4, HWC_DISPLAY_EXTERNAL_BIT|EXTERNAL_MAIN_DISPLAY_EXTERNAL_BIT},
    {MPP_G2D, MPP_LOGICAL_G2D_COMBO, "G2D0-COMBO_VIR", 0, 5, HWC_DISPLAY_VIRTUAL_BIT|EXTERNAL_MAIN_DISPLAY_VIRTUAL_BIT}
};

#endif
