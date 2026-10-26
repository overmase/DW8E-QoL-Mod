# 真・三國無雙7 Empires Mod(dinput8.dll)

[日本語](README.ja.md) | [English](README.md) | **繁體中文**

這是 **真・三國無雙7 Empires**(Steam 版)的非官方 Mod。只需要一個 `dinput8.dll`,並以 `version.ini` 進行設定。可以修正遊戲原有的錯誤、防止當機,並調整戰鬥平衡與畫面配置。

> **英文版與繁體中文版目前為測試版。** 日文版已經測試過很多次,但英文版和繁體中文版的測試次數還很少。如果發現問題,請回報(請參閱[回報問題](#回報問題))。

## 支援的版本

DLL 會讀取 `Launch.exe` 的建置時間戳記,自動使用對應版本的位址。同一個 `dinput8.dll` 和 `version.ini` 可用於三個版本。

| 版本 | Launch.exe 建置時間戳記 |
|---|---|
| 日文版 1.0.5.0 | `0x56CEA82C` |
| 英文版 | `0x56CE9E3F` |
| 繁體中文版 | `0x56CEAB10` |

如果是其他的 `Launch.exe`,DLL 不會做任何修改,只會在 `mod_log.txt` 寫入 `[停止]`,遊戲會正常執行。

## 安裝方法

1. 下載 `dinput8.dll` 和 `version.ini`。
2. 將兩個檔案放到遊戲資料夾(`Launch.exe` 所在的資料夾)。
   例: `C:\Program Files (x86)\Steam\steamapps\common\Dynasty Warriors 8 - Empires\`
3. 照常啟動遊戲。

**解除安裝:** 刪除 `dinput8.dll` 即可(也可以一併刪除 `version.ini`)。

請不要同時使用會修改相同位置的 Cheat Engine 腳本。如果 DLL 發現某個位置已經被其他程式修改,就不會啟用該功能,並在記錄中寫入 `[中止]`。

## 功能

所有功能都可以在 `version.ini` 中開啟、關閉或調整(檔案內的說明為日文與英文)。

| 區段 | 說明 |
|---|---|
| `[Mod]` StealPatch | 修正單位欄位不足時,我方武將不出現在戰場上的問題 |
| `[Mod]` AllyStrayZako / AllyEscort / EnemyEscort / EnemyLinkedZakoLevel | 開啟或關閉我方的散兵與武將的護衛部隊,並減少敵方護衛附帶的士兵 |
| `[Mod]` EnemyEscortRewrite / BaseEscortRewrite / RoamingEscortRewrite | 更改敵方護衛部隊(武將 / 據點兵長 / 遊擊隊長)的兵種 |
| `[Mod]` UniqueModelBoxes | 增加可同時以專屬外觀出場的自創武將・固有武將人數(原本 6 → 14) |
| `[Mod]` LoadTableGuard | 防止武將讀取表(90 筆)滿載時當機 |
| `[Mod]` CrashGuard… | 防止遊戲原有錯誤造成的當機(7 種) |
| `[HpScale]` | 依 TypeID 設定 HP 倍率(減少受到的傷害 / 延長血條) |
| `[Message]` | 調整畫面中央的訊息條與左側對話視窗的位置 |
| `[Map]` | 在地圖切換中加入「隱藏」,調整周邊地圖的位置與大小 |
| `[Affinity]` | 減弱武器相性不利時的慢動作,可取消反擊的緩衝時間 |
| `[AllyHP]` | 在畫面左側顯示我方武將的名字與 HP(F9 切換顯示) |
| `[Stratagem]` | 秘計的冷卻時間(自己 / 我方 AI / 敵方)、AI 的等待時間與點數回復、效果時間、對我方有效的範圍、自己的秘計量表最大值 |
| `[StratagemOverride]` | 依秘計種類個別設定冷卻時間與效果時間的倍率(預設:挑釁〔挑発,TypeID 60〕的冷卻時間 10 倍、效果時間 20 倍) |
| `[Camera]` | 一般鏡頭與防禦鏡頭的距離與角度(FOV) |
| `[RawValue]` | 測試中的數值(進階使用者) |

隨附的 `version.ini` 多數為本 Mod 的建議設定。把數值改回說明中寫的原始值,該部分就會恢復原狀。

### version.ini 常用設定

| 設定 | 說明 |
|---|---|
| `1` / `0` | 大多數項目 1 = 開啟,0 = 關閉 |
| `PlayerCooldown` / `AllyCooldown` / `EnemyCooldown` | 秘計冷卻時間倍率(1 = 原始,0.5 = 一半,0 = 立即可用) |
| `Duration` | 秘計效果時間倍率 |
| `BannerY` / `TalkOffsetX` / `TalkOffsetY` | 文字位置(以 1280×720 為基準的座標) |
| `ToggleKey=120` | 我方 HP 顯示的切換鍵(120 = F9) |

## 記錄檔

DLL 會在遊戲資料夾寫入以下檔案(內容為日文)。

| 檔案 | 內容 |
|---|---|
| `mod_log.txt` | 啟動記錄。`[OK] … を入れました` = 已啟用,`[中止]` = 無法啟用,`[設定で無効]` = 在 ini 中關閉,`[エラー]` = ini 的數值錯誤 |
| `mod_status.txt` | 各功能的計數(每 30 秒更新) |
| `mod_crash.txt` | 只有在 `Launch.exe` 內當機時才會寫入 |

`mod_log.txt` 的第二行應該顯示 `中国語版 として動きます`(以中文版執行)。

## 回報問題

請建立 Issue,並附上:

- 遊戲版本(日文版 / 英文版 / 繁體中文版)
- `mod_log.txt`、`mod_status.txt`(如果有的話也附上 `mod_crash.txt`)
- 使用的 `version.ini`
- 發生問題時的情況(模式、地圖、戰鬥開始後多久等)

## 注意事項

- 本 Mod 為非官方的玩家自製 Mod,與 KOEI TECMO GAMES 無關。
- 只會在遊戲執行期間修改記憶體,不會修改 `Launch.exe` 或遊戲資料檔。
- 使用時請自行承擔風險。建議使用前先備份存檔。

## 作者

- **overmase**(主要開發)
- **もりね**
- **絶対助けるマン**

## 授權

[MIT 授權條款](LICENSE)
