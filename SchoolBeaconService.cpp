// Experimental Nordic nRF52 / micro:bit V2 SoftDevice advertising implementation.
// Based conceptually on pizayanz/pxt-linebeacon (MIT); BLE SDK usage follows Nordic SDK terms.
#include "MicroBitConfig.h"
#if MICROBIT_CODAL
#include "SchoolBeaconService.h"
#include <string.h>
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "ble_gap.h"
#include "nrf_error.h"

#define SCHOOL_CONN_CFG_TAG 1
#define SCHOOL_ADV_INTERVAL MSEC_TO_UNITS(200, UNIT_0_625_MS)
static uint8_t advHandle = BLE_GAP_ADV_SET_HANDLE_NOT_SET;
static uint8_t payload[30];
static bool stackInitialized = false;

SchoolBeaconService::SchoolBeaconService(): advertising(false) {
    if (!stackInitialized) {
        uint32_t ramStart = 0;
        MICROBIT_BLE_ECHK(nrf_sdh_enable_request());
        MICROBIT_BLE_ECHK(nrf_sdh_ble_default_cfg_set(SCHOOL_CONN_CFG_TAG, &ramStart));
        MICROBIT_BLE_ECHK(nrf_sdh_ble_enable(&ramStart));
        stackInitialized = true;
    }
}

void SchoolBeaconService::start(const uint8_t uuid[16], uint16_t major, uint16_t minor, int8_t txPower) {
    if (advertising) stop();
    // Flags AD structure (3 bytes) + Manufacturer Specific Data AD structure (27 bytes).
    payload[0] = 0x02; payload[1] = 0x01; payload[2] = 0x06;
    payload[3] = 0x1A; payload[4] = 0xFF;  // 26 bytes of manufacturer content
    payload[5] = 0x4C; payload[6] = 0x00; // Apple company ID, little endian
    payload[7] = 0x02; payload[8] = 0x15; // iBeacon indicator
    memcpy(&payload[9], uuid, 16);
    payload[25] = (major >> 8) & 0xff;
    payload[26] = major & 0xff;
    payload[27] = (minor >> 8) & 0xff;
    payload[28] = minor & 0xff;
    payload[29] = static_cast<uint8_t>(txPower);

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
    MICROBIT_BLE_ECHK(sd_ble_gap_adv_set_configure(&advHandle, &data, &params));
    MICROBIT_BLE_ECHK(sd_ble_gap_adv_start(advHandle, SCHOOL_CONN_CFG_TAG));
    advertising = true;
}

void SchoolBeaconService::stop() {
    if (!advertising) return;
    MICROBIT_BLE_ECHK(sd_ble_gap_adv_stop(advHandle));
    advertising = false;
}
#endif
