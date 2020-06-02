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
#include "ExynosDisplayDrmInterfaceModule.h"

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
    :    ExynosPrimaryDisplay(HWC_DISPLAY_PRIMARY, device), mDisplayColorLoader(DISPLAY_COLOR_LIB)
{
#ifdef FORCE_GPU_COMPOSITION
    exynosHWCControl.forceGpu = true;
#endif

    mDisplayColorInterface = mDisplayColorLoader.GetDisplayColorGS101();
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
                    DISPLAY_LOGE("WIN_CONFIG error: invalid assign id : "
                            "%zu,  s_w : %d, d_w : %d, s_h : %d, d_h : %d, mppType : %d",
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

int32_t ExynosPrimaryDisplayModule::getColorModes(
        uint32_t* outNumModes, int32_t* outModes)
{
    const ColorModesMap colorModeMap =
        mDisplayColorInterface->ColorModesAndRenderIntents();
    ALOGD("%s: size(%zu)", __func__, colorModeMap.size());
    if (outModes == nullptr) {
        *outNumModes = colorModeMap.size();
        return HWC2_ERROR_NONE;
    }
    if (*outNumModes != colorModeMap.size()) {
        DISPLAY_LOGE("%s: Invalid color mode size(%d), It should be(%zu)",
                __func__, *outNumModes, colorModeMap.size());
        return HWC2_ERROR_BAD_PARAMETER;
    }

    uint32_t index = 0;
    for (const auto &it : colorModeMap)
    {
        outModes[index] = static_cast<int32_t>(it.first);
        ALOGD("\tmode[%d] %d", index, outModes[index]);
        index++;
    }

    return HWC2_ERROR_NONE;
}

int32_t ExynosPrimaryDisplayModule::setColorMode(int32_t mode)
{
    ALOGD("%s: mode(%d)", __func__, mode);
    const ColorModesMap colorModeMap =
        mDisplayColorInterface->ColorModesAndRenderIntents();
    hwc::ColorMode colorMode =
        static_cast<hwc::ColorMode>(mode);
    const auto it = colorModeMap.find(colorMode);
    if (it == colorModeMap.end()) {
        DISPLAY_LOGE("%s: Invalid color mode(%d)", __func__, mode);
        return HWC2_ERROR_BAD_PARAMETER;
    }
    mDisplaySceneInfo.setColorMode(colorMode);

    if (mColorMode != mode)
        setGeometryChanged(GEOMETRY_DISPLAY_COLOR_MODE_CHANGED);
    mColorMode = (android_color_mode_t)mode;

    return HWC2_ERROR_NONE;
}

int32_t ExynosPrimaryDisplayModule::getRenderIntents(int32_t mode,
        uint32_t* outNumIntents, int32_t* outIntents)
{
    const ColorModesMap colorModeMap =
        mDisplayColorInterface->ColorModesAndRenderIntents();
    ALOGD("%s, size(%zu)", __func__, colorModeMap.size());
    hwc::ColorMode colorMode =
        static_cast<hwc::ColorMode>(mode);
    const auto it = colorModeMap.find(colorMode);
    if (it == colorModeMap.end()) {
        DISPLAY_LOGE("%s: Invalid color mode(%d)", __func__, mode);
        return HWC2_ERROR_BAD_PARAMETER;
    }
    auto &renderIntents = it->second;
    if (outIntents == NULL) {
        *outNumIntents = renderIntents.size();
        ALOGD("\tintent num(%zu)", renderIntents.size());
        return HWC2_ERROR_NONE;
    }
    if (*outNumIntents != renderIntents.size()) {
        DISPLAY_LOGE("%s: Invalid intent size(%d), It should be(%zu)",
                __func__, *outNumIntents, renderIntents.size());
        return HWC2_ERROR_BAD_PARAMETER;
    }

    for (uint32_t i = 0; i < renderIntents.size(); i++)
    {
        outIntents[i] = static_cast<uint32_t>(renderIntents[i]);
        ALOGD("\tintent[%d] %d", i, outIntents[i]);
    }

    return HWC2_ERROR_NONE;
}

int32_t ExynosPrimaryDisplayModule::setColorModeWithRenderIntent(int32_t mode,
        int32_t intent)
{
    ALOGD("%s: mode(%d), intent(%d)", __func__, mode, intent);
    const ColorModesMap colorModeMap =
        mDisplayColorInterface->ColorModesAndRenderIntents();
    hwc::ColorMode colorMode =
        static_cast<hwc::ColorMode>(mode);
    hwc::RenderIntent renderIntent =
        static_cast<hwc::RenderIntent>(intent);

    const auto mode_it = colorModeMap.find(colorMode);
    if (mode_it == colorModeMap.end()) {
        DISPLAY_LOGE("%s: Invalid color mode(%d)", __func__, mode);
        return HWC2_ERROR_BAD_PARAMETER;
    }

    auto &renderIntents = mode_it->second;
    auto intent_it = std::find(renderIntents.begin(),
            renderIntents.end(), renderIntent);
    if (intent_it == renderIntents.end()) {
        DISPLAY_LOGE("%s: Invalid render intent(%d)", __func__, intent);
        return HWC2_ERROR_BAD_PARAMETER;
    }

    mDisplaySceneInfo.setColorMode(colorMode);
    mDisplaySceneInfo.setRenderIntent(renderIntent);

    if (mColorMode != mode)
        setGeometryChanged(GEOMETRY_DISPLAY_COLOR_MODE_CHANGED);
    mColorMode = (android_color_mode_t)mode;

    return HWC2_ERROR_NONE;
}

int32_t ExynosPrimaryDisplayModule::setColorTransform(
        const float* matrix, int32_t hint)
{
    if ((hint < HAL_COLOR_TRANSFORM_IDENTITY) ||
        (hint > HAL_COLOR_TRANSFORM_CORRECT_TRITANOPIA))
        return HWC2_ERROR_BAD_PARAMETER;
    ALOGI("%s:: %d, %d", __func__, mColorTransformHint, hint);
    if (mColorTransformHint != hint)
        setGeometryChanged(GEOMETRY_DISPLAY_COLOR_TRANSFORM_CHANGED);
    mColorTransformHint = hint;
#ifdef HWC_SUPPORT_COLOR_TRANSFORM
    mDisplaySceneInfo.setColorTransform(matrix);
#endif
    return HWC2_ERROR_NONE;

}

int32_t ExynosPrimaryDisplayModule::setLayersColorData()
{
    int32_t ret = 0;
    uint32_t layerNum = 0;
    for (uint32_t i = 0; i < mLayers.size(); i++)
    {
        ExynosLayer* layer = mLayers[i];

        if (layer->mValidateCompositionType == HWC2_COMPOSITION_CLIENT)
            continue;

        LayerColorData& layerColorData =
            mDisplaySceneInfo.getLayerColorDataInstance(layerNum);

        /* set layer data mapping info */
        if ((ret = mDisplaySceneInfo.setLayerDataMappingInfo(layer, layerNum))
                != NO_ERROR) {
            DISPLAY_LOGE("%s: layer[%d] setLayerDataMappingInfo fail, layerNum(%d)",
                    __func__, i, layerNum);
            return ret;
        }

        if ((ret = mDisplaySceneInfo.setLayerColorData(layerColorData, layer))
                != NO_ERROR) {
            DISPLAY_LOGE("%s: layer[%d] setLayerColorData fail, layerNum(%d)",
                    __func__, i, layerNum);
            return ret;
        }

        layerNum++;
    }

    /* Resize layer_data when layers were destroyed */
    if (layerNum < mDisplaySceneInfo.displayScene.layer_data.size())
        mDisplaySceneInfo.displayScene.layer_data.resize(layerNum);

    return NO_ERROR;
}

bool ExynosPrimaryDisplayModule::hasDppForLayer(ExynosLayer* layer)
{
    if (mDisplaySceneInfo.layerDataMappingInfo.count(layer) == 0)
        return false;

    uint32_t index =  mDisplaySceneInfo.layerDataMappingInfo[layer];
    if (index >= mDisplayColorInterface->Dpp().size()) {
        DISPLAY_LOGE("%s: invalid dpp index(%d) dpp size(%zu)",
                __func__, index, mDisplayColorInterface->Dpp().size());
        return false;
    }

    return true;
}

const IDisplayColorGS101::IDpp& ExynosPrimaryDisplayModule::getDppForLayer(ExynosLayer* layer)
{
    uint32_t index = mDisplaySceneInfo.layerDataMappingInfo[layer];
    return mDisplayColorInterface->Dpp()[index].get();
}

int32_t ExynosPrimaryDisplayModule::getDppIndexForLayer(ExynosLayer* layer)
{
    if (mDisplaySceneInfo.layerDataMappingInfo.count(layer) == 0)
        return -1;
    uint32_t index = mDisplaySceneInfo.layerDataMappingInfo[layer];

    return static_cast<int32_t>(index);
}

int ExynosPrimaryDisplayModule::deliverWinConfigData()
{
    int ret = 0;
    ExynosDisplayDrmInterfaceModule *moduleDisplayInterface =
        (ExynosDisplayDrmInterfaceModule*)(mDisplayInterface.get());

    moduleDisplayInterface->setColorSettingChanged(
            mDisplaySceneInfo.needDisplayColorSetting());

    ret = ExynosDisplay::deliverWinConfigData();

    return ret;
}

LayerColorData& ExynosPrimaryDisplayModule::DisplaySceneInfo::getLayerColorDataInstance(
        uint32_t index)
{
    size_t currentSize = displayScene.layer_data.size();
    if (index >= currentSize) {
        displayScene.layer_data.resize(currentSize+1);
        colorSettingChanged = true;
    }
    return displayScene.layer_data[index];
}

int32_t ExynosPrimaryDisplayModule::DisplaySceneInfo::setLayerDataMappingInfo(
        ExynosLayer* layer, uint32_t index)
{
    if (layerDataMappingInfo.count(layer) != 0) {
        ALOGE("layer mapping is already inserted (layer: %p, index:%d)",
                layer, index);
        return -EINVAL;
    }
    layerDataMappingInfo.insert(std::make_pair(layer, index));

    return NO_ERROR;
}

void ExynosPrimaryDisplayModule::DisplaySceneInfo::setLayerDataspace(
        LayerColorData& layerColorData,
        hwc::Dataspace dataspace)
{
    if (layerColorData.dataspace != dataspace) {
        colorSettingChanged = true;
        layerColorData.dataspace = dataspace;
    }
}

void ExynosPrimaryDisplayModule::DisplaySceneInfo::disableLayerHdrStaticMetadata(
        LayerColorData& layerColorData)
{
    if (layerColorData.static_metadata.is_valid) {
        colorSettingChanged = true;
        layerColorData.static_metadata.is_valid = false;
    }
}

void ExynosPrimaryDisplayModule::DisplaySceneInfo::setLayerHdrStaticMetadata(
        LayerColorData& layerColorData,
        const ExynosHdrStaticInfo &exynosHdrStaticInfo)
{
    if (layerColorData.static_metadata.is_valid == false) {
        colorSettingChanged = true;
        layerColorData.static_metadata.is_valid = true;
    }

    updateInfoSingleVal(layerColorData.static_metadata.display_red_primary_x,
            exynosHdrStaticInfo.sType1.mR.x);
    updateInfoSingleVal(layerColorData.static_metadata.display_red_primary_y,
            exynosHdrStaticInfo.sType1.mR.y);
    updateInfoSingleVal(layerColorData.static_metadata.display_green_primary_x,
            exynosHdrStaticInfo.sType1.mG.x);
    updateInfoSingleVal(layerColorData.static_metadata.display_green_primary_y,
            exynosHdrStaticInfo.sType1.mG.y);
    updateInfoSingleVal(layerColorData.static_metadata.display_blue_primary_x,
            exynosHdrStaticInfo.sType1.mB.x);
    updateInfoSingleVal(layerColorData.static_metadata.display_blue_primary_y,
            exynosHdrStaticInfo.sType1.mB.y);
    updateInfoSingleVal(layerColorData.static_metadata.white_point_x,
            exynosHdrStaticInfo.sType1.mW.x);
    updateInfoSingleVal(layerColorData.static_metadata.white_point_y,
            exynosHdrStaticInfo.sType1.mW.y);
    updateInfoSingleVal(layerColorData.static_metadata.max_luminance,
            exynosHdrStaticInfo.sType1.mMaxDisplayLuminance);
    updateInfoSingleVal(layerColorData.static_metadata.min_luminance,
            exynosHdrStaticInfo.sType1.mMinDisplayLuminance);
    updateInfoSingleVal(layerColorData.static_metadata.max_content_light_level,
            exynosHdrStaticInfo.sType1.mMaxContentLightLevel);
    updateInfoSingleVal(
            layerColorData.static_metadata.max_frame_average_light_level,
            exynosHdrStaticInfo.sType1.mMaxFrameAverageLightLevel);
}

void ExynosPrimaryDisplayModule::DisplaySceneInfo::setLayerColorTransform(
        LayerColorData& layerColorData,
        std::array<float, TRANSFORM_MAT_SIZE> &matrix)
{
    updateInfoSingleVal(layerColorData.matrix, matrix);
}

void ExynosPrimaryDisplayModule::DisplaySceneInfo::disableLayerHdrDynamicMetadata(
        LayerColorData& layerColorData)
{
    if (layerColorData.dynamic_metadata.is_valid) {
        colorSettingChanged = true;
        layerColorData.dynamic_metadata.is_valid = false;
    }
}

void ExynosPrimaryDisplayModule::DisplaySceneInfo::setLayerHdrDynamicMetadata(
        LayerColorData& layerColorData,
        const ExynosHdrDynamicInfo &exynosHdrDynamicInfo)
{
    if (layerColorData.dynamic_metadata.is_valid == false) {
        colorSettingChanged = true;
        layerColorData.dynamic_metadata.is_valid = true;
    }
    updateInfoSingleVal(layerColorData.dynamic_metadata.display_maximum_luminance,
            exynosHdrDynamicInfo.data.display_maximum_luminance);

    if (!std::equal(layerColorData.dynamic_metadata.maxscl.begin(),
                layerColorData.dynamic_metadata.maxscl.end(),
                exynosHdrDynamicInfo.data.maxscl)) {
        colorSettingChanged = true;
        for (uint32_t i = 0 ; i < layerColorData.dynamic_metadata.maxscl.size(); i++) {
            layerColorData.dynamic_metadata.maxscl[i] =
                exynosHdrDynamicInfo.data.maxscl[i];
        }
    }
    static constexpr uint32_t DYNAMIC_META_DAT_SIZE = 15;

    updateInfoVectorVal(layerColorData.dynamic_metadata.maxrgb_percentages,
            exynosHdrDynamicInfo.data.maxrgb_percentages,
            DYNAMIC_META_DAT_SIZE);
    updateInfoVectorVal(layerColorData.dynamic_metadata.maxrgb_percentiles,
            exynosHdrDynamicInfo.data.maxrgb_percentiles,
            DYNAMIC_META_DAT_SIZE);
    updateInfoSingleVal(layerColorData.dynamic_metadata.tm_flag,
            exynosHdrDynamicInfo.data.tone_mapping.tone_mapping_flag);
    updateInfoSingleVal(layerColorData.dynamic_metadata.tm_knee_x,
            exynosHdrDynamicInfo.data.tone_mapping.knee_point_x);
    updateInfoSingleVal(layerColorData.dynamic_metadata.tm_knee_y,
            exynosHdrDynamicInfo.data.tone_mapping.knee_point_y);
    updateInfoVectorVal(layerColorData.dynamic_metadata.bezier_curve_anchors,
            exynosHdrDynamicInfo.data.tone_mapping.bezier_curve_anchors,
            DYNAMIC_META_DAT_SIZE);
}

int32_t ExynosPrimaryDisplayModule::DisplaySceneInfo::setLayerColorData(
        LayerColorData& layerData, ExynosLayer* layer)
{
    setLayerDataspace(layerData,
            static_cast<hwc::Dataspace>(layer->mDataSpace));
    if (layer->mIsHdrLayer) {
        if (layer->getMetaParcel() == nullptr) {
            HDEBUGLOGE("%s:: meta data parcel is null", __func__);
            return -EINVAL;
        }
        if (layer->getMetaParcel()->eType & VIDEO_INFO_TYPE_HDR_STATIC)
            setLayerHdrStaticMetadata(layerData, layer->getMetaParcel()->sHdrStaticInfo);
        else
            disableLayerHdrStaticMetadata(layerData);

        if (layer->getMetaParcel()->eType & VIDEO_INFO_TYPE_HDR_DYNAMIC)
            setLayerHdrDynamicMetadata(layerData, layer->getMetaParcel()->sHdrDynamicInfo);
        else
            disableLayerHdrDynamicMetadata(layerData);
    } else {
        disableLayerHdrStaticMetadata(layerData);
        disableLayerHdrDynamicMetadata(layerData);
    }

    static std::array<float, TRANSFORM_MAT_SIZE> defaultMatrix {
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0
    };
    if (layer->mLayerColorTransform.enable)
        setLayerColorTransform(layerData,
                layer->mLayerColorTransform.mat);
    else
        setLayerColorTransform(layerData,
                defaultMatrix);

    return NO_ERROR;
}

int32_t ExynosPrimaryDisplayModule::updateColorConversionInfo()
{
    int ret = 0;
    /* clear flag and layer mapping info before setting */
    mDisplaySceneInfo.reset();

    if ((ret = setLayersColorData()) != NO_ERROR)
        return ret;
    if (hwcCheckDebugMessages(eDebugColorManagement))
        mDisplaySceneInfo.printDisplayScene();

    if ((ret = mDisplayColorInterface->Update(mDisplaySceneInfo.displayScene)) != 0) {
        DISPLAY_LOGE("Display Scene update error (%d)", ret);
        return ret;
    }

    return ret;
}

bool ExynosPrimaryDisplayModule::DisplaySceneInfo::needDisplayColorSetting()
{
    /* TODO: Check if we can skip color setting */
    /* For now, propage setting every frame */
    return true;

    if (colorSettingChanged)
        return true;
    if (prev_layerDataMappingInfo != layerDataMappingInfo)
        return true;

    return false;
}

void ExynosPrimaryDisplayModule::DisplaySceneInfo::printDisplayScene()
{
    ALOGD("======================= DisplayScene info ========================");
    ALOGD("dpu_bit_depth: %d", static_cast<uint32_t>(displayScene.dpu_bit_depth));
    ALOGD("color_mode: %d", static_cast<uint32_t>(displayScene.color_mode));
    ALOGD("render_intent: %d", static_cast<uint32_t>(displayScene.render_intent));
    ALOGD("matrix");
    for (uint32_t i = 0; i < 16; (i += 4)) {
        ALOGD("%f, %f, %f, %f",
                displayScene.matrix[i], displayScene.matrix[i+1],
                displayScene.matrix[i+2], displayScene.matrix[i+3]);
    }
    ALOGD("layer: %zu ++++++",
            displayScene.layer_data.size());
    for (uint32_t i = 0; i < displayScene.layer_data.size(); i++) {
        ALOGD("layer[%d] info", i);
        printLayerColorData(displayScene.layer_data[i]);
    }

    ALOGD("layerDataMappingInfo: %zu ++++++",
            layerDataMappingInfo.size());
    for (auto layer : layerDataMappingInfo) {
        ALOGD("[layer: %p] %d", layer.first, layer.second);
    }
}

void ExynosPrimaryDisplayModule::DisplaySceneInfo::printLayerColorData(
    const LayerColorData& layerData)
{
    ALOGD("dataspace: 0x%8x", static_cast<uint32_t>(layerData.dataspace));
    ALOGD("matrix");
    for (uint32_t i = 0; i < 16; (i += 4)) {
        ALOGD("%f, %f, %f, %f",
                layerData.matrix[i], layerData.matrix[i+1],
                layerData.matrix[i+2], layerData.matrix[i+3]);
    }
    ALOGD("static_metadata.is_valid(%d)", layerData.static_metadata.is_valid);
    if (layerData.static_metadata.is_valid) {
        ALOGD("\tdisplay_red_primary(%d, %d)",
                layerData.static_metadata.display_red_primary_x,
                layerData.static_metadata.display_red_primary_y);
        ALOGD("\tdisplay_green_primary(%d, %d)",
                layerData.static_metadata.display_green_primary_x,
                layerData.static_metadata.display_green_primary_y);
        ALOGD("\tdisplay_blue_primary(%d, %d)",
                layerData.static_metadata.display_blue_primary_x,
                layerData.static_metadata.display_blue_primary_y);
        ALOGD("\twhite_point(%d, %d)",
                layerData.static_metadata.white_point_x,
                layerData.static_metadata.white_point_y);
    }
    ALOGD("dynamic_metadata.is_valid(%d)", layerData.dynamic_metadata.is_valid);
    if (layerData.dynamic_metadata.is_valid) {
        ALOGD("\tdisplay_maximum_luminance: %d",
                layerData.dynamic_metadata.display_maximum_luminance);
        ALOGD("\tmaxscl(%d, %d, %d)", layerData.dynamic_metadata.maxscl[0],
                layerData.dynamic_metadata.maxscl[1],
                layerData.dynamic_metadata.maxscl[2]);
        ALOGD("\ttm_flag(%d)", layerData.dynamic_metadata.tm_flag);
        ALOGD("\ttm_knee_x(%d)", layerData.dynamic_metadata.tm_knee_x);
        ALOGD("\ttm_knee_y(%d)", layerData.dynamic_metadata.tm_knee_y);
    }
}
