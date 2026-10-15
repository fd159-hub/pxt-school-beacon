#include "pxt.h"
#include "MicroBitConfig.h"
#if MICROBIT_CODAL
#include "SchoolBeaconService.h"
using namespace pxt;
//% color=#0B83D8 weight=90 icon="\uf294" block="School Beacon"
namespace schoolbeacon {
    static SchoolBeaconService *service = NULL;
    static int hexNibble(char c) {
        if (c >= '0' && c <= '9') return c - '0';
        if (c >= 'A' && c <= 'F') return c - 'A' + 10;
        if (c >= 'a' && c <= 'f') return c - 'a' + 10;
        return -1;
    }
    //% shim=schoolbeacon::start
    void start(String uuid, int major, int minor, int txPower) {
        ManagedString s = MSTR(uuid);
        uint8_t bytes[16] = {0};
        int digits = 0;
        int half = -1;
        for (int i = 0; i < s.length(); ++i) {
            char c = s.charAt(i);
            if (c == '-') continue;
            int nibble = hexNibble(c);
            if (nibble < 0 || digits >= 32) { uBit.display.print("E1"); return; }
            if (half < 0) half = nibble;
            else { bytes[digits / 2] = (half << 4) | nibble; half = -1; }
            ++digits;
        }
        if (digits != 32 || half != -1 || major < 0 || major > 65535 || minor < 0 || minor > 65535 || txPower < -127 || txPower > 20) {
            uBit.display.print("E2"); return;
        }
        if (!service) service = new SchoolBeaconService();
        service->start(bytes, static_cast<uint16_t>(major), static_cast<uint16_t>(minor), static_cast<int8_t>(txPower));
    }
    //% shim=schoolbeacon::stop
    void stop() { if (service) service->stop(); }
}
#endif
