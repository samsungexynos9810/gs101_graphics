/*
 * Copyright (C) 2020 The Android Open Source Project
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

#include "ExynosDisplayDrmInterfaceModule.h"
#include "ExynosPrimaryDisplayModule.h"
#include <drm/samsung_drm.h>

template <typename T, typename M>
int32_t convertDqeMatrixDataToMatrix(
        const IDisplayColorGS101::MatrixData<T> &colorMatrix,
        M &mat, uint32_t dimension)
{
    if (colorMatrix.coeffs.size() != (dimension * dimension)) {
        HWC_LOGE(nullptr, "Invalid coeff size(%zu)",
                colorMatrix.coeffs.size());
        return -EINVAL;
    }
    for (uint32_t i = 0; i < (dimension * dimension); i++) {
        mat.coeffs[i] = colorMatrix.coeffs[i];
    }

    if (colorMatrix.offsets.size() != dimension) {
        HWC_LOGE(nullptr, "Invalid offset size(%zu)",
                colorMatrix.offsets.size());
        return -EINVAL;
    }
    for (uint32_t i = 0; i < dimension; i++) {
        mat.offsets[i] = colorMatrix.offsets[i];
    }
    return NO_ERROR;
}

/////////////////////////////////////////////////// ExynosDisplayDrmInterfaceModule //////////////////////////////////////////////////////////////////
ExynosDisplayDrmInterfaceModule::ExynosDisplayDrmInterfaceModule(ExynosDisplay *exynosDisplay)
: ExynosDisplayDrmInterface(exynosDisplay)
{
}

ExynosDisplayDrmInterfaceModule::~ExynosDisplayDrmInterfaceModule()
{
}

void ExynosDisplayDrmInterfaceModule::destroyOldBlobs(
        std::vector<uint32_t> &oldBlobs)
{
    for (auto &blob : oldBlobs) {
        mDrmDevice->DestroyPropertyBlob(blob);
    }
    oldBlobs.clear();
}

int32_t ExynosDisplayDrmInterfaceModule::createCgcBlobFromIDqe(
        const IDisplayColorGS101::IDqe &dqe, uint32_t &blobId)
{
    struct cgc_lut cgc;
    const IDisplayColorGS101::IDqe::CgcData &cgcData = dqe.Cgc();
    if ((cgcData.r_values.size() != DRM_SAMSUNG_CGC_LUT_REG_CNT) ||
        (cgcData.g_values.size() != DRM_SAMSUNG_CGC_LUT_REG_CNT) ||
        (cgcData.b_values.size() != DRM_SAMSUNG_CGC_LUT_REG_CNT)) {
        ALOGE("CGC data size is not same (r: %zu, g: %zu: b: %zu)",
                cgcData.r_values.size(), cgcData.g_values.size(),
                cgcData.b_values.size());
        return -EINVAL;
    }

    for (uint32_t i = 0; i < DRM_SAMSUNG_CGC_LUT_REG_CNT; i++) {
        cgc.r_values[i] = cgcData.r_values[i];
        cgc.g_values[i] = cgcData.g_values[i];
        cgc.b_values[i] = cgcData.b_values[i];
    }
    int ret = mDrmDevice->CreatePropertyBlob(&cgc, sizeof(cgc_lut), &blobId);
    if (ret) {
        HWC_LOGE(mExynosDisplay, "Failed to create cgc blob %d", ret);
        return ret;
    }
    return NO_ERROR;
}

int32_t ExynosDisplayDrmInterfaceModule::createDegammaLutBlobFromIDqe(
        const IDisplayColorGS101::IDqe &dqe, uint32_t &blobId)
{
    int ret = 0;
    uint64_t lut_size = 0;
    std::tie(ret, lut_size) = mDrmCrtc->degamma_lut_size_property().value();
    if (ret < 0) {
         HWC_LOGE(mExynosDisplay, "%s: there is no degamma_lut_size (ret = %d)",
                 __func__, ret);
         return ret;
    }
    if (lut_size != IDisplayColorGS101::IDqe::DegammaLutData::kLutLen) {
        HWC_LOGE(mExynosDisplay, "%s: invalid lut size (%" PRId64 ")",
                __func__, lut_size);
        return -EINVAL;
    }

    struct drm_color_lut color_lut[IDisplayColorGS101::IDqe::DegammaLutData::kLutLen];
    for (uint32_t i = 0; i < lut_size; i++) {
        color_lut[i].red = dqe.DegammaLut().values[i];
    }
    ret = mDrmDevice->CreatePropertyBlob(color_lut, sizeof(color_lut), &blobId);
    if (ret) {
        HWC_LOGE(mExynosDisplay, "Failed to create degamma lut blob %d", ret);
        return ret;
    }
    return NO_ERROR;
}

int32_t ExynosDisplayDrmInterfaceModule::createRegammaLutBlobFromIDqe(
        const IDisplayColorGS101::IDqe &dqe, uint32_t &blobId)
{
    int ret = 0;
    uint64_t lut_size = 0;
    std::tie(ret, lut_size) = mDrmCrtc->gamma_lut_size_property().value();
    if (ret < 0) {
         HWC_LOGE(mExynosDisplay, "%s: there is no gamma_lut_size (ret = %d)",
                 __func__, ret);
         return ret;
    }
    if (lut_size != IDisplayColorGS101::IDqe::DegammaLutData::kLutLen) {
        HWC_LOGE(mExynosDisplay, "%s: invalid lut size (%" PRId64 ")",
                __func__, lut_size);
        return -EINVAL;
    }

    struct drm_color_lut color_lut[IDisplayColorGS101::IDqe::DegammaLutData::kLutLen];
    for (uint32_t i = 0; i < lut_size; i++) {
        color_lut[i].red = dqe.RegammaLut().r_values[i];
        color_lut[i].green = dqe.RegammaLut().g_values[i];
        color_lut[i].blue = dqe.RegammaLut().b_values[i];
    }
    ret = mDrmDevice->CreatePropertyBlob(color_lut, sizeof(color_lut), &blobId);
    if (ret) {
        HWC_LOGE(mExynosDisplay, "Failed to create gamma lut blob %d", ret);
        return ret;
    }
    return NO_ERROR;
}

int32_t ExynosDisplayDrmInterfaceModule::createGammaMatBlobFromIDqe(
        const IDisplayColorGS101::IDqe &dqe, uint32_t &blobId)
{
    /* TODO: This function should be implemented */
    return NO_ERROR;
}

int32_t ExynosDisplayDrmInterfaceModule::createLinearMatBlobFromIDqe(
        const IDisplayColorGS101::IDqe &dqe, uint32_t &blobId)
{
    /* TODO: This function should be implemented */
    return NO_ERROR;
}

int32_t ExynosDisplayDrmInterfaceModule::createEotfBlobFromIDpp(
        const IDisplayColorGS101::IDpp &dpp, uint32_t &blobId)
{
    struct hdr_eotf_lut eotf_lut;
    if ((dpp.EotfLut().posx.size() != DRM_SAMSUNG_HDR_EOTF_LUT_LEN) ||
        (dpp.EotfLut().posy.size() != DRM_SAMSUNG_HDR_EOTF_LUT_LEN)) {
        HWC_LOGE(mExynosDisplay, "%s: eotf pos size (%zu, %zu)",
                __func__, dpp.EotfLut().posx.size(), dpp.EotfLut().posy.size());
        return -EINVAL;
    }
    for (uint32_t i = 0; i < DRM_SAMSUNG_HDR_EOTF_LUT_LEN; i++) {
        eotf_lut.posx[i] = dpp.EotfLut().posx[i];
        eotf_lut.posy[i] = dpp.EotfLut().posy[i];
    }
    int ret = mDrmDevice->CreatePropertyBlob(&eotf_lut, sizeof(eotf_lut), &blobId);
    if (ret) {
        HWC_LOGE(mExynosDisplay, "Failed to create eotf lut blob %d", ret);
        return ret;
    }
    return NO_ERROR;
}

int32_t ExynosDisplayDrmInterfaceModule::createGmBlobFromIDpp(
        const IDisplayColorGS101::IDpp &dpp, uint32_t &blobId)
{
    int ret = 0;
    struct hdr_gm_data gm_matrix;
    if ((ret = convertDqeMatrixDataToMatrix(
                    dpp.Gm(), gm_matrix, DRM_SAMSUNG_HDR_GM_DIMENS)) != NO_ERROR)
    {
        HWC_LOGE(mExynosDisplay, "Failed to convert gm matrix");
        return ret;
    }
    ret = mDrmDevice->CreatePropertyBlob(&gm_matrix, sizeof(gm_matrix), &blobId);
    if (ret) {
        HWC_LOGE(mExynosDisplay, "Failed to create gm matrix blob %d", ret);
        return ret;
    }
    return NO_ERROR;
}

int32_t ExynosDisplayDrmInterfaceModule::createDtmBlobFromIDpp(
        const IDisplayColorGS101::IDpp &dpp, uint32_t &blobId)
{
    struct hdr_tm_data tm_data;
    if ((dpp.Dtm().posx.size() != DRM_SAMSUNG_HDR_TM_LUT_LEN) ||
        (dpp.Dtm().posy.size() != DRM_SAMSUNG_HDR_TM_LUT_LEN)) {
        HWC_LOGE(mExynosDisplay, "%s: dtm pos size (%zu, %zu)",
                __func__, dpp.Dtm().posx.size(), dpp.Dtm().posy.size());
        return -EINVAL;
    }

    for (uint32_t i = 0; i < DRM_SAMSUNG_HDR_TM_LUT_LEN; i++) {
        tm_data.posx[i] = dpp.Dtm().posx[i];
        tm_data.posy[i] = dpp.Dtm().posy[i];
    }

    tm_data.coeff_r = dpp.Dtm().coeff_r;
    tm_data.coeff_g = dpp.Dtm().coeff_g;
    tm_data.coeff_b = dpp.Dtm().coeff_b;
    tm_data.rng_x_min = dpp.Dtm().rng_x_min;
    tm_data.rng_x_max = dpp.Dtm().rng_x_max;
    tm_data.rng_y_min = dpp.Dtm().rng_y_min;
    tm_data.rng_y_max = dpp.Dtm().rng_y_max;

    int ret = mDrmDevice->CreatePropertyBlob(&tm_data, sizeof(tm_data), &blobId);
    if (ret) {
        HWC_LOGE(mExynosDisplay, "Failed to create tm_data blob %d", ret);
        return ret;
    }

    return NO_ERROR;
}
int32_t ExynosDisplayDrmInterfaceModule::createOetfBlobFromIDpp(
        const IDisplayColorGS101::IDpp &dpp, uint32_t &blobId)
{
    struct hdr_oetf_lut oetf_lut;
    if ((dpp.OetfLut().posx.size() != DRM_SAMSUNG_HDR_OETF_LUT_LEN) ||
        (dpp.OetfLut().posy.size() != DRM_SAMSUNG_HDR_OETF_LUT_LEN)) {
        HWC_LOGE(mExynosDisplay, "%s: oetf pos size (%zu, %zu)",
                __func__, dpp.OetfLut().posx.size(), dpp.OetfLut().posy.size());
        return -EINVAL;
    }
    for (uint32_t i = 0; i < DRM_SAMSUNG_HDR_OETF_LUT_LEN; i++) {
        oetf_lut.posx[i] = dpp.OetfLut().posx[i];
        oetf_lut.posy[i] = dpp.OetfLut().posy[i];
    }
    int ret = mDrmDevice->CreatePropertyBlob(&oetf_lut, sizeof(oetf_lut), &blobId);
    if (ret) {
        HWC_LOGE(mExynosDisplay, "Failed to create oetf lut blob %d", ret);
        return ret;
    }
    return NO_ERROR;
}

int32_t ExynosDisplayDrmInterfaceModule::setDisplayColorSetting(
        ExynosDisplayDrmInterface::DrmModeAtomicReq &drmReq)
{
    if ((mColorSettingChanged == false) ||
        (isPrimary() == false))
        return NO_ERROR;

    ExynosPrimaryDisplayModule* display =
        (ExynosPrimaryDisplayModule*)mExynosDisplay;

    int ret = NO_ERROR;
    uint32_t blobId = 0;
    const IDisplayColorGS101::IDqe &dqe = display->getDqe();

    if (mDrmCrtc->cgc_lut_property().id()) {
        blobId = 0;
        if (dqe.Cgc().enable) {
            if ((ret = createCgcBlobFromIDqe(dqe, blobId)) != NO_ERROR) {
                HWC_LOGE(mExynosDisplay, "%s: createCgcBlobFromIDqe fail", __func__);
                return ret;
            }
            drmReq.addOldBlob(blobId);
        }
        if ((ret = drmReq.atomicAddProperty(mDrmCrtc->id(),
                        mDrmCrtc->cgc_lut_property(), blobId)) < 0) {
            HWC_LOGE(mExynosDisplay, "%s: Fail to set cgc_lut_property",
                    __func__);
            return ret;
        }
    }
    if (mDrmCrtc->degamma_lut_property().id()) {
        blobId = 0;
        if (dqe.DegammaLut().enable) {
            if ((ret = createDegammaLutBlobFromIDqe(dqe, blobId)) != NO_ERROR) {
                HWC_LOGE(mExynosDisplay, "%s: createDegammaLutBlobFromIDqe fail",
                        __func__);
                return ret;
            }
            drmReq.addOldBlob(blobId);
        }
        if ((ret = drmReq.atomicAddProperty(mDrmCrtc->id(),
                        mDrmCrtc->degamma_lut_property(), blobId)) < 0) {
            HWC_LOGE(mExynosDisplay, "%s: Fail to set degamma_lut_property",
                    __func__);
            return ret;
        }
    }
    if (mDrmCrtc->gamma_lut_property().id()) {
        blobId = 0;
        if (dqe.RegammaLut().enable) {
            if ((ret = createRegammaLutBlobFromIDqe(dqe, blobId)) != NO_ERROR) {
                HWC_LOGE(mExynosDisplay, "%s: createRegammaLutBlobFromIDqe fail",
                        __func__);
                return ret;
            }
            drmReq.addOldBlob(blobId);
        }
        if ((ret = drmReq.atomicAddProperty(mDrmCrtc->id(),
                        mDrmCrtc->gamma_lut_property(), blobId)) < 0) {
            HWC_LOGE(mExynosDisplay, "%s: Fail to set gamma_lut_property",
                    __func__);
            return ret;
        }
    }
    if (mDrmCrtc->gamma_matrix_property().id()) {
        blobId = 0;
        if (dqe.GammaMatrix().enable) {
            if ((ret = createGammaMatBlobFromIDqe(dqe, blobId)) != NO_ERROR) {
                HWC_LOGE(mExynosDisplay, "%s: createGammaMatBlobFromIDqe fail",
                        __func__);
                return ret;
            }
            drmReq.addOldBlob(blobId);
        }
        if ((ret = drmReq.atomicAddProperty(mDrmCrtc->id(),
                        mDrmCrtc->gamma_matrix_property(), blobId)) < 0) {
            HWC_LOGE(mExynosDisplay, "%s: Fail to set gamma_matrix_property",
                    __func__);
            return ret;
        }
    }
    if (mDrmCrtc->linear_matrix_property().id()) {
        blobId = 0;
        if (dqe.LinearMatrix().enable) {
            if ((ret = createLinearMatBlobFromIDqe(dqe, blobId)) != NO_ERROR) {
                HWC_LOGE(mExynosDisplay, "%s: createLinearMatBlobFromIDqe fail",
                        __func__);
                return ret;
            }
            drmReq.addOldBlob(blobId);
        }
        if ((ret = drmReq.atomicAddProperty(mDrmCrtc->id(),
                        mDrmCrtc->linear_matrix_property(), blobId)) < 0) {
            HWC_LOGE(mExynosDisplay, "%s: Fail to set linear_matrix_property",
                    __func__);
            return ret;
        }
    }

    return NO_ERROR;
}

int32_t ExynosDisplayDrmInterfaceModule::setPlaneColorSetting(
        ExynosDisplayDrmInterface::DrmModeAtomicReq &drmReq,
        const std::unique_ptr<DrmPlane> &plane,
        const exynos_win_config_data &config)
{
    if ((mColorSettingChanged == false) ||
        (isPrimary() == false))
        return NO_ERROR;

    if ((config.assignedMPP == nullptr) ||
        (config.assignedMPP->mAssignedSources.size() == 0)) {
        HWC_LOGE(mExynosDisplay, "%s:: config's mpp source size is invalid",
                __func__);
        return -EINVAL;
    }
    ExynosMPPSource* mppSource = config.assignedMPP->mAssignedSources[0];

    /*
     * Color conversion of Client and Exynos composition buffer
     * is already addressed by GLES or G2D
     */
    if (mppSource->mSourceType == MPP_SOURCE_COMPOSITION_TARGET)
        return NO_ERROR;

    if (mppSource->mSourceType != MPP_SOURCE_LAYER) {
        HWC_LOGE(mExynosDisplay, "%s:: invalid mpp source type (%d)",
                __func__, mppSource->mSourceType);
        return -EINVAL;
    }

    ExynosLayer* layer = (ExynosLayer*)mppSource;

    /* color conversion was already handled by m2mMPP */
    if ((layer->mM2mMPP != nullptr) &&
        (layer->mSrcImg.dataSpace != layer->mMidImg.dataSpace)) {
        return NO_ERROR;
    }

    ExynosPrimaryDisplayModule* display =
        (ExynosPrimaryDisplayModule*)mExynosDisplay;

    if (display->hasDppForLayer(layer) == false) {
        HWC_LOGE(mExynosDisplay,
                "%s: layer need color conversion but there is no IDpp",
                __func__);
        return -EINVAL;
    }

    const IDisplayColorGS101::IDpp &dpp = display->getDppForLayer(layer);
    uint32_t blobId = 0;
    int ret = 0;

    if (plane->eotf_lut_property().id())
    {
        blobId = 0;
        if (dpp.EotfLut().enable) {
            if ((ret = createEotfBlobFromIDpp(dpp, blobId)) != NO_ERROR) {
                HWC_LOGE(mExynosDisplay, "%s: createEotfBlobFromIDpp fail",
                        __func__);
                return ret;
            }
            drmReq.addOldBlob(blobId);
        }
        if ((ret = drmReq.atomicAddProperty(plane->id(),
                        plane->eotf_lut_property(), blobId)) < 0) {
            HWC_LOGE(mExynosDisplay, "%s: Fail to set eotf_lut_property",
                    __func__);
            return ret;
        }
    }

    if (plane->gammut_matrix_property().id())
    {
        blobId = 0;
        if (dpp.Gm().enable) {
            if ((ret = createGmBlobFromIDpp(dpp, blobId)) != NO_ERROR) {
                HWC_LOGE(mExynosDisplay, "%s: createGmBlobFromIDpp fail",
                        __func__);
                return ret;
            }
            drmReq.addOldBlob(blobId);
        }
        if ((ret = drmReq.atomicAddProperty(plane->id(),
                        plane->gammut_matrix_property(), blobId)) < 0) {
            HWC_LOGE(mExynosDisplay, "%s: Fail to set gammut_matrix_property",
                    __func__);
            return ret;
        }
    }

    if (plane->tone_mapping_property().id())
    {
        blobId = 0;
        if (dpp.Dtm().enable) {
            if ((ret = createDtmBlobFromIDpp(dpp, blobId)) != NO_ERROR) {
                HWC_LOGE(mExynosDisplay, "%s: createDtmBlobFromIDpp fail",
                        __func__);
                return ret;
            }
            drmReq.addOldBlob(blobId);
        }
        if ((ret = drmReq.atomicAddProperty(plane->id(),
                        plane->tone_mapping_property(), blobId)) < 0) {
            HWC_LOGE(mExynosDisplay, "%s: Fail to set tone_mapping_property",
                    __func__);
            return ret;
        }
    }

    if (plane->oetf_lut_property().id())
    {
        blobId = 0;
        if (dpp.OetfLut().enable) {
            if ((ret = createOetfBlobFromIDpp(dpp, blobId)) != NO_ERROR) {
                HWC_LOGE(mExynosDisplay, "%s: createOetfBlobFromIDpp fail",
                        __func__);
                return ret;
            }
            drmReq.addOldBlob(blobId);
        }
        if ((ret = drmReq.atomicAddProperty(plane->id(),
                        plane->oetf_lut_property(), blobId)) < 0) {
            HWC_LOGE(mExynosDisplay, "%s: Fail to set oetf_lut_property",
                    __func__);
            return ret;
        }
    }

    return 0;
}

//////////////////////////////////////////////////// ExynosPrimaryDisplayDrmInterfaceModule //////////////////////////////////////////////////////////////////
ExynosPrimaryDisplayDrmInterfaceModule::ExynosPrimaryDisplayDrmInterfaceModule(ExynosDisplay *exynosDisplay)
: ExynosDisplayDrmInterfaceModule(exynosDisplay)
{
}

ExynosPrimaryDisplayDrmInterfaceModule::~ExynosPrimaryDisplayDrmInterfaceModule()
{
}

//////////////////////////////////////////////////// ExynosExternalDisplayDrmInterfaceModule //////////////////////////////////////////////////////////////////
ExynosExternalDisplayDrmInterfaceModule::ExynosExternalDisplayDrmInterfaceModule(ExynosDisplay *exynosDisplay)
: ExynosDisplayDrmInterfaceModule(exynosDisplay)
{
}

ExynosExternalDisplayDrmInterfaceModule::~ExynosExternalDisplayDrmInterfaceModule()
{
}
