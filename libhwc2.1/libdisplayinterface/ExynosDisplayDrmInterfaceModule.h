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

#ifndef EXYNOS_DISPLAY_DRM_INTERFACE_MODULE_H
#define EXYNOS_DISPLAY_DRM_INTERFACE_MODULE_H

#include <gs101/displaycolor/displaycolor_gs101.h>

#include "ExynosDisplayDrmInterface.h"

using namespace displaycolor;

class ExynosDisplayDrmInterfaceModule : public ExynosDisplayDrmInterface {
    public:
        ExynosDisplayDrmInterfaceModule(ExynosDisplay *exynosDisplay);
        virtual ~ExynosDisplayDrmInterfaceModule();

        virtual int32_t setDisplayColorSetting(
                ExynosDisplayDrmInterface::DrmModeAtomicReq &drmReq);
        virtual int32_t setPlaneColorSetting(
                ExynosDisplayDrmInterface::DrmModeAtomicReq &drmReq,
                const std::unique_ptr<DrmPlane> &plane,
                const exynos_win_config_data &config);
        void setColorSettingChanged(bool changed) {
            mColorSettingChanged = changed; };
        void destroyOldBlobs(std::vector<uint32_t> &oldBlobs);

        int32_t createCgcBlobFromIDqe(const IDisplayColorGS101::IDqe &dqe,
                uint32_t &blobId);
        int32_t createDegammaLutBlobFromIDqe(const IDisplayColorGS101::IDqe &dqe,
                uint32_t &blobId);
        int32_t createRegammaLutBlobFromIDqe(const IDisplayColorGS101::IDqe &dqe,
                uint32_t &blobId);
        int32_t createGammaMatBlobFromIDqe(const IDisplayColorGS101::IDqe &dqe,
                uint32_t &blobId);
        int32_t createLinearMatBlobFromIDqe(const IDisplayColorGS101::IDqe &dqe,
                uint32_t &blobId);

        int32_t createEotfBlobFromIDpp(const IDisplayColorGS101::IDpp &dpp,
                uint32_t &blobId);
        int32_t createGmBlobFromIDpp(const IDisplayColorGS101::IDpp &dpp,
                uint32_t &blobId);
        int32_t createDtmBlobFromIDpp(const IDisplayColorGS101::IDpp &dpp,
                uint32_t &blobId);
        int32_t createOetfBlobFromIDpp(const IDisplayColorGS101::IDpp &dpp,
                uint32_t &blobId);
    private:
        bool mColorSettingChanged = false;
};

class ExynosPrimaryDisplayDrmInterfaceModule : public ExynosDisplayDrmInterfaceModule {
    public:
        ExynosPrimaryDisplayDrmInterfaceModule(ExynosDisplay *exynosDisplay);
        virtual ~ExynosPrimaryDisplayDrmInterfaceModule();
};

class ExynosExternalDisplayDrmInterfaceModule : public ExynosDisplayDrmInterfaceModule {
    public:
        ExynosExternalDisplayDrmInterfaceModule(ExynosDisplay *exynosDisplay);
        virtual ~ExynosExternalDisplayDrmInterfaceModule();
};
#endif
