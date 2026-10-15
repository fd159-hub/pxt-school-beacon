// 教學範例：同一校園共用 UUID，Major 表示區域，Minor 表示裝置。
const BEACON_UUID = "7E6A9F10-4B92-4B8C-A571-6B59D81E0026"
const BEACON_MAJOR = 101
const BEACON_MINOR = 1

function showBeaconState() {
    basic.showIcon(schoolbeacon.isBeaconActive() ? IconNames.Yes : IconNames.No)
}

input.onButtonPressed(Button.A, function () {
    if (schoolbeacon.isBeaconActive()) {
        schoolbeacon.stopBeacon()
    } else {
        schoolbeacon.startBeacon(BEACON_UUID, BEACON_MAJOR, BEACON_MINOR)
    }
    showBeaconState()
    serial.writeValue("beaconError", schoolbeacon.getLastError())
})
input.onButtonPressed(Button.B, function () {
    basic.showNumber(BEACON_MINOR)
    showBeaconState()
})
// 開機自動廣播；勾號僅代表 SDK 啟動成功，手機接收仍須實測。
schoolbeacon.startBeacon(BEACON_UUID, BEACON_MAJOR, BEACON_MINOR)
showBeaconState()
serial.writeValue("beaconError", schoolbeacon.getLastError())
