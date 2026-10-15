/** Experimental micro:bit V2 iBeacon broadcaster. Simulator does not transmit BLE. */
//% color=#0B83D8 weight=90 icon="\uf294" block="校園 Beacon"
namespace schoolbeacon {
    /** Start or replace an iBeacon advertisement. Measured power is fixed at -59 dBm (uncalibrated). */
    //% shim=schoolbeacon::startBeacon
    //% blockId=schoolbeacon_start_beacon block="啟動 iBeacon UUID %uuid Major %major Minor %minor"
    //% weight=100 blockExternalInputs=1
    //% uuid.defl="7E6A9F10-4B92-4B8C-A571-6B59D81E0026"
    //% major.min=0 major.max=65535 major.defl=101
    //% minor.min=0 minor.max=65535 minor.defl=1
    export function startBeacon(uuid: string, major: number, minor: number): void {
        // Native shim handles hardware. Simulator deliberately never reports active.
        return;
    }

    /** Stop broadcasting. Repeated calls are safe. */
    //% shim=schoolbeacon::stopBeacon
    //% blockId=schoolbeacon_stop_beacon block="停止 iBeacon"
    //% weight=90
    export function stopBeacon(): void { return; }

    /** True only after the native SDK starts advertising successfully. Not a reception guarantee. */
    //% shim=schoolbeacon::isBeaconActive
    //% blockId=schoolbeacon_is_active block="iBeacon 廣播已啟動？"
    //% weight=80
    export function isBeaconActive(): boolean { return false; }

    /** Last native error: 0=success; 7=invalid parameter; otherwise Nordic SDK error code. Simulator: -1. */
    //% shim=schoolbeacon::getLastError
    export function getLastError(): number { return -1; }
}
