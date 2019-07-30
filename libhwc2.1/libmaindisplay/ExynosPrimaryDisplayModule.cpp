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
}

ExynosPrimaryDisplayModule::~ExynosPrimaryDisplayModule () {
}

void ExynosPrimaryDisplayModule::usePreDefinedWindow(bool use)
{
    if (use) {
        mBaseWindowIndex = PRIMARY_DISP_BASE_WIN[mDevice->mDisplayMode];
        mMaxWindowNum = NUM_HW_WINDOWS - PRIMARY_DISP_BASE_WIN[mDevice->mDisplayMode];
    } else {
        mBaseWindowIndex = 0;
        mMaxWindowNum = NUM_HW_WINDOWS;
    }
}

decon_idma_type ExynosPrimaryDisplayModule::getDeconDMAType(ExynosMPP *otfMPP)
{
    return getDPPChannel(otfMPP);
}

ExynosMPP* ExynosPrimaryDisplayModule::getExynosMPPForDma(decon_idma_type channel)
{
    mpp_phycal_type_t mppType = getMPPTypeFromDPPChannel((uint32_t)channel);
    switch (mppType) {
        case MPP_DPP_GF:
            return ExynosResourceManager::getExynosMPP(MPP_LOGICAL_DPP_GF);
        case MPP_DPP_VG:
            return ExynosResourceManager::getExynosMPP(MPP_LOGICAL_DPP_VG);
        case MPP_DPP_VGS:
            return ExynosResourceManager::getExynosMPP(MPP_LOGICAL_DPP_VGS);
        case MPP_DPP_VGF:
            return ExynosResourceManager::getExynosMPP(MPP_LOGICAL_DPP_VGF);
        case MPP_DPP_VGRFS:
            return ExynosResourceManager::getExynosMPP(MPP_LOGICAL_DPP_VGRFS);
        default:
            return NULL;
    }
}

int32_t ExynosPrimaryDisplayModule::validateWinConfigData()
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

decon_idma_type ExynosPrimaryDisplayModule::getDPPChannel(ExynosMPP *otfMPP) {
    if (otfMPP == NULL)
        return MAX_DECON_DMA_TYPE;

    for (int i=0; i < MAX_DECON_DMA_TYPE; i++){
        if((IDMA_CHANNEL_MAP[i].type == otfMPP->mPhysicalType) &&
           (IDMA_CHANNEL_MAP[i].index == otfMPP->mPhysicalIndex))
            return IDMA_CHANNEL_MAP[i].channel;
    }

    return MAX_DECON_DMA_TYPE;
}
