/** School iBeacon experimental blocks; hardware requires micro:bit V2. */
//% color=#0B83D8 weight=90 icon="\uf294" block="School Beacon"
namespace schoolbeacon {
    /** Start broadcasting an iBeacon. UUID = 32 hex digits (hyphens allowed). */
    //% shim=schoolbeacon::start
    //% block="start iBeacon UUID %uuid major %major minor %minor Tx power %txPower"
    //% major.min=0 major.max=65535 minor.min=0 minor.max=65535
    //% txPower.min=-127 txPower.max=20
    //% uuid.defl="7E6A9F10-4B92-4B8C-A571-6B59D81E0026"
    //% major.defl=101 minor.defl=1 txPower.defl=-59
    export function start(uuid: string, major: number, minor: number, txPower: number): void {
        return;
    }

    /** Stop advertising. */
    //% shim=schoolbeacon::stop
    //% block="stop iBeacon"
    export function stop(): void {
        return;
    }
}
