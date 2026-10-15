# 校園 iBeacon：pxt-school-beacon

**實驗性專案：已通過 MakeCode micro:bit V2 原生編譯，尚未燒錄實機，尚未驗證空中 BLE 廣播。** 不提供手機 App、Cloudflare Worker 或 LINE Bot 後端。

Repository：https://github.com/fd159-hub/pxt-school-beacon

## 安裝與使用

1. 開啟 https://makecode.microbit.org/ ，新增專案。
2. 在「進階 → 擴充功能」搜尋框貼上 https://github.com/fd159-hub/pxt-school-beacon ，加入套件。
3. 使用 micro:bit **V2 / V2.2**。移除 Bluetooth 等會啟用另一套 BLE 管理流程的擴充功能；本套件會以設定衝突阻擋官方 Bluetooth 套件。
4. 切換 JavaScript，將下面完整範例貼入 main.ts。匯入套件不會自動執行套件的 test.ts。
5. 使用 USB 下載並寫入 micro:bit V2。也可使用本次交付的 V2 HEX；不要將 V1 當作支援裝置。
6. 先用 Android nRF Connect 驗證，再進行校園部署評估。

## API 與積木

| JavaScript API | 積木與用途 |
| --- | --- |
| schoolbeacon.startBeacon(uuid, major, minor) | 啟動 iBeacon，或先停止再更新現有廣播 |
| schoolbeacon.stopBeacon() | 停止 iBeacon；尚未啟動或重複停止皆可 |
| schoolbeacon.isBeaconActive() | 讀取 SDK 啟停結果所維護的本機狀態，不表示手機已收到 |
| schoolbeacon.getLastError() | JavaScript 診斷用，0 為成功、7 為參數錯誤，其餘為 Nordic SDK 回傳碼 |

UUID 接受 32 個十六進位字元，或標準 8-4-4-4-12 格式，大小寫皆可。Major 與 Minor 請使用 0–65535 的整數；原生 int 參數會依 PXT 規則轉換數字，不應傳入小數。無效 UUID 或超出範圍的整數不會更新現有廣播。Measured Power 固定 -59 dBm，尚未校正，與射頻發射功率不同。

模擬器不傳送 BLE：isBeaconActive() 永遠為 false、getLastError() 為 -1，因此 LED 顯示叉號是預期行為。

## 完整範例

- 開機：自動呼叫啟動廣播。
- A 鍵：依 SDK 狀態切換廣播。
- B 鍵：顯示 Minor，再恢復狀態圖示。
- LED：勾號代表 SDK 啟動成功，叉號代表未啟動。錯誤碼寫入 USB Serial。

```typescript
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
```

## 重新編譯（Windows PowerShell）

安裝 Node.js（本次 22.22.3）與 Git 後執行：

```powershell
git clone https://github.com/fd159-hub/pxt-school-beacon.git
cd pxt-school-beacon
npm.cmd ci --ignore-scripts
npm.cmd run build
# 僅編譯模擬器 JavaScript：
npm.cmd run build:sim
```

建置使用 Microsoft makecode / MKC 1.3.6，會下載 MakeCode 網頁發行版並呼叫其雲端 C++ 編譯服務，需要網路，不是完全離線工具鏈。腳本將快取放在專案的 .pxt/。package-lock.json 鎖定 npm 依賴；mkc.json 指向穩定站台，站台日後可能更新，需比對 verification/build-manifest.json 的編譯版本，不能假設永久可逐位元重現。

成功產物：

- built/mbcodal-binary.hex：micro:bit V2 專用 Intel HEX，建議本階段使用。
- built/binary.hex：Universal HEX；產生它不代表本套件支援 V1。
- built/codal.json：實際原生建置設定。
- 沒有 UF2：本次 micro:bit target 輸出 HEX，未將 HEX 改名為 UF2。

npm run build 也會檢查 V2 HEX 全部 Intel HEX 紀錄的長度、checksum 與 EOF。built/、node_modules/、.pxt/ 都不提交至 Git。

## 2026-10-15 編譯結果

| 檢查 | 結果 |
| --- | --- |
| 原型未修改建置 | 失敗：缺少 shims.d.ts |
| 僅補宣告檔後的原型 C++ 建置 | 失敗：SchoolBeaconService 的 constructor/start/stop undefined reference |
| 修正版 V2 原生 C++ 與 TypeScript 建置 | Build OK，exit 0 |
| 獨立專案以本機連結匯入擴充套件 | Build OK，exit 0 |
| 獨立專案從 GitHub a967a86c63975a533c2ca8c236825b7da68ae207 匯入 | Build OK，exit 0 |
| 模擬器 JavaScript 建置 | Build OK，exit 0；不代表 BLE 成功 |
| 與官方 Bluetooth 同時使用 | 預期失敗：microbit-dal.bluetooth.enabled 設定衝突 |
| V2 HEX 檔案驗證 | 18,933 筆紀錄、302,695 data bytes、checksum 皆正確 |
| A/B 按鍵與空中封包 | 已包含在編譯產物；實機行為未驗證 |

MakeCode target 9.0.12、PXT 13.0.9、CODAL v0.3.5、Nordic SDK Commit 4b8abc690f6c9fca6132e6db5ee13a795a263f88（S113）。詳細版本、SHA256 與建置日誌見 verification/。

## 修正與相容性

1. 補上 PXT 要求的 shims.d.ts / enums.d.ts；TS 明確指定 C++ shim，三個積木各有固定 blockId。
2. SchoolBeaconService.cpp 先載入 pxt.h 再檢查 MICROBIT_CODAL，避免整份實作在 V2 被略過造成連結失敗。
3. 本套件自行管理 SoftDevice，將 bluetooth.enabled 與 pairing_mode 設為 0。DEVICE_BLE=1 是 CODAL 的 SoftDevice 支援，不能用來判斷是否與 Bluetooth 擴充功能並用；衝突檢查使用 MICROBIT_BLE_ENABLED。
4. 檢查初始化、configure、start 與 stop 的 SDK 回傳值；啟動成功後才將狀態設為 true。不在靜態建構期間啟動 BLE，不再依靠獨立的測試程式旗標假定成功。
5. 廣播資料保存在 static buffer；更新前先成功停止廣播，再重用廣播 handle。stop 只停止廣播，保留 SoftDevice 供下次啟動。
6. 固定非連線、不可掃描、1 Mbps legacy advertising，duration=0。SDK S113 標頭的 interval 範圍為 20 ms–10.24 s，200 ms = 320 個 0.625 ms 單位；已以 static_assert 和原生編譯核對，實際空中間隔未量測。
7. 保留上游 MIT 與 Nordic 完整授權，見 LICENSE、THIRD_PARTY_NOTICES.md、third-party/。

## iBeacon 預期封包

本範例 UUID 7E6A9F10-4B92-4B8C-A571-6B59D81E0026、Major 101、Minor 1、Measured Power -59：

```text
02 01 06 1A FF 4C 00 02 15
7E 6A 9F 10 4B 92 4B 8C A5 71 6B 59 D8 1E 00 26
00 65 00 01 C5
```

共 30 bytes。Flags 為 02 01 06；1A 為 Manufacturer AD 長度、FF 為 AD type；Apple Company ID 0x004C 在封包內是 Little Endian 的 4C 00；02 15 為 iBeacon 類型與資料長度；UUID 16 bytes；Major/Minor 各 2 bytes、Big Endian；C5 是 -59 的 signed int8 表示。以上是程式預期值，尚無手機截圖或空中封包擷取證據。

## micro:bit V2 / nRF Connect 實機驗收

1. 準備 micro:bit V2、USB 資料線、Android 手機與 nRF Connect。先將單片裝置放在桌上測試，記錄板子版本、手機 OS、App 版本及 HEX SHA256。
2. 將 V2 HEX 複製到 MICROBIT 磁碟，等待寫入完成，重新供電。若 LED 叉號，讀取 Serial 的 beaconError，不應直接判定天線或手機故障。
3. 開啟 nRF Connect Scanner，授予系統要求的 Bluetooth／附近裝置／定位等權限。此廣播沒有裝置名稱，也不可連線；用 Manufacturer Data 或 iBeacon 解析欄位定位，無須配對。
4. 檢查 4C 00 02 15、UUID、Major=101、Minor=1、Measured Power=-59。部分介面將 Apple Company ID 獨立列出，資料欄可能從 02 15 開始。
5. 按 B，確認 LED 顯示 1 並恢復狀態圖示；掃描端持續收到新資料。
6. 按 A 停止，確認不再有新封包時間戳。掃描列表中的裝置可能仍保留快取，不能只看裝置是否仍在列表。再按 A，確認收到新封包。
7. 連續啟停至少 20 次、斷電重開 5 次，確認無 panic、無卡住且內容正確。
8. 修改 Minor=42、Major=1，重建後核對尾端 00 01 00 2A C5；另測 0 / 65535 邊界、無效 UUID、重複 start/stop、stop-before-start，記錄結果。
9. 手機接收時間差受掃描與 OS 排程影響，不能精準證明 200 ms；需要精準量測時使用 BLE sniffer。iOS 若未顯示原始 iBeacon 資料，改用 Core Location 測試程式確認。

以上皆為待執行流程，不是本次已通過的實機測試。

## 已知限制與下一階段

- 尚未實測 BLE、A/B 按鍵、反覆啟停、長時間供電或手機接收；尚未做完整的參數邊界執行測試。實驗性 0.0.1，未建立正式 Release 或 tag。
- 不與 Bluetooth、其他 Beacon 擴充功能或會直接操作無線電的 radio 程式並用。本次未驗證其餘第三方套件組合。
- 不提供可連線 GATT 服務與 Bluetooth 燒錄；使用 USB 寫入。Measured Power 尚未校正，不能依此精準估算距離。
- 此 iBeacon 不會直接觸發 LINE Beacon HWID 流程，後續需要伴隨 App；本次沒有更動任何 LINE Bot 或部署雲端服務。
- iOS 下一階段採 Core Location 的 Beacon 區域監控（region monitoring）偵測進出，必要時進行 ranging（距離估測），並實測授權、鎖屏、背景、重開機與強制結束 App。背景觸發不保證即時。採固定校園 UUID，以 Major/Minor 分配區域與裝置，後端另設事件去重與冷卻時間。

## 官方與上游參考

- https://github.com/pizayanz/pxt-linebeacon
- https://github.com/microsoft/pxt-mkc
- https://github.com/lancaster-university/codal-microbit-v2/tree/v0.3.5
- https://github.com/microbit-foundation/codal-microbit-nrf5sdk/blob/4b8abc690f6c9fca6132e6db5ee13a795a263f88/nRF5SDK/components/softdevice/s113/headers/ble_gap.h
- https://developer.apple.com/documentation/corelocation/determining-the-proximity-to-an-ibeacon-device
