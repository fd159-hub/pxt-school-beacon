#include "pxt.h"
#include "MicroBitConfig.h"
#if MICROBIT_CODAL
#include "SchoolBeaconService.h"
#include "nrf_error.h"
using namespace pxt;

namespace schoolbeacon {
    static SchoolBeaconService *service = NULL;
    static int lastError = 0;

    static int hexNibble(char c) {
        if (c >= '0' && c <= '9') return c - '0';
        if (c >= 'A' && c <= 'F') return c - 'A' + 10;
        if (c >= 'a' && c <= 'f') return c - 'a' + 10;
        return -1;
    }

    // Accept exactly 32 hex characters or canonical 8-4-4-4-12 UUID format.
    static bool parseUUID(String value, uint8_t bytes[16]) {
        ManagedString text = MSTR(value);
        const int length = text.length();
        if (length != 32 && length != 36) return false;
        int digits = 0;
        for (int i = 0; i < length; ++i) {
            if (length == 36 && (i == 8 || i == 13 || i == 18 || i == 23)) {
                if (text.charAt(i) != '-') return false;
                continue;
            }
            int nibble = hexNibble(text.charAt(i));
            if (nibble < 0) return false;
            if ((digits & 1) == 0) bytes[digits / 2] = nibble << 4;
            else bytes[digits / 2] |= nibble;
            ++digits;
        }
        return digits == 32;
    }

    //% parts="schoolbeacon"
    void startBeacon(String uuid, int major, int minor) {
        uint8_t bytes[16] = {0};
        if (!parseUUID(uuid, bytes) || major < 0 || major > 65535 || minor < 0 || minor > 65535) {
            lastError = NRF_ERROR_INVALID_PARAM;
            return; // Preserve any existing valid advertisement.
        }
        if (!service) service = new SchoolBeaconService();
        service->start(bytes, (uint16_t)major, (uint16_t)minor);
        lastError = (int)service->error();
    }

    //%
    void stopBeacon() {
        if (service) { service->stop(); lastError = (int)service->error(); }
        else lastError = 0;
    }

    //%
    bool isBeaconActive() { return service && service->isActive(); }

    //%
    int getLastError() { return lastError; }
}
#else
// PXT may compile both variants when producing Universal HEX. V1 must never silently run.
using namespace pxt;
namespace schoolbeacon {
    //%
    void startBeacon(String uuid, int major, int minor) { target_panic(PANIC_VARIANT_NOT_SUPPORTED); }
    //%
    void stopBeacon() { target_panic(PANIC_VARIANT_NOT_SUPPORTED); }
    //%
    bool isBeaconActive() { return false; }
    //%
    int getLastError() { return -1; }
}
#endif
