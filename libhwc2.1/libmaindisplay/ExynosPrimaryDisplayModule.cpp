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

#include "ExynosPrimaryDisplayModule.h"
#include "ExynosHWCDebug.h"

#ifdef FORCE_GPU_COMPOSITION
extern exynos_hwc_control exynosHWCControl;
#endif

mpp_phycal_type_t getMPPTypeFromDPPChannel(uint32_t channel) {

    for (int i=0; i < MAX_DECON_DMA_TYPE; i++){
        if(IDMA_CHANNEL_MAP[i].channel == channel)
            return IDMA_CHANNEL_MAP[i].type;
    }

    return MPP_P_TYPE_MAX;
}

ExynosPrimaryDisplayModule::ExynosPrimaryDisplayModule(uint32_t __unused type, ExynosDevice *device)
    :    ExynosPrimaryDisplay(HWC_DISPLAY_PRIMARY, device)
{
#ifdef FORCE_GPU_COMPOSITION
    exynosHWCControl.forceGpu = true;
#endif
}

ExynosPrimaryDisplayModule::~ExynosPrimaryDisplayModule () {
}

void ExynosPrimaryDisplayModule::usePreDefinedWindow(bool use)
{
#ifdef FIX_BASE_WINDOW_INDEX
    /* Use fixed base window index */
    mBaseWindowIndex = FIX_BASE_WINDOW_INDEX;
    return;
#endif

    if (use) {
        mBaseWindowIndex = PRIMARY_DISP_BASE_WIN[mDevice->mDisplayMode];
        mMaxWindowNum = mDisplayInterface->getMaxWindowNum() - PRIMARY_DISP_BASE_WIN[mDevice->mDisplayMode];
    } else {
        mBaseWindowIndex = 0;
        mMaxWindowNum = mDisplayInterface->getMaxWindowNum();
    }
}

int32_t ExynosPrimaryDisplayModule::validateWinConfigData()
{
    bool flagValidConfig = true;

    if (ExynosDisplay::validateWinConfigData() != NO_ERROR)
        flagValidConfig = false;

    for (size_t i = 0; i < mDpuData.configs.size(); i++) {
        struct exynos_win_config_data &config = mDpuData.configs[i];
        if (config.state == config.WIN_STATE_BUFFER) {
            bool configInvalid = false;
            uint32_t mppType = config.assignedMPP->mPhysicalType;
            if ((config.src.w != config.dst.w) ||
                (config.src.h != config.dst.h)) {
                if ((mppType == MPP_DPP_GF) ||
                    (mppType == MPP_DPP_VG) ||
                    (mppType == MPP_DPP_VGF)) {
                    DISPLAY_LOGE("WIN_CONFIG error: invalid assign id : %zu,  s_w : %d, d_w : %d, s_h : %d, d_h : %d, mppType : %d",
                            i, config.src.w, config.dst.w, config.src.h, config.dst.h, mppType);
                    configInvalid = true;
                }
            }
            if (configInvalid) {
                config.state = config.WIN_STATE_DISABLED;
                flagValidConfig = false;
            }
        }
    }
    if (flagValidConfig)
        return NO_ERROR;
    else
        return -EINVAL;
}

void ExynosPrimaryDisplayModule::doPreProcessing() {
    ExynosDisplay::doPreProcessing();
    ExynosDisplay *externalDisplay = mDevice->getDisplay(HWC_DISPLAY_EXTERNAL);
    ExynosDisplay *virtualDisplay = mDevice->getDisplay(HWC_DISPLAY_VIRTUAL);

    if ((externalDisplay->mPlugState) || (virtualDisplay->mPlugState)) {
        mDisplayControl.adjustDisplayFrame = true;
    } else {
        mDisplayControl.adjustDisplayFrame = false;
    }
}
