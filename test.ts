const UUID = "7E6A9F10-4B92-4B8C-A571-6B59D81E0026"
const MAJOR = 101
const MINOR = 1
let isAdvertising = true

function startBeacon() {
    schoolbeacon.start(UUID, MAJOR, MINOR, -59)
    isAdvertising = true
    basic.showIcon(IconNames.Yes)
}

input.onButtonPressed(Button.A, function () {
    if (isAdvertising) {
        schoolbeacon.stop()
        isAdvertising = false
        basic.showIcon(IconNames.No)
    } else {
        startBeacon()
    }
})
input.onButtonPressed(Button.B, function () {
    basic.showNumber(MINOR)
    basic.pause(500)
    basic.showIcon(isAdvertising ? IconNames.Yes : IconNames.No)
})
startBeacon()
