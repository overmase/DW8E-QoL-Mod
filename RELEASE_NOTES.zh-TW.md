# 更新內容 — 2026-11-25 (272)

以下是自上次交給測試者的版本 **2026-11-13 (163)** 以來的變更。更早的變更請看本檔案下方的「之前版本的變更」。

> **關於隨附的 `version.ini`:** 這次同樣是作者實際遊玩時的設定。也包含改造秘計「絆」「集中射擊」的設定(見下方「秘計」),以及關閉鏡頭牆壁碰撞的設定。若想以接近原始遊戲的狀態測試,請把各項目改回說明中寫的「原始」數值。

> **關於英文版與繁體中文版:** 本次的新功能是在日文版上確認的。英文版與中文版已自動對應,但尚未在實機上測試。如果無法運作或當機,請回報。

## 秘計

### 直接修改秘計數值 `[StratagemParam]` / `[StratagemInfo]` / `[StratagemShotParam]`(進階)
- `[StratagemParam]` `種類.位置(16進位)=值`:直接改寫秘計定義中的數值。效果種類・持續時間・強度・消耗・冷卻秒數・AI 使用機率,大規模秘計的撤退部隊數(`+26`)・發動前秒數(`+28`)・所需據點數(`+2A`),是否也對友軍有效(`+32`)與半徑(`+34`)等。
  - 所有秘計的原始數值會以可直接複製的格式寫到 **`DW8E_StratagemParam_Default.txt`**(`[StratagemAI] Dump=1` 時,進入戰場時產生)。只要把想改的行複製到 `[StratagemParam]` 再改數字即可。
  - 與遊戲原始數值相同的行不做任何事(`[StratagemOverride]` 等倍率照常生效)。
  - 範例(隨附的 ini):`64.26=0`~`77.26=0`,使用大規模秘計時武將不會撤退。
- `[StratagemInfo]` `種類.位置=值`:改寫秘計資訊表(一開始就解鎖・分類・可用模式)。
- `[StratagemShotParam]`:改寫攻擊秘計放出物(炎擊等)的數值。

### 讓絆・集中射擊可以使用 `[StratagemAI] Unlock`
- `Unlock=48,52`:資料中存在卻無法使用的「絆」「集中射擊」,一開始就解鎖,任何模式都能裝備。
- 隨附的 ini 改造了這兩個秘計:
  - **絆**:提升自己與周圍友軍的移動速度・攻擊速度 25 秒(`48.04` / `48.06` / `48.08` / `48.0E` / `48.10` / `48.32` / `48.34`)。
  - **集中射擊 → 集中攻擊**:效果與召集相同,把 2 名武將拉到自己身邊(`52.06=49` 與 `[StratagemAI] SummonCount=52:2`)。

### 據點急襲・據點佔領奪取的據點數 `[StratagemAI]`
- `BaseRaidCount` / `BaseSeizeCount`(1~19,原始 1):據點急襲・據點佔領奪取的據點數。敵方使用時也相同。

### 召集的人數 `[StratagemAI]`
- `SummonCount=種類:人數,…`(1~6):具有召集效果的秘計拉來的武將人數。例:`52:2`。

### 替換遊戲文字 `[Text]`
- `編號=文字`:替換遊戲文字(ini 為 UTF-8,`\n` 換行)。編號與 `[Debug] DumpText=1` 產生的 `DW8E_Text.txt` 相同。
- 秘計名稱 = 20878 + 種類,說明 = 21134 + 種類,短說明 = 21262 + 種類。
- 隨附的 ini(日文文字)把集中射擊改名為「集中攻擊」,並配合新效果修改了絆・集中攻擊的說明。

## 救援 `[Rescue]`
- 與依賴戰的「用這個治療」相同,接近(`Range`)HP 偏低(`HpBelow` %)的友軍武將時,把該武將的 HP 回復到 `HealTo` %。畫面上方會顯示「救援○○」。同一武將需間隔 `Cooldown` 秒。`Enable=1` 開啟。

## AI

### 切換武器
- `[AI] AffinitySwitchAlly` / `AffinitySwitchEnemy`:友軍 / 敵軍武將在對瞄準的敵將相性不佔優勢時也會切換武器(`AffinitySwitchCooldownMs`,預設 12000 = 同一武將間隔 12 秒)。
- `[AI] AISwitch`(試驗):不讓 AI 使出武器切換攻擊(延遲出現的切換)(`1`),或換成其他動作(`SwitchReplace`)(`2`)。

### 目標的選擇 `[AITarget]`
- `SkipDeadTarget=1`:AI 不再持續瞄準 HP 已為 0 的對象(倒下的身體)。防止友軍一直把已擊倒的敵人打飛,導致屍體不消失、戰局無法推進。

### 自動戰鬥 `[AutoBattle]`
- `UseStratagem=1`:自動戰鬥中也會以與武將 AI 相同的判斷使用秘計。
- `UseInstallStratagem`:使用(`1`)/ 不使用(`0`)設置型秘計(箭樓・投石車・祭壇・陷阱)。
- `AffinitySwitch=1`:檢查與瞄準的敵將之間的相性,另一把武器較好時按 R1 切換(`AffinitySwitchRange` / `AffinitySwitchCooldownMs`)。

## 鏡頭 `[Camera]`
- `WallCollision=0`:關閉鏡頭對牆壁・設置物的碰撞。靠牆時鏡頭也不會卡住(可能會看到牆內或牆後)。

## 爭霸模式 `[Conquest]`

- **自己以外武將的成長** `AllyGrowth` / `EnemyGrowth`:本國武將 / 他國・在野武將的功績(經驗值)增加倍率。對每月成長・戰場・指令有效。玩家本人不在對象內。Lv 依遊戲機制在隔天上升。
- **從戰場回來時的疲勞** `FatigueAdd` / `FatigueMul`:增加固定的疲勞值(`-1` = 不使用),或對原始的量(一場約 20)乘上倍率。
- **疲勞造成的能力下降** `FatigueStat=重,中,輕`(原始 70,80,90)/ `FatigueStat2=重・輕,中`(原始 50,70):各疲勞階段的能力 %。武將資訊畫面的數字也會改變。
  - 體力・防禦之後會加上裝備道具的部分,所以看起來比攻擊・統率下降得少。
- `AllianceMonths` 也能寫成 `8-12` 的範圍。
- `StageMap`:以與自由模式相同的寫法,替換爭霸地區的戰場。
- `[CommandCost]`:退隱(87 號)也能設定兵力・金錢消耗(ini 中有範例)。
- **修正**:`FixSortieDamage=1` 時,自己不是君主或他國進攻時,進攻方的出擊名額也會減少,已修正。

## 開始時的戰況 `[Situation]`

- `TroopRatio`:把戰鬥開始時的據點分配,以「敵方兵力 × 此值」作為我方兵力重新決定。`0.2` 左右為極度劣勢,`0` = 不使用,寫成 `0.1-0.5` 則為亂數。戰況量表也套用相同倍率。

## 自由模式 `[FreeMode]`

- **隨機的候選** `RandomUnique` / `RandomGeneric` / `RandomCustom`:選擇出擊武將的隨機是否包含 無雙武將 / 泛用武將 / 編輯武將(若會少於 16 人則不篩選)。
- **替換依賴戰的旅團長** `RequestOfficer=1`(試驗):把自由模式依賴戰中固定的「旅團長」換成候選(`RequestUnique` / `RequestGeneric` / `RequestCustom`)中的武將。
- **專用地圖** `ExtraStages=29,28`:在自由模式的戰場清單加入前作的許昌(護衛)・洛陽(暗殺)地圖(可用 28・29・143~149)。
  - `StageMap=地區:編號,…` / `AttackStage`:改變選擇地區時的戰場(0~23・28・29・140~149。24~27 會當機,無法使用)。
  - `StageName編號`(日文版)/ `StageName編號_EN`(英文版,僅半形)/ `StageName編號_CN`(中文版,簡體字):清單中顯示的名稱。

## 據點 `[Base]` / `[BaseUpgrade]` / `[BaseEffect]`

- **據點隨機** `RandomAlly` / `RandomEnemy`(%):在爭霸・自由模式中,把一開始為一般據點的據點換成 補給・攻擊・防禦據點。Lv 由 `RandomLevelAlly` / `RandomLevelEnemy`(例 `2-4`),種類比重由 `RandomWeight=補給,攻擊,防禦` 決定。
- **本陣・副本陣的 Lv** `MainCampLevelAlly` / `MainCampLevelEnemy` / `SubCampLevelAlly` / `SubCampLevelEnemy`:1~4,寫成 `2-4` 則為亂數。
- **據點耐久度** `DurabilityLv=Lv1,Lv2,Lv3,Lv4` / `DurabilityCamp`(本陣・副本陣。1 個值套用全部,4 個值則依 Lv):進入戰場時的耐久度倍率。
- **據點強化費用** `LevelCost`(升級,Lv2,Lv3,Lv4)/ `RebuildCost`(改建,目前 Lv 1~4):爭霸的物資。`FreeLevelCost` / `FreeRebuildCost` 為自由模式的特典點數。只寫 1 個值則全部相同,空白 = 遊戲原樣。`CostMul` / `FreeCostMul` 為未寫部分的倍率。
- **據點效果** `AttackTable` / `DefenseTable` / `SupplyTable`:持有 1,2,3 … 個時的值(遊戲 150,180,200 …)。攻擊 = 傷害 ×(1 + 值 × 0.0025),防禦 = 最大體力 ×(值 + 100)÷ 200,補給 = 兵力回復 × 值 ÷ 100。
- `HPCap`:戰場體力上限(遊戲 1000)。避免防禦據點倍率在 1000 處被吃掉。
- `DefenseLive`:防禦據點數量改變時重新套用體力(1 = 玩家 / 2 = 武將也套用)。原始遊戲中玩家不會獲得防禦據點效果。
- 據點 Lv 上限 4 無法更改。

## 敵方隨從 `[Mod]`

- `EnemyOfficerEscortSize` / `EnemyBaseEscortSize` / `EnemyRoamingEscortSize`:敵將 / 據點兵長 / 遊擊隊長的隨從士兵人數倍率(每個部隊最多 30 人)。增加的只有兵卒・親衛隊・騎兵・弓兵等一般士兵。
  - **不建議設為大於 1**(見下方「已知事項」)。
- `EscortSizeLimit`(預設 80,0 = 無):增加隨從時戰場人數的上限。
- 變更了 `EnemyEscort=1` 時 `EnemyLinkedZakoLevel` 小於 4 的行為。`0` = 不出隨從,`1`~`3` = 敵將隨從人數 0.3 / 0.5 / 0.7 倍。
- **修正**:`EnemyEscort=1` 且 `EnemyLinkedZakoLevel` 小於 4 時,地圖上出現大量看不見的敵將圖示,導致自動戰鬥無法推進,已修正。
- `SquadId` / `BaseSquadId` / `RoamingSquadId` 不能再指定戰場專用士兵(老虎・兵器・妖術兵・野盜等)(看不見或會當機。mod_log 會寫出錯誤)。

## 武器隨機 `[Chaos]`

- `RandomWeapon=1`:進入戰場時,自己的 2 把武器從所有種類中隨機決定(天地人也隨機)。離開戰場後回到原本的 2 把。
- `RandomWeaponTier`(1~10,預設 3):各種類使用第幾強的武器。也能寫成 `4-9` 的範圍。

## AI 的行動 `[AI]` / `[AITarget]` / 自動戰鬥 / 本陣防衛

- **積極的 AI** `[AI] AggressivePlayer` / `AggressiveAlly` / `AggressiveEnemy`(0~100 %):與展示遊玩的 AI 相同,以此機率跳過「觀望」行動。Player 是自動戰鬥中的自己。
- **被貼身時的行動** `[AI] CloseIdleMs` / `CloseIdleRange` / `CloseIdleGuard` / `Attack` / `Charge` / `Jump` / `Move`:敵將在身邊 `CloseIdleMs` 時間內什麼都不做時,以機率使出防禦・攻擊・蓄力・跳躍・小幅移動。
- **解除防禦的延遲** `[AI] AllyGuardHoldMs` / `EnemyGuardHoldMs`:AI 停止防禦後仍持續防禦該時間(毫秒)。
- **對對手攻擊的防禦** `[AI] ReactGuardNormal` / `Charge` / `Switch` / `EX` / `Storm`(%),`ReactGuardRange` / `ReactGuardMs`。
- **防禦後的行動** `ReactAfterJump`(對對手跳躍攻擊)/ `ReactAfterCombo`(靠近後通常攻擊連段)(%),`ReactComboMs` / `ReactComboRange`。
- `[Target] TurnSpeed`:轉向速度。**2.25 以上在受到拘束型無雙時動作會異常**,請設在 2.2 以下。
- `[AutoBattle] UnstuckAttack`:卡住而橫向移動時或跳躍期間,也使出通常攻擊(1 = 橫移,2 = 跳躍,3 = 兩者)。
- `[MainCamp] OnlyWhenReachable=1`:進攻方的據點沒有連到本陣(無法攻下本陣)時,不回去防守本陣。
- `[AITarget] EscortNearRange`(預設 0,建議 600~1000):護衛要請等帶著的武將,若此距離內有敵將就以其為目標。僅在 `OfficerNearest=1` 時有效。
- **修正**:`[AITarget] OfficerNearest=1` 時,護衛要請帶著的武將在附近有敵人時仍站著不動,已修正。

## 畫面 `[Hud]`

- `TargetHP=1`:在 Lv 旁顯示瞄準中敵將的 HP 數字(目前 / 最大)。
- `PlayerInfoLevel=1`:在 PlayerInfo 文字開頭顯示自己的 Lv。
- `TargetLevel=1`:在量表下方顯示瞄準中敵將的 Lv(位置 `TargetLevelX` / `TargetLevelY`)。
- `LevelAdd`:畫面 Lv 差 1 時的修正。
- **修正**:左上敵將 HP 量表停在最大 HP 1000 的問題(`TargetBarMaxHP` 不是 1000 時),已修正。

## 其他・修正

- `[Mod] RandomSeed=1`:遊戲的亂數每次啟動都是相同順序,導致自由模式的隨機武將等每次都一樣,已修正。
- `[Debug] DemoSeconds`:展示遊玩(在標題畫面等待時開始的戰鬥)的長度(秒,原始 60)。
- **修正**:按 2 次防禦的鎖定有時會突然無法左右移動、被對手拉過去(類似 `FaceTarget=2` 的動作),已修正。
- **修正**:加入 `SkipDeadTarget` 後,AI 思考秘計時可能當機,已修正(272)。

## 已知事項・尚無法做到的事

- **戰場上能有實體的人數總共只有 150 人。** 增加隨從人數(`EnemyOfficerEscortSize` / `EnemyBaseEscortSize` / `EnemyRoamingEscortSize`)或據點士兵數(`[Base] GarrisonSoldiers`,試驗)而超過時,會出現只有地圖圖示、看不見也打不倒的士兵,AI 會一直揮空、據點也無法壓制。兩者請維持 1 / 0。
  - 有約 21 人的據點和 50 人以上的據點是原始遊戲的規格。
- 護衛要請帶著的武將會與士兵、隊長戰鬥,但有時不與敵將戰鬥。已確認原始遊戲也會發生。
- 用 `[Text]` 修改的秘計名稱・說明會出現在秘計畫面等處,但**戰場上的秘計欄(卡片名稱與短說明)還不會改變**。
- `[StratagemAI] Use`(讓 AI 使用平常不用的秘計)暫緩中。金城鐵壁・流言・據點修復等,因各秘計的其他條件 AI 不會使用。隨附的 ini 中為空白。
- `[Camera] TerrainCollision` 沒有效果。請維持 `1`。
- `[AI] AirEscape*`(在空中持續被打時以受身・空中無雙脫離)仍在試驗中。
- 無法在一般戰場放出動物(老虎)・兵器等(沒有戰場專用資料,看不見或會當機)。
- 隨機武器最多 2 把。
- `RequestOfficer` 仍在試驗中。
- 追加地圖在戰場準備畫面可能顯示別的名稱。
- `[Debug]` 的調查用項目(`WatchWrite` / `BaseOwnerProbe` / `StratagemAIProbe` / `SwitchProbe` / `SquadProbe` / `DemoProbe` / `BattleProbe` / `ExpProbe` / `ArmyProbe` / `DumpPersons` / `BaseProbe` / `GaugeProbe` / `BaseCrowdProbe` 等)請設為 `0`。

---

# 之前版本的變更 — 2026-11-13 (163)

以下是自上次交給測試者的版本 **2026-11-09 (129)** 以來的變更。更早的變更請看本檔案下方的「之前版本的變更」。

> **關於隨附的 `version.ini`:** 這次的 `version.ini` 是作者實際遊玩時的設定,**開啟了許多功能**(之前的版本大多為遊戲原始狀態)。若想以接近原始遊戲的狀態測試,請把各項目改回說明中寫的「原始」數值。

> **關於英文版與繁體中文版:** 本次的新功能是在日文版上確認的。英文版與中文版已自動對應,但尚未在實機上測試。如果無法運作或當機,請回報。

## 新功能:戰場環境・畫質

### 天候與晝夜的變化 `[Weather]`
- 戰鬥中,隨著時間經過,**天候**與**白天・夜晚**會改變(`Enable=1`)。
- `WeatherMinutes`:改變天候的間隔(分鐘)。隨附的 ini 為 4 分鐘。
- `DayNightMinutes`:切換白天與夜晚的間隔(分鐘)。隨附的 ini 為 8 分鐘。
- `Sunny` / `Rain` / `Snow` / `Wind` / `Storm` / `Blizzard`:晴・雨・雪・強風・暴風雨・暴風雪出現的比重(合計不必為 100)。
- 天候種類與祭壇使用的相同(晴 0 / 雨 2 / 雪 4 / 強風 18 / 暴風雨 20 / 暴風雪 5)。
- 只在戰鬥中(自己的武將在戰場上時)計算時間。

### 關閉景深(模糊) `[Display]`
- `DepthOfField`:遠近模糊的強度。`1` = 原始,`0` = 不模糊,`0.5` = 一半。隨附的 ini 為 `0`(不模糊)。
- `DofToggleKey`:戰鬥中按此鍵,在「上面的數值」與「原始」之間切換(預設 122 = **F11**)。

### 異向性過濾 `[Display]`
- `Anisotropy`:讓斜看的地面與牆壁紋理更清楚(2 / 4 / 8 / 16)。隨附的 ini 為 16。`0` = 原始。

### 畫格間隔的記錄 `[Frame]`
- `Stats=1`:在 `mod_status.txt` 的 [フレームの間隔] 寫入每個畫格的平均・最大時間與延遲的次數。可用於回報卡頓。
- `PreciseWait=1`:更精細的畫格等待(維持 60FPS)。作者的環境用原始的就足夠,所以隨附的 ini 為 `0`。

## 新功能:操作

### 鎖定目標 `[Target]`
- `DoubleTapGuard=1`:**快速按兩下防禦(L1)**,鎖定附近的敵方武將。鎖定中再按兩下即解除。間隔為 `DoubleTapMs`,搜尋距離為 `Range`。
- `FaceTarget`:持續面向目標。`1` = 只在防禦中,`2` = 一律(跑動時方向也會被拉向目標)。
- `FaceTargetAI=1`:玩家以外的角色(敵方・我方)在防禦中也會持續面向自己的目標。

## 新功能:道具・武器

### 士兵掉落的道具 `[Drop]`
- `ConquestDrop`:`1` = 爭霸模式的戰場上士兵也會掉落道具(肉包・老酒・戰神之斧等,原始不會掉落)。`2` = 所有模式。
- `DropChance`:掉落機率(%)。`-1` = 原始(依難易度 90〜5 %)。
- `AllMembers=1`:士兵部隊的所有人都會抽道具(原始只有隊伍最前面的 1 人)。

### 武器適性 `[WeaponAffinity]`
- 更改各武器適性(★)的攻擊加成(`Star1`〜`Star4`,原始為 2 / 5 / 7 / 10)。`PlayerOnly=1` = 只對玩家。

### 特典道具 `[BonusItem]`
- `TrueHP` / `TrueAttack` / `TrueDefense`:真・朱雀翼 / 真・白虎牙 / 真・玄武甲的數值(原始 1000)。
- `HokouPercent` / `HokouTime`:步光之劍(戰鬥開始時的攻擊強化)的 % 與時間。
- `GyorinPercent` / `KanshoPercent`:魚鱗甲(提升我方武將能力)/ 干將(降低敵方武將能力)的 %。
- 遊戲原本的數值會顯示在 `mod_status.txt` 的 [特典アイテム]。

### 裝備道具 `[Item]`
- `MusouBarMax=4`:無雙量表最多 4 條。Lv24 以上也能用青龍膽加上第 4 條(**測試中**)。
- `UkemiAnytime=1`:裝備定風珠時,被打在空中時**隨時都能按 L1 受身**(原始只有在受擊動作快結束且正在落下時)。

### 道具效果的數值 `[ItemParam]`(進階使用者)
- 直接修改裝備道具・特典道具的效果數值(例:神速符的移動速度、白刃珠的機率與倍率)。原本的數值會顯示在 `mod_status.txt` 的 [アイテムの値]。

## 新功能・改良:畫面 `[Hud]`
- `PlayerInfo=1`:以文字顯示自己的**無雙量表條數(目前/最大)**與 **HP 數值(目前/最大)**(F9 切換,位置 `InfoX` / `InfoY`)。
- `HpBarMaxHP` / `HpBarWidth`:自己 HP 量表的長度。原始在最大 HP 1000 時就不再變長,現在可以繼續延長。
- `TargetBarMaxHP` / `TargetBarWidth`:鎖定的敵方武將 HP 量表的長度(同樣可超過 1000)。
- `TargetBarEffectiveHP=1`:把 `[HpScale]` Mode=1(減少受到的傷害)的倍率也加進敵方武將的 HP 量表(以實際需要削減的量計算的 HP)。

## 其他
- `[Conquest]` `HideDamageForecast=1`:不顯示地區資訊的「損害預測」(損壞度設為隨機時預測不準)。已加入 ini。
- `[StratagemOverride]`:在 ini 中列出了所有秘計的編號與名稱(日・英・中)。

## 已知問題・尚未能做到的事
- `[Drop]` `WeaponDropChance`(敵方武將掉落武器箱的機率)**目前沒有效果**。請不要更改。
- `[Display]` `Width` / `Height`(高解析度)**目前無法使用**。請保持 `0`。
- `[Debug]` 的項目(調查 PS3 版殘留的連線畫面)僅供測試。請保持 `0`。
- `[EnemyPower]` 的 `…Difficulty` / `…AiRow` 沒有效果。請保持 `-1`。
- `[CommandSuccess]` 無法寫範圍(如 `20-60`)。請寫一個數字。
- `[Item]` `MusouBarMax=4` 為測試中。
- 時間只有「白天」與「夜晚」兩種。

---

# 之前版本的變更 — 2026-11-09 (129)

### 新功能:爭霸模式

沒有寫出區段名稱的項目都在 `[Conquest]` 中。

#### 經驗值・資源
- `ExpBattle` / `ExpCommand` / `ExpMission`:從戰場回來時(所有武將)、訓練等指令、戰場任務所得經驗值的倍率。`ExpBattleRandom` / `ExpCommandRandom` 可加上 ±% 的隨機浮動。結果畫面與指令畫面上的數字也會改變。
- `MaxSupplies` / `MaxTroops` / `MaxGold`(以及 `EnemyMax…`):本國與其他國家的物資・兵力・金錢上限(原始 999999)。
- `Command…` / `Request…` / `Income…` / `EnemyIncome…`:指令、武將委託的報酬、每月收入所得物資・兵力・金錢的倍率與隨機浮動。

#### 處斬・戰死
- `ExecuteChance`:AI 君主處斬被俘武將的機率(%),其餘會被釋放。對敵方君主,以及自己不是君主時的本國君主有效。先被登用的武將不在此限。
- `DeathTroops` / `DeathChance`:兵力少於 `DeathTroops` 時被擊敗的武將,以 `DeathChance` % 的機率戰死。可再出擊的兵力(原始 2000)不變。

#### 指令的花費・成功率 `[CommandCost]` `[CommandSuccess]`
- 可依指令編號設定花費(物資・兵力・金錢),例如 `40=5000,10000,20000`。`-1` 的項目維持原始值。
- 可設定畫面上會顯示機率的指令(登用、挖角、外交等)的成功率,例如 `35=30`。畫面上的機率也會改變,結果也會依照該機率。
- `CommandLog=1` 會在每次執行指令時,把編號・成敗・花費・報酬寫入 `mod_log.txt`。所有指令的編號都寫在 ini 中。

#### 侵攻・地區損壞度
- `InvasionChance`:AI 國家每月計畫侵攻的機率(%)。`InvasionChanceMul` 則是乘在原始機率上的倍率。目前的機率會顯示在 `mod_status.txt`。
- `TerritoryDamageMin` / `TerritoryDamageMax`(以及 `EnemyTerritoryDamage…`):襲擊・侵攻時增加的地區損壞度的隨機倍率,例如 `1` 和 `4` = 1〜4 倍。修復與每月的恢復不變。
- `FixSortieDamage=1`:修正官方錯誤:攻入已損壞的地區時,連**進攻方(自己的軍隊)**的出擊人數也會減少。改為只有防守方減少。

#### 同盟
- `AllianceMonths` / `AllianceMonthsMax`:新締結同盟的期間(月,原始 6)。兩者都設定時會在該範圍內隨機決定(例如 6〜12)。

#### 自動存檔 `[AutoSave]`
- 月份變更並回到軍略畫面時,會自動快速存檔(與按 Delete 相同)。從戰場回來時月份也會變更,所以也會存檔。
- 每次快速存檔(包括自己按的)都會在 `Savedata\ModAutoSave` 建立附日期時間的備份。保留數量由 `Keep` 設定。
- 要回到之前的狀態時,請先關閉遊戲,把備份檔覆蓋到 `Savedata\QuickSave.dat`,再啟動遊戲並快速讀檔。
- `BlockSoftReset`:阻止軟重置(Space + Delete / 手把 Start + Select)。`1` = 只在爭霸模式中(軍略畫面與戰場),`2` = 一律阻止。搭配自動存檔,就無法用重置來重來失敗的指令或戰死的武將。

#### 爭霸固定參數表 `[ConquestParam]`(進階使用者)
- 直接寫入遊戲的爭霸參數表(`P位置=值`)。大多數數值以 1000 倍儲存。

### 新功能:畫面

#### 畫面元件的位置・強化效果的剩餘時間 `[Hud]`
- `GaugeOffset` / `OrderOffset` / `KoOffset` / `MapOffset` / `BarOffset` / `ChainOffset` / `TargetOffset` / `BaseOffset`:移動自己的量表、指令框、K.O.COUNT、地圖(全體地圖與周邊地圖)、戰況量表、連擊數、敵方武將量表、據點耐久框。以 1280×720 座標寫「橫,縱」(往右與往下為 +)。`0,0` = 遊戲原始狀態。
- `BuffTime=1`:在畫面下方顯示強化效果的剩餘秒數(例如「攻擊 x3.0 25   速度 x1.8 90」)。對象為道具(戰神之斧・戰神之鎧・韋駄天靴)與秘計(攻擊・防禦・速度・攻擊速度・回復・無敵・羅剎・修羅・霸體・捕縛)。同種類重疊時顯示剩餘時間最長的。不顯示武器效果。位置由 `BuffX` / `BuffY` 設定。

### 新功能:戰場

#### 自動戰鬥 `[AutoBattle]`
- 按鍵盤 **F8 開啟 / F10 關閉**,即可把玩家角色交給與武將相同的 AI 操作(設定 `Enable=1` 後可使用)。
- 會進軍並壓制據點,自己的本陣受到攻擊時會回去防守。也會使用普通攻擊、防禦、蓄力攻擊與無雙。
- 沒有可攻擊的據點時,會去擊破敵方總大將(例如侵攻戰的最後)。
- 被設置物等卡住無法移動時,會往旁邊移動脫困(`Unstuck`)。
- 開啟期間,HP 顯示上方會顯示「AutoBattle」,受到的傷害變為 1/`PlayerDamageScale`(預設 3)。
- 鏡頭不會自動跟隨。AI 防禦時會轉向目標。

#### 本陣防衛 `[MainCamp]`
- 敵方武將靠近本陣(`Radius`)時,命令該軍的武將(玩家除外)回去防守本陣。我方與敵方可分別開關。
- 畫面外或正在戰鬥中的武將也會回來。不再受到攻擊經過 `ReleaseSeconds` 秒後,恢復原本的命令。
- 設定 `NoFightOnWay=1` 時,回防途中不會被敵人拖住。

#### 畫面外的進軍速度 `[OffscreenMarch]`
- 分別設定我方與敵方畫面外(未出現在戰場上)武將的進軍速度倍率。
- 回去防守自己本陣時,還可以再乘上額外的倍率(`AllyDefendSpeed` / `EnemyDefendSpeed`)。

#### 攻擊據點的速度 `[BaseCapture]`
- 分別設定我方與敵方 AI 武將在畫面外削減據點耐久力的速度(玩家不受影響)。

#### AI 的目標 `[AITarget]`
- `OfficerNearest=1`: 武將的 AI 在攻擊自己的對象之後,會鎖定附近(`NearRange`)的敵方武將。
- `StuckSeconds`: 選了到不了的目標而一直站著不動時,暫時排除該目標。
- `NearIncludePlayer=0`: 尋找附近的敵方武將時不計入玩家(防止後方的武將都聚集到玩家身邊)。
- `EnemyPlayerFirst`: 降低敵方 AI 優先尋找玩家的機率,讓敵人也會攻擊我方武將與士兵。

#### AI 強度・空中行動 `[AI]`
- `AllyLevelAdd` / `AllyLevelMin` / `EnemyLevelAdd` / `EnemyLevelMin`: AI 強度(0〜15)。武將本來就是最大值,主要影響士兵。
- `AirRecoverChance`: 被打到空中的武將進行受身的機率(原始為依 AI 強度 0〜50%)。
- 空中行動(僅限武將): 受身之後(`AirAfter…`)以及無法受身持續被打在空中時(`AirStuck…`),分別以機率使出空中無雙、蓄力攻擊、普通攻擊、跳躍。

#### 敵方兵長・士兵的攻擊力 `[EnemyPower]`
- 敵方據點兵長、遊擊隊長、士兵(武將的護衛與據點的士兵)造成傷害的倍率。不影響武將。

#### 武將能力・量表 `[Status]`
- `GaugeMax`: 武將資訊畫面量表的最大值(原始 1000),超過 1000 的能力也能看得到。
- `StatMax`: 能力上限(原始 1000,最大 30000)。使用提升能力的道具可以超過 1000。
- `StatRank`: 能力強化階段的最低值。寫成 `1-6` 這樣的範圍時,每場戰鬥會從中選一個(敵我雙方都會變強)。

#### 武器相性 `[Affinity]`
- `FlinchAtDisadvantage=1`: 即使自己相性不利,敵人也會被自己的攻擊打出硬直。

#### 我方 HP 顯示 `[AllyHP]`
- `ShowCamp=1`: 在列表最上方顯示自己本陣的耐久力(中文版為「本阵 321」,英文版為「Base 321」,日文版為「本陣 321」)。

### 修正與改進
- 英文版・中文版:位址表缺少 2 個位置,導致自動存檔(月份變更的判斷)與損壞度的本國/他國判斷無法運作,已修正。
- 爭霸模式:可修正攻入已損壞地區時連進攻方出擊人數也減少的官方錯誤(`FixSortieDamage`)。
- 新增防護,避免武將動作指令資料損壞時當機(包含在 `CrashGuardMotion` 中)。
- 修正侵攻戰中本陣被奪取的一方仍保留防衛命令,導致總大將等不行動的問題。
- `GaugeMax` 現在也會套用到第三個武將資訊畫面。

### 已知限制
- 目前還無法調整敵方兵長・士兵的**攻擊頻率**(只能使用攻擊力倍率)。
- 只有**畫面外**的武將回防速度會變快。出現在戰場上的武將維持原本的速度。
- 自動戰鬥期間鏡頭不會自動跟隨。
- 自動存檔會等到回到軍略畫面才執行。月初的事件與侵攻報告會先出現。
