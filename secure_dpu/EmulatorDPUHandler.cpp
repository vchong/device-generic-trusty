/*
 * Copyright 2020, The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "DPUHandler.h"
#include "SecureDpu.h"

#include <android-base/logging.h>
#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <unistd.h>
#include <trusty/tipc.h>

namespace android {
namespace trusty {
namespace secure_dpu {

DPUHandler::DPUHandler() {
}

DPUHandler::~DPUHandler() {
    tipc_close(dpu_handle_);
}

android::base::Result<void> DPUHandler::Init(std::string device_name) {
    dpu_handle_ = tipc_connect(device_name.c_str(),
                               SECURE_DPU_PORT_NAME);
    if (dpu_handle_ < 0) {
        return base::Error() << "Failed to connect to: " << device_name;
    }
    return {};
}

android::base::Result<void> DPUHandler::HandleStartSecureDisplay() {
    LOG(INFO) << "Started Secure Display.";
    return {};
}

android::base::Result<void> DPUHandler::HandleStopSecureDisplay() {
    LOG(INFO) << "Stopped Secure Display.";
    return {};
}

android::base::Result<void> DPUHandler::HandleCmd(const void* in_buf,
                                                  const size_t in_size,
                                                  void* out_buf,
                                                  size_t &out_size) {
    if (in_size < sizeof(struct secure_dpu_req)) {
        return base::Error() << "Invalid payload";
    }
    const struct secure_dpu_req* req = reinterpret_cast<const struct secure_dpu_req*>(in_buf);
    switch (req->cmd) {
        case SECURE_DPU_CMD_START_SECURE_DISPLAY: {
            struct secure_dpu_resp* rsp = reinterpret_cast<struct secure_dpu_resp*>(out_buf);
            rsp->cmd = SECURE_DPU_CMD_START_SECURE_DISPLAY | SECURE_DPU_CMD_RESP_BIT;

            auto result = HandleStartSecureDisplay();
            if (result.ok()) {
                rsp->status = SECURE_DPU_ERROR_OK;
            } else {
                rsp->status = SECURE_DPU_ERROR_FAIL;
            }

            out_size = sizeof(struct secure_dpu_resp);
            break;
        }
        case SECURE_DPU_CMD_STOP_SECURE_DISPLAY: {
            struct secure_dpu_resp* rsp = reinterpret_cast<struct secure_dpu_resp*>(out_buf);
            rsp->cmd = SECURE_DPU_CMD_STOP_SECURE_DISPLAY | SECURE_DPU_CMD_RESP_BIT;

            auto result = HandleStopSecureDisplay();
            if (result.ok()) {
                rsp->status = SECURE_DPU_ERROR_OK;
            } else {
                rsp->status = SECURE_DPU_ERROR_FAIL;
            }

            out_size = sizeof(struct secure_dpu_resp);
            break;
        }
        default:
            LOG(ERROR) << "Unknown command: " << (uint32_t)req->cmd;
            return base::Error() << "Unknown command";
    }
    return {};
}

android::base::Result<void> DPUHandler::Handle() {
    uint8_t in_buf[SECURE_DPU_MAX_MSG_SIZE];
    uint8_t out_buf[SECURE_DPU_MAX_MSG_SIZE];
    size_t out_size = 0;

    auto read_len = read(dpu_handle_, in_buf, sizeof(in_buf));
    if (read_len < 0) {
        return base::Error() << "Failed to read command";
    }
    auto result = HandleCmd(in_buf, read_len, out_buf, out_size);
    if (!result.ok()) {
        return base::Error() << "Failed to handle command";
    }
    auto write_len = write(dpu_handle_, out_buf, out_size);
    if (write_len != out_size) {
        return base::Error() << "Failed to write command";
    }
    return {};
}

}  // namespace secure_dpu
}  // namespace trusty
}  // namespace android
