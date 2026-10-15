# pxt-school-beacon — EXPERIMENTAL / UNVERIFIED

A micro:bit V2 native C++ MakeCode extension prototype for **iBeacon** advertisement. **Not compiled or tested on hardware.** No LINE HWID required; a separate phone app is needed for LINE Bot interaction.

## Usage
1. Host these files in a GitHub repository with its own `pxt.json`, or develop the extension locally using PXT.
2. Import the extension GitHub URL into MakeCode. The extension is not currently published.
3. Put the contents of `test.ts` into the project's `main.ts`.
4. Select micro:bit V2 and compile; resolve any CODAL/Nordic SDK incompatibility reported by the compiler.
5. Flash device and inspect BLE advertising with nRF Connect. Manufacturer data should be `4C 00 02 15` followed by UUID (16 bytes), major (2 bytes big endian), minor (2 bytes big endian), measured power (1 byte).
6. Button A: toggle advertising; Button B: display minor/device number.

## Critical caveats
- Proof-of-concept source only. NOT a working `.hex` file.
- The PXT native C++ toolchain, CODAL SoftDevice initialization, radio ownership, and advertisement handle reuse require actual integration testing.
- Do not include MakeCode Bluetooth extension alongside this experimental extension.
- `txPower = -59` is only a placeholder measured-power value, **not calibrated**. Calibrate against real RSSI at 1m for distance estimation if needed.
- Standard manufacturer payload uses Apple company ID for the iBeacon format. Do not claim Apple certification or authorization.
- A smartphone cannot natively forward these self-issued beacons to the LINE platform without a companion application.

Reference: https://github.com/pizayanz/pxt-linebeacon (MIT).
