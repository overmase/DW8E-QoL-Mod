# 真・三國無雙7 Empires Mod(dinput8.dll)

[日本語](README.ja.md) | [English](README.md) | **繁體中文**

這是 **真・三國無雙7 Empires**(Steam 版)的非官方 Mod。只需要一個 `dinput8.dll`,並以 `version.ini` 進行設定。可以修正遊戲原有的錯誤、防止當機,並調整戰鬥平衡、AI 的行為與畫面配置。也有把玩家角色交給 AI 操作的**自動戰鬥**。

#使用此模组玩游戏时，建议先使用下载的 version.ini 文件中的默认设置运行一次游戏。

這是 **真・三國無雙7 Empires**(Steam 版)的非官方 Mod。只需要一個 `dinput8.dll`,並以 `version.ini` 進行設定。可以修正遊戲原有的錯誤、防止當機,並調整戰鬥平衡、AI 的行為與畫面配置。也有把玩家角色交給 AI 操作的**自動戰鬥**。

**下载此文件可能会导致 Windows 将其检测为病毒，但这是误报。请允许 Windows Defender 接受该文件。该文件在 GitHub 上开源，且该模组使用非恶意代码创建。 

> **英文版與繁體中文版目前為測試版。** 日文版已經測試過很多次,但英文版和繁體中文版的測試次數還很少。特別是新加入的功能(`[Status]` 之後的區段、爭霸模式的功能、`[Drop]` 之後的區段)尚未在英文版與中文版上實機測試。如果發現問題,請回報(請參閱[回報問題](#回報問題))。

自上次發布以來的變更請參閱 [RELEASE_NOTES.zh-TW.md](RELEASE_NOTES.zh-TW.md)。

## 支援的版本

DLL 會讀取 `Launch.exe` 的建置時間戳記,自動使用對應版本的位址。同一個 `dinput8.dll` 和 `version.ini` 可用於三個版本。

| 版本 | Launch.exe 建置時間戳記 |
|---|---|
| 日文版 1.0.5.0 | `0x56CEA82C` |
| 英文版 | `0x56CE9E3F` |
| 繁體中文版 | `0x56CEAB10` |

如果是其他的 `Launch.exe`,DLL 不會做任何修改,只會在 `mod_log.txt` 寫入 `[停止]`,遊戲會正常執行。

**文件说明

文版游戏与overmase-AI-V~.exe不兼容。
您可以尝试安装TAU制作的替代mod: https://drive.google.com/file/d/1Ci6f2cEV3uhXRVC5SeqpmYJ76btcUp09/view?usp=sharing
https://steamcommunity.com/app/322520/discussions/0/3417682380495884007/

tau_battle_ai_empires_meiling.exe 与 tau_battle_ai_empires_meiling_no_cut.exe 的区别在于，后者在发动无双乱舞时不会放大特写镜头，而是直接触发无双攻击。
注意：使用 tau_battle_ai_empires_meiling.exe 时，庞德的无双乱舞「砂暴大征阵」将无法正常运作。他不会骑马，而是会卡进地里挥动武器。 

**安装方法 

1. 下载 dinput8.dll、version.ini 以及 tau_battle_ai_empires_meiling.exe 或 tau_battle_ai_empires_meiling_no_cut.exe（请根据个人喜好选择其一）。
2. 将这三个文件全部放入游戏根目录（即 Launch.exe 所在的文件夹）。
例如：C:\Program Files (x86)\Steam\steamapps\common\Dynasty Warriors 8 - Empires\
3. 运行 tau_battle_ai_empires_~.exe，如果窗口滚动到最后显示“Success!”，则说明安装成功。
4. 像往常一样启动游戏即可。

**更新:** 替換 `dinput8.dll`。`version.ini` 可以換成新的,或是把新的區段加到自己的 ini 中(沒有寫的項目會維持遊戲原始狀態)。

**解除安裝:** 刪除 `dinput8.dll` 即可(也可以一併刪除 `version.ini`)。

請不要同時使用會修改相同位置的 Cheat Engine 腳本。如果 DLL 發現某個位置已經被其他程式修改,就不會啟用該功能,並在記錄中寫入 `[中止]`。

## 功能

所有功能都可以在 `version.ini` 中開啟、關閉或調整(檔案內的說明為日文與英文)。

### 錯誤修正・防止當機

| 區段 | 說明 |
|---|---|
| `[Mod]` StealPatch | 修正單位欄位不足時,我方武將不出現在戰場上的問題 |
| `[Mod]` UniqueModelBoxes | 增加可同時以專屬外觀出場的自創武將・固有武將人數(原本 6 → 14) |
| `[Mod]` LoadTableGuard | 防止武將讀取表(90 筆)滿載時當機 |
| `[Mod]` CrashGuard… | 防止遊戲原有錯誤造成的當機(包含武將動作指令資料損壞時的防護) |

### 士兵・護衛

| 區段 | 說明 |
|---|---|
| `[Mod]` AllyStrayZako / AllyEscort / EnemyEscort / EnemyLinkedZakoLevel | 開啟或關閉我方的散兵與武將的護衛部隊,並減少敵方護衛附帶的士兵 |
| `[Mod]` EnemyEscortRewrite / BaseEscortRewrite / RoamingEscortRewrite | 更改敵方護衛部隊(武將 / 據點兵長 / 遊擊隊長)的兵種 |
| `[EnemyPower]` | 敵方據點兵長、遊擊隊長、士兵造成傷害的倍率(不影響武將) |

### 戰鬥平衡・能力

| 區段 | 說明 |
|---|---|
| `[HpScale]` | 依 TypeID 設定 HP 倍率(減少受到的傷害 / 延長血條) |
| `[Affinity]` | 減弱武器相性不利時的慢動作,可取消反擊的緩衝時間,相性不利時也能讓敵人硬直(`FlinchAtDisadvantage`) |
| `[Status]` | 武將資訊畫面量表的最大值(`GaugeMax`)、能力上限(`StatMax`,最大 30000)、能力強化階段的最低值(`StatRank`,也可以寫成 `1-6` 這樣的範圍) |
| `[Stratagem]` | 秘計的冷卻時間(自己 / 我方 AI / 敵方)、AI 的等待時間與點數回復、效果時間、對我方有效的範圍、自己的秘計量表最大值 |
| `[StratagemOverride]` | 依秘計種類個別設定冷卻時間與效果時間的倍率(ini 中列出了所有秘計的編號與名稱) |
| `[WeaponAffinity]` | 各武器適性(★)的攻擊加成(原始 2 / 5 / 7 / 10),也可以只對玩家 |

### AI・戰場

| 區段 | 說明 |
|---|---|
| `[AI]` | AI 強度(我方 / 敵方)、空中受身的機率,以及受身後或持續被打在空中時使出空中無雙・蓄力攻擊・普通攻擊・跳躍的機率 |
| `[AITarget]` | 武將的 AI 鎖定附近的敵方武將、排除到不了的目標、減少敵方 AI 只鎖定玩家的情況 |
| `[BaseCapture]` | AI 武將在畫面外削減據點耐久力的速度(我方 / 敵方) |
| `[OffscreenMarch]` | 畫面外武將的進軍速度(我方 / 敵方),以及回防本陣時的額外速度 |
| `[MainCamp]` | 敵方武將靠近本陣時,讓該軍的武將回去防守(我方 / 敵方)。也可以設定為回防途中不被敵人拖住 |
| `[AutoBattle]` | **自動戰鬥**。F8 開啟 / F10 關閉(也可以使用手把按鍵組合)。AI 會進軍、壓制據點、防守本陣、攻擊、防禦與使用無雙。開啟期間可以減少受到的傷害 |

### 操作・鎖定目標

| 區段 | 說明 |
|---|---|
| `[Target]` | 快速按兩下防禦(L1)鎖定附近的敵方武將,再按兩下解除。持續面向目標(`FaceTarget`:只在防禦中 / 一律)。敵我 AI 在防禦中也會面向目標(`FaceTargetAI`) |

### 道具・武器

| 區段 | 說明 |
|---|---|
| `[Drop]` | 爭霸模式的戰場上士兵也會掉落道具(`ConquestDrop`)、掉落機率(`DropChance`)、部隊所有人都會抽道具(`AllMembers`) |
| `[BonusItem]` | 特典道具的數值:真・朱雀翼 / 真・白虎牙 / 真・玄武甲、步光之劍、魚鱗甲・干將 |
| `[Item]` | 無雙量表最多 4 條(`MusouBarMax`,測試中)。裝備定風珠時隨時能按 L1 受身(`UkemiAnytime`) |
| `[ItemParam]` | 直接修改裝備道具・特典道具的效果數值(進階使用者) |

### 畫面

| 區段 | 說明 |
|---|---|
| `[Message]` | 調整畫面中央的訊息條與左側對話視窗的位置 |
| `[Map]` | 在地圖切換中加入「隱藏」,調整周邊地圖的位置與大小 |
| `[AllyHP]` | 在畫面左側顯示我方武將的名字與 HP(F9 切換顯示)。也可以在最上方顯示自己本陣的耐久力(`ShowCamp`) |
| `[Hud]` | 移動畫面元件的位置(自己的量表、指令框、K.O.COUNT、地圖、戰況量表、連擊數、敵方武將量表、據點耐久框),並在畫面下方顯示強化效果(道具・秘計)的剩餘時間(`BuffTime`)。以文字顯示無雙量表條數與 HP 數值(`PlayerInfo`,F9)。讓自己與敵方武將的 HP 量表超過最大 HP 1000 繼續延長(`HpBar…` / `TargetBar…`) |
| `[Camera]` | 一般鏡頭與防禦鏡頭的距離與角度(FOV) |
| `[RawValue]` | 測試中的數值(進階使用者) |

### 戰場環境・畫質

| 區段 | 說明 |
|---|---|
| `[Weather]` | 戰鬥中隨時間改變天候(晴・雨・雪・強風・暴風雨・暴風雪)與白天・夜晚。可設定間隔(分鐘)與各天候的比重 |
| `[Display]` | 景深(模糊)的強度或關閉(`DepthOfField`,F11 與原始切換)、異向性過濾(`Anisotropy`) |
| `[Frame]` | 在 `mod_status.txt` 寫入畫格間隔(平均・最大・延遲次數)(`Stats`)、更精細的畫格等待(`PreciseWait`) |

### 爭霸模式

| 區段 | 說明 |
|---|---|
| `[Conquest]` ExpBattle / ExpCommand / ExpMission | 從戰場回來時、指令(訓練等)、戰場任務所得經驗值的倍率,也可以加上隨機浮動(`…Random`) |
| `[Conquest]` Max… / EnemyMax… | 物資・兵力・金錢的上限(本國 / 其他國家) |
| `[Conquest]` Command… / Request… / Income… / EnemyIncome… | 指令、武將委託的報酬、每月收入所得物資・兵力・金錢的倍率與隨機浮動 |
| `[Conquest]` ExecuteChance | AI 君主處斬被俘武將的機率(其餘釋放) |
| `[Conquest]` DeathTroops / DeathChance | 兵力少於 `DeathTroops` 時被擊敗的武將,以 `DeathChance` % 的機率戰死(不改變再出擊的規則) |
| `[CommandCost]` / `[CommandSuccess]` | 各指令(含外交)的花費(物資・兵力・金錢)與成功率。`CommandLog=1` 會在 `mod_log.txt` 寫出指令的編號 |
| `[Conquest]` InvasionChance / InvasionChanceMul | AI 國家每月計畫侵攻的機率(固定 %,或對原始機率的倍率) |
| `[Conquest]` TerritoryDamageMin/Max / EnemyTerritoryDamageMin/Max | 襲擊・侵攻時增加的地區損壞度的隨機倍率(本國地區 / 其他國家地區) |
| `[Conquest]` FixSortieDamage | 修正官方錯誤:攻入已損壞的地區時,連**進攻方**的出擊人數也會減少 |
| `[Conquest]` AllianceMonths / AllianceMonthsMax | 同盟期間(月),可固定或隨機(例: 6〜12) |
| `[Conquest]` HideDamageForecast | 不顯示地區資訊的「損害預測」(損壞度設為隨機時使用) |
| `[AutoSave]` | 月份變更時(從戰場回來時也是)自動快速存檔,並在 `Savedata\ModAutoSave` 保留附日期時間的備份。`BlockSoftReset` 可以只在爭霸模式中、或一律阻止軟重置(Space + Delete / Start + Select) |
| `[ConquestParam]` | 直接修改爭霸的固定參數表(進階使用者) |

隨附的 `version.ini` 是作者實際遊玩時的設定(開啟了許多功能)。把數值改回說明中寫的原始值,該部分就會恢復原狀。

**目前無法使用的項目:** `[Drop]` `WeaponDropChance`、`[Display]` `Width` / `Height`、`[Debug]` 的全部、`[EnemyPower]` 的 `…Difficulty` / `…AiRow`。ini 中也標有「These don't work now」。請不要更改。

### version.ini 常用設定

| 設定 | 說明 |
|---|---|
| `1` / `0` | 大多數項目 1 = 開啟,0 = 關閉 |
| `PlayerCooldown` / `AllyCooldown` / `EnemyCooldown` | 秘計冷卻時間倍率(1 = 原始,0.5 = 一半,0 = 立即可用) |
| `Duration` | 秘計效果時間倍率 |
| `BannerY` / `TalkOffsetX` / `TalkOffsetY` | 文字位置(以 1280×720 為基準的座標) |
| `ToggleKey=120` | 我方 HP 顯示的切換鍵(120 = F9) |
| `[AutoBattle]` `KeyOn=119` / `KeyOff=121` | 自動戰鬥的開啟鍵 / 關閉鍵(119 = F8,121 = F10) |
| `[Display]` `DofToggleKey=122` | 景深(模糊)與原始狀態的切換鍵(122 = F11) |

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
- `[AutoSave]` 使用遊戲本身的快速存檔,會覆寫 `QuickSave.dat`,但不會動到一般的存檔欄位。
- 若不想在指令失敗或武將戰死後立刻重置,請將 `[AutoSave]` 與 `BlockSoftReset=1`(只在爭霸模式中阻止軟重置)一起使用。

## 作者

- **overmase**(主要開發)
- **もりね**
- **絶対助けるマン**

## 授權

[MIT 授權條款](LICENSE)
