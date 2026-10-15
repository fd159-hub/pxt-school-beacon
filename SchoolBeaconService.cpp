// Adapted from pizayanz/pxt-linebeacon. MIT license: see LICENSE.
// Nordic SDK-derived implementation: retain third-party/NORDIC-LICENSE.txt.
/**
 * Copyright (c) 2014 - 2020, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "pxt.h"
#include "MicroBitConfig.h"
#if MICROBIT_CODAL
// This implementation owns the SoftDevice. Reject Bluetooth/radio packages that enable it elsewhere.
#if CONFIG_ENABLED(MICROBIT_BLE_ENABLED)
#error "School Beacon cannot be combined with the Bluetooth extension"
#endif
#include "SchoolBeaconService.h"
#include <string.h>
#include "nordic_common.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "ble_gap.h"
#include "nrf_error.h"

#define SCHOOL_CONN_CFG_TAG 1
#define SCHOOL_ADV_INTERVAL MSEC_TO_UNITS(200, UNIT_0_625_MS)
static_assert(SCHOOL_ADV_INTERVAL == 320, "Expected 200 ms in 0.625 ms units");
static_assert(SCHOOL_ADV_INTERVAL >= BLE_GAP_ADV_INTERVAL_MIN && SCHOOL_ADV_INTERVAL <= BLE_GAP_ADV_INTERVAL_MAX, "Advertising interval out of SDK range");
static uint8_t advHandle = BLE_GAP_ADV_SET_HANDLE_NOT_SET;
// SoftDevice retains this pointer, so the payload must outlive start().
static uint8_t payload[30];
static bool stackInitialized = false;

SchoolBeaconService::SchoolBeaconService(): advertising(false), lastError(NRF_SUCCESS) {}

bool SchoolBeaconService::initialize() {
    if (stackInitialized) return true;
    // Detect an already enabled SoftDevice rather than configuring someone else's stack.
    if (nrf_sdh_is_enabled()) { lastError = NRF_ERROR_INVALID_STATE; return false; }
    uint32_t ramStart = 0;
    lastError = nrf_sdh_enable_request();
    if (lastError != NRF_SUCCESS) return false;
    lastError = nrf_sdh_ble_default_cfg_set(SCHOOL_CONN_CFG_TAG, &ramStart);
    if (lastError == NRF_SUCCESS) lastError = nrf_sdh_ble_enable(&ramStart);
    if (lastError != NRF_SUCCESS) {
        // Best-effort rollback so a later start can retry after initialization failure.
        nrf_sdh_disable_request();
        return false;
    }
    stackInitialized = true;
    return true;
}

bool SchoolBeaconService::start(const uint8_t uuid[16], uint16_t major, uint16_t minor) {
    lastError = NRF_SUCCESS;
    if (!initialize()) return false;
    if (advertising && !stop()) return false;
    // Flags (3 bytes) + Manufacturer Specific Data AD structure (27 bytes).
    payload[0] = 0x02; payload[1] = 0x01; payload[2] = 0x06;
    payload[3] = 0x1A; payload[4] = 0xFF;
    payload[5] = 0x4C; payload[6] = 0x00; // Apple Company ID, little endian.
    payload[7] = 0x02; payload[8] = 0x15;
    memcpy(&payload[9], uuid, 16);
    payload[25] = (major >> 8) & 0xff; payload[26] = major & 0xff;
    payload[27] = (minor >> 8) & 0xff; payload[28] = minor & 0xff;
    payload[29] = (uint8_t)(int8_t)-59; // Measured power placeholder, NOT radio TX power.

    ble_gap_adv_params_t params;
    memset(&params, 0, sizeof(params));
    params.properties.type = BLE_GAP_ADV_TYPE_NONCONNECTABLE_NONSCANNABLE_UNDIRECTED;
    params.filter_policy = BLE_GAP_ADV_FP_ANY;
    params.interval = SCHOOL_ADV_INTERVAL;
    params.duration = 0;
    params.primary_phy = BLE_GAP_PHY_1MBPS;
    params.secondary_phy = BLE_GAP_PHY_1MBPS;
    ble_gap_adv_data_t data;
    memset(&data, 0, sizeof(data));
    data.adv_data.p_data = payload;
    data.adv_data.len = sizeof(payload);
    lastError = sd_ble_gap_adv_set_configure(&advHandle, &data, &params);
    if (lastError != NRF_SUCCESS) return false;
    lastError = sd_ble_gap_adv_start(advHandle, SCHOOL_CONN_CFG_TAG);
    advertising = lastError == NRF_SUCCESS;
    return advertising;
}

bool SchoolBeaconService::stop() {
    lastError = NRF_SUCCESS;
    if (!advertising) return true;
    lastError = sd_ble_gap_adv_stop(advHandle);
    if (lastError != NRF_SUCCESS) return false;
    advertising = false;
    return true;
}
#endif
