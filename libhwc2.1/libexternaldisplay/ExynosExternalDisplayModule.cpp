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
//#define LOG_NDEBUG 0
#include "ExynosExternalDisplayModule.h"
#include "ExynosPrimaryDisplayModule.h"

#ifdef USES_VIRTUAL_DISPLAY
#include "ExynosVirtualDisplayModule.h"
#endif

#include "ExynosHWCDebug.h"

#define SKIP_FRAME_COUNT        3

ExynosExternalDisplayModule::ExynosExternalDisplayModule(uint32_t __unused type, ExynosDevice *device)
    :    ExynosExternalDisplay(HWC_DISPLAY_EXTERNAL, device)
{

}

ExynosExternalDisplayModule::~ExynosExternalDisplayModule ()
{

}

decon_idma_type ExynosExternalDisplayModule::getDeconDMAType(ExynosMPP *otfMPP)
{
    ExynosDisplay *primary_display = mDevice->getDisplay(HWC_DISPLAY_PRIMARY);
    if (primary_display == NULL) {
        DISPLAY_LOGE("There is no primary display");
        return MAX_DECON_DMA_TYPE;
    }
    return primary_display->getDeconDMAType(otfMPP);
}

ExynosMPP* ExynosExternalDisplayModule::getExynosMPPForDma(decon_idma_type idma)
{
    ExynosDisplay *primary_display = mDevice->getDisplay(HWC_DISPLAY_PRIMARY);
    if (primary_display == NULL) {
        DISPLAY_LOGE("There is no primary display");
        return NULL;
    }
    return primary_display->getExynosMPPForDma(idma);
}

int32_t ExynosExternalDisplayModule::validateWinConfigData()
{
    struct decon_win_config *config = mWinConfigData->config;
    bool flagValidConfig = true;

    if (ExynosDisplay::validateWinConfigData() != NO_ERROR)
        flagValidConfig = false;

    for (size_t i = 0; i < MAX_DECON_WIN; i++) {
        if (config[i].state == config[i].DECON_WIN_STATE_BUFFER) {
            bool configInvalid = false;
            mpp_phycal_type_t mppType = getMPPTypeFromDPPChannel((uint32_t)config[i].idma_type);
            if ((config[i].src.w != config[i].dst.w) ||
                (config[i].src.h != config[i].dst.h)) {
                if ((mppType == MPP_DPP_GF) ||
                    (mppType == MPP_DPP_VG) ||
                    (mppType == MPP_DPP_VGF)) {
                    DISPLAY_LOGE("WIN_CONFIG error: invalid assign id : %zu,  s_w : %d, d_w : %d, s_h : %d, d_h : %d, channel : %d, mppType : %d",
                            i, config[i].src.w, config[i].dst.w, config[i].src.h, config[i].dst.h,
                            config[i].idma_type, mppType);
                    configInvalid = true;
                }
            }
            if (configInvalid) {
                config[i].state = config[i].DECON_WIN_STATE_DISABLED;
                flagValidConfig = false;
            }
        }
    }
    if (flagValidConfig)
        return NO_ERROR;
    else
        return -EINVAL;
}
