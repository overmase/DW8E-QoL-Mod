# What's new — 2026-11-25 (272)

Changes since the previous tester build **2026-11-13 (163)**. Older changes are listed further down under "Changes in earlier builds".

> **About the included `version.ini`:** it is again the author's own play settings. It also reworks the "Bond" and "Focused Fire" stratagems (see "Stratagems" below) and turns off the camera's wall collision. To test something closer to the original game, set each value back to the "vanilla" value written in its comment.

> **English / Traditional Chinese versions:** the new features were tested on the Japanese version. They are ported to the English and Chinese versions automatically but have not been tested in-game there yet. Please report any problem or crash.

## Stratagems

### Edit stratagem values directly `[StratagemParam]` / `[StratagemInfo]` / `[StratagemShotParam]` (advanced)
- `[StratagemParam]` `type.offset(hex)=value`: rewrites a value in the stratagem definition: effect type, duration, strength, cost, cooldown, AI use chance, and for large-scale stratagems the number of units that retreat (`+26`), the delay before it fires (`+28`) and the number of bases needed (`+2A`), whether it also affects allies (`+32`) and its radius (`+34`), etc.
  - The original values of every stratagem are written to **`DW8E_StratagemParam_Default.txt`** in a copy-and-paste format (made when you enter a battle with `[StratagemAI] Dump=1`). Copy only the lines you want to change into `[StratagemParam]` and edit the numbers.
  - A line equal to the game's original value does nothing (multipliers such as `[StratagemOverride]` still apply).
  - Example (included ini): `64.26=0` to `77.26=0` — officers no longer retreat when a large-scale stratagem is used.
- `[StratagemInfo]` `type.offset=value`: rewrites the stratagem info table (unlocked from the start, category, usable modes).
- `[StratagemShotParam]`: rewrites the values of objects fired by attack stratagems (fire blast etc.).

### Make Bond and Focused Fire usable `[StratagemAI] Unlock`
- `Unlock=48,52`: unlocks "Bond" and "Focused Fire" — they exist in the data but could not be used — from the start, equippable in every mode.
- The included ini reworks both:
  - **Bond**: raises move speed and attack speed of yourself and nearby allies for 25 seconds (`48.04` / `48.06` / `48.08` / `48.0E` / `48.10` / `48.32` / `48.34`).
  - **Focused Fire → Focused Attack**: same effect as Summon, pulls 2 officers to you (`52.06=49` and `[StratagemAI] SummonCount=52:2`).

### Number of bases taken by Base Raid / Base Seize `[StratagemAI]`
- `BaseRaidCount` / `BaseSeizeCount` (1–19, vanilla 1): number of bases taken by Base Raid / Base Seize. Also applies when the enemy uses them.

### Number of officers summoned `[StratagemAI]`
- `SummonCount=type:count,…` (1–6): how many officers a stratagem with the Summon effect pulls. Example: `52:2`.

### Replace game text `[Text]`
- `id=text`: replaces a game text (ini is UTF-8, `\n` for a new line). The ids are the same as in `DW8E_Text.txt` made by `[Debug] DumpText=1`.
- Stratagem name = 20878 + type, description = 21134 + type, short description = 21262 + type.
- The included ini (Japanese text) renames Focused Fire to "Focused Attack" and updates the Bond / Focused Attack descriptions.

## Rescue `[Rescue]`
- Like "use this to heal" in request battles: when you get near (`Range`) an allied officer whose HP is low (`HpBelow` %), their HP is restored to `HealTo` %. "Rescued ○○" is shown at the top of the screen. The same officer waits `Cooldown` seconds. Turn on with `Enable=1`.

## AI

### Weapon switching
- `[AI] AffinitySwitchAlly` / `AffinitySwitchEnemy`: allied / enemy officers also switch weapons when they are not at an advantage against the officer they are targeting (`AffinitySwitchCooldownMs`, default 12000 = 12 seconds per officer).
- `[AI] AISwitch` (experimental): stops the AI's switch attack (the delayed switch) (`1`), or replaces it with another motion (`SwitchReplace`) (`2`).

### Target choice `[AITarget]`
- `SkipDeadTarget=1`: the AI stops targeting someone whose HP is 0 (a fallen body). Prevents allies from juggling a defeated enemy forever so the body never disappears and nothing moves on.

### Auto battle `[AutoBattle]`
- `UseStratagem=1`: uses stratagems during auto battle, with the same decision as officer AI.
- `UseInstallStratagem`: use (`1`) / don't use (`0`) placed stratagems (tower, catapult, altar, traps).
- `AffinitySwitch=1`: checks affinity against the targeted enemy officer and switches with R1 if the other weapon is better (`AffinitySwitchRange` / `AffinitySwitchCooldownMs`).

## Camera `[Camera]`
- `WallCollision=0`: turns off the camera's collision with walls and placed objects. The camera no longer gets stuck near walls (you may see inside or behind walls).

## Conquest `[Conquest]`

- **Growth of other officers** `AllyGrowth` / `EnemyGrowth`: multiplier for merit (EXP) gained by officers of your force / other forces and free officers. Applies to monthly growth, battles and commands. Does not apply to the player. Levels go up the next day as in the game.
- **Fatigue after a battle** `FatigueAdd` / `FatigueMul`: a fixed amount of fatigue to add (`-1` = off), or a multiplier for the vanilla amount (about 20 per battle).
- **Stats lowered by fatigue** `FatigueStat=heavy,medium,light` (vanilla 70,80,90) / `FatigueStat2=heavy/light,medium` (vanilla 50,70): stat % at each fatigue stage. The numbers on the officer info screen change too.
  - HP and defense get item bonuses added afterwards, so they look less affected than attack and command.
- `AllianceMonths` now accepts a range like `8-12`.
- `StageMap`: replaces the battlefield of a conquest region, same format as Free Mode.
- `[CommandCost]`: Retire (87) can also be given troop / gold costs (example in the ini).
- **Fix**: with `FixSortieDamage=1`, the attacker's sortie slots were also reduced when you were not the ruler or when another force attacked. Fixed.

## Starting situation `[Situation]`

- `TroopRatio`: redoes the start-of-battle base split as if your troops were "enemy troops × this value". Around `0.2` is a heavy disadvantage, `0` = off, `0.1-0.5` = random. The same multiplier goes into the situation gauge.

## Free Mode `[FreeMode]`

- **Random candidates** `RandomUnique` / `RandomGeneric` / `RandomCustom`: choose whether unique / generic / edit officers are included in the random sortie officers (not filtered if fewer than 16 would remain).
- **Replace the request battle brigade leader** `RequestOfficer=1` (experimental): replaces the fixed "brigade leader" in Free Mode request battles with an officer from the candidates (`RequestUnique` / `RequestGeneric` / `RequestCustom`).
- **Special maps** `ExtraStages=29,28`: adds the previous game's Xu Chang (escort) / Luoyang (assassination) maps to the Free Mode stage list (28, 29, 143–149 can be used).
  - `StageMap=region:id,…` / `AttackStage`: changes the battlefield when a region is chosen (0–23, 28, 29, 140–149; 24–27 crash and cannot be used).
  - `StageName<id>` (Japanese version) / `StageName<id>_EN` (English version, ASCII only) / `StageName<id>_CN` (Chinese version, Simplified): name shown in the list.

## Bases `[Base]` / `[BaseUpgrade]` / `[BaseEffect]`

- **Random bases** `RandomAlly` / `RandomEnemy` (%): in Conquest / Free Mode, replaces bases that start as normal bases with supply / attack / defense bases. Level from `RandomLevelAlly` / `RandomLevelEnemy` (e.g. `2-4`), type weights from `RandomWeight=supply,attack,defense`.
- **Main / sub camp level** `MainCampLevelAlly` / `MainCampLevelEnemy` / `SubCampLevelAlly` / `SubCampLevelEnemy`: 1–4, or random like `2-4`.
- **Base durability** `DurabilityLv=Lv1,Lv2,Lv3,Lv4` / `DurabilityCamp` (main / sub camps; one value for all, or four for each level): durability multiplier when the battle starts.
- **Base upgrade cost** `LevelCost` (level up, Lv2,Lv3,Lv4) / `RebuildCost` (rebuild, current Lv 1–4): Conquest supplies. `FreeLevelCost` / `FreeRebuildCost` are Free Mode bonus points. One value = all the same, empty = vanilla. `CostMul` / `FreeCostMul` multiply the ones not written.
- **Base effects** `AttackTable` / `DefenseTable` / `SupplyTable`: value when you hold 1, 2, 3 … bases (vanilla 150,180,200 …). Attack = damage × (1 + value × 0.0025), defense = max HP × (value + 100) ÷ 200, supply = troop recovery × value ÷ 100.
- `HPCap`: battle HP cap (vanilla 1000). Keeps the defense base multiplier from being lost at 1000.
- `DefenseLive`: re-applies HP when the number of defense bases changes (1 = player / 2 = officers too). In the vanilla game the player does not get the defense base effect.
- The base level cap of 4 cannot be changed.

## Enemy escorts `[Mod]`

- `EnemyOfficerEscortSize` / `EnemyBaseEscortSize` / `EnemyRoamingEscortSize`: multiplier for the number of escort soldiers of enemy officers / base captains / roaming captains (up to 30 per unit). Only regular soldiers (soldiers, guards, cavalry, archers, etc.) increase.
  - **Raising these above 1 is not recommended** (see "Known issues" below).
- `EscortSizeLimit` (default 80, 0 = none): cap on the number of people on the battlefield when escorts are increased.
- Changed what `EnemyLinkedZakoLevel` below 4 does with `EnemyEscort=1`: `0` = no escorts, `1`–`3` = enemy officer escort size × 0.3 / 0.5 / 0.7.
- **Fix**: with `EnemyEscort=1` and `EnemyLinkedZakoLevel` below 4, many invisible enemy officer icons appeared on the map and auto battle stopped moving on. Fixed.
- `SquadId` / `BaseSquadId` / `RoamingSquadId` no longer accept battlefield-only units (tigers, siege weapons, sorcerers, bandits, etc.) because they are invisible or crash (an error is written to mod_log).

## Random weapons `[Chaos]`

- `RandomWeapon=1`: when a battle starts, your two weapons are randomized from all types (affinity random too). They go back to your original two when you leave the battle.
- `RandomWeaponTier` (1–10, default 3): which strength rank of each weapon type is used. A range like `4-9` also works.

## AI behavior `[AI]` / `[AITarget]` / auto battle / main camp defense

- **Aggressive AI** `[AI] AggressivePlayer` / `AggressiveAlly` / `AggressiveEnemy` (0–100 %): like the demo play AI, skips "wait and see" actions with this chance. Player = yourself in auto battle.
- **When pressed up close** `[AI] CloseIdleMs` / `CloseIdleRange` / `CloseIdleGuard` / `Attack` / `Charge` / `Jump` / `Move`: when an enemy officer stays idle right next to you for `CloseIdleMs`, makes it guard / attack / charge / jump / move a little by chance.
- **Guard release delay** `[AI] AllyGuardHoldMs` / `EnemyGuardHoldMs`: the AI keeps guarding for this long (ms) after it would stop.
- **Guard against attacks** `[AI] ReactGuardNormal` / `Charge` / `Switch` / `EX` / `Storm` (%), `ReactGuardRange` / `ReactGuardMs`.
- **After guarding** `ReactAfterJump` (jump attack at the opponent) / `ReactAfterCombo` (close in with a normal combo) (%), `ReactComboMs` / `ReactComboRange`.
- `[Target] TurnSpeed`: turning speed. **2.25 or more breaks movement when hit by a grab-type Musou**, so keep it at 2.2 or less.
- `[AutoBattle] UnstuckAttack`: also uses normal attacks while sliding sideways when stuck or while jumping (1 = sideways, 2 = jump, 3 = both).
- `[MainCamp] OnlyWhenReachable=1`: does not go back to defend the main camp when the attacker's bases do not connect to it (the main camp cannot be taken).
- `[AITarget] EscortNearRange` (default 0, recommended 600–1000): officers you lead from an escort request target an enemy officer within this distance. Works only with `OfficerNearest=1`.
- **Fix**: with `[AITarget] OfficerNearest=1`, officers from an escort request stood still doing nothing even with enemies nearby. Fixed.

## Screen `[Hud]`

- `TargetHP=1`: shows the targeted enemy officer's HP (current / max) next to the level.
- `PlayerInfoLevel=1`: shows your level at the start of the PlayerInfo text.
- `TargetLevel=1`: shows the targeted enemy officer's level under the gauge (position `TargetLevelX` / `TargetLevelY`).
- `LevelAdd`: correction when the shown level is off by 1.
- **Fix**: the enemy officer HP gauge at the top left stopped at max HP 1000 (when `TargetBarMaxHP` is not 1000). Fixed.

## Other / fixes

- `[Mod] RandomSeed=1`: the game's random numbers were the same sequence every launch, so Free Mode random officers etc. were always the same. Fixed.
- `[Debug] DemoSeconds`: length of the demo play (the battle that starts when you wait at the title screen) in seconds (vanilla 60).
- **Fix**: target lock by pressing guard twice sometimes suddenly stopped you from moving left / right and pulled you toward the opponent (like `FaceTarget=2`). Fixed.
- **Fix**: after adding `SkipDeadTarget`, the game could crash while the AI was deciding on a stratagem. Fixed (272).

## Known issues / not possible yet

- **Only 150 bodies in total can be on the battlefield.** If escort sizes (`EnemyOfficerEscortSize` / `EnemyBaseEscortSize` / `EnemyRoamingEscortSize`) or base soldiers (`[Base] GarrisonSoldiers`, experimental) push past that, you get invisible, unkillable soldiers that exist only as map icons, the AI swings at nothing, and bases can't be captured. Keep these at 1 / 0.
  - Some bases having about 21 soldiers and others 50 or more is how the vanilla game works.
- Officers from an escort request fight soldiers and captains but sometimes not enemy officers. This also happens in the vanilla game.
- Stratagem names / descriptions changed with `[Text]` show in the stratagem screens, but **the stratagem slot on the battlefield (card name and short description) does not change yet**.
- `[StratagemAI] Use` (make the AI use stratagems it normally doesn't) is on hold. Stratagems like Impregnable Fortress, Rumor and Base Repair have other per-stratagem conditions that stop the AI from using them. The included ini leaves it empty.
- `[Camera] TerrainCollision` had no effect. Keep it at `1`.
- `[AI] AirEscape*` (breaking out with a recovery / aerial Musou when juggled in the air for a long time) is experimental.
- Animals (tigers), siege weapons, etc. cannot be put on normal battlefields (the battlefield-only data is missing; they are invisible or crash).
- Random weapons are limited to two.
- `RequestOfficer` is experimental.
- Extra maps may show a different name on the battle preparation screen.
- Keep the `[Debug]` investigation keys (`WatchWrite` / `BaseOwnerProbe` / `StratagemAIProbe` / `SwitchProbe` / `SquadProbe` / `DemoProbe` / `BattleProbe` / `ExpProbe` / `ArmyProbe` / `DumpPersons` / `BaseProbe` / `GaugeProbe` / `BaseCrowdProbe`, etc.) at `0`.

---

# Changes in earlier builds — 2026-11-13 (163)

Changes since the previous tester build **2026-11-09 (129)**. Older changes are listed further down under "Changes in earlier builds".

> **About the included `version.ini`:** this time the `version.ini` is the author's own play settings, so **many features are turned on** (in earlier builds most of them were off). To test something closer to the original game, set each value back to the "vanilla" value written in its comment.

> **English / Traditional Chinese versions:** the new features were tested on the Japanese version. They are ported to the English and Chinese versions automatically but have not been tested in-game there yet. Please report any problem or crash.

## New features: battlefield environment and graphics

### Changing weather and day / night `[Weather]`
- During battle, the **weather** and **day / night** change as time passes (`Enable=1`).
- `WeatherMinutes`: minutes between weather changes. 4 in the included ini.
- `DayNightMinutes`: minutes between day and night. 8 in the included ini.
- `Sunny` / `Rain` / `Snow` / `Wind` / `Storm` / `Blizzard`: how often each weather is picked (weights; they do not need to add up to 100).
- The weather types are the same ones the altars use (sunny 0 / rain 2 / snow 4 / strong wind 18 / storm 20 / blizzard 5).
- Time is only counted during battle (while your character is on the field).

### Turn off depth of field (blur) `[Display]`
- `DepthOfField`: strength of the near / far blur. `1` = vanilla, `0` = no blur, `0.5` = half. `0` (no blur) in the included ini.
- `DofToggleKey`: in battle, this key switches between the value above and vanilla (default 122 = **F11**).

### Anisotropic filtering `[Display]`
- `Anisotropy`: makes ground and wall textures seen at an angle sharper (2 / 4 / 8 / 16). 16 in the included ini. `0` = vanilla.

### Frame-time log `[Frame]`
- `Stats=1`: writes the average / maximum frame time and the number of late frames to [フレームの間隔] in `mod_status.txt`. Useful when reporting stutter.
- `PreciseWait=1`: a finer frame limiter (stays at 60 FPS). The vanilla timing was fine on the author's PC, so it is `0` in the included ini.

## New features: controls

### Target lock `[Target]`
- `DoubleTapGuard=1`: **double-tap guard (L1)** to target the nearest enemy officer. Double-tap again to release. `DoubleTapMs` = max time between the presses, `Range` = search distance.
- `FaceTarget`: keep facing the target. `1` = only while guarding, `2` = always (your running direction is pulled toward it too).
- `FaceTargetAI=1`: other characters (enemies and allies) also keep facing their target while guarding.

## New features: items and weapons

### Items dropped by soldiers `[Drop]`
- `ConquestDrop`: `1` = soldiers also drop items (meat buns, wine, Ares' Axe…) in conquest mode battles (vanilla: no drops). `2` = in every mode.
- `DropChance`: drop chance (%). `-1` = vanilla (90–5 % by difficulty).
- `AllMembers=1`: every soldier in a squad rolls for an item (vanilla: only the squad leader).

### Weapon affinity `[WeaponAffinity]`
- Changes the attack bonus for each weapon affinity level (`Star1`–`Star4`; vanilla 2 / 5 / 7 / 10). `PlayerOnly=1` = player only.

### Bonus items `[BonusItem]`
- `TrueHP` / `TrueAttack` / `TrueDefense`: values of True Phoenix Wing / True White Tiger Fang / True Black Tortoise Shell (vanilla 1000).
- `HokouPercent` / `HokouTime`: % and duration of the Bu Guang sword (attack boost at the start of battle).
- `GyorinPercent` / `KanshoPercent`: % of Fish Scale Armor (raises allied officers' abilities) / Gan Jiang (lowers enemy officers' abilities).
- The game's original values are shown under [特典アイテム] in `mod_status.txt`.

### Equipment items `[Item]`
- `MusouBarMax=4`: up to 4 Musou gauge bars. The Azure Dragon item adds the 4th bar even at Lv 24+ (**experimental**).
- `UkemiAnytime=1`: with the Wind Pearl equipped, you can **air-recover with L1 at any time** while being juggled (vanilla: only near the end of the hit reaction while falling).

### Item effect values `[ItemParam]` (advanced)
- Directly changes the effect values of equipment and bonus items (e.g. Swift Talisman move speed, White Blade chance and multiplier). The original values are listed under [アイテムの値] in `mod_status.txt`.

## New features and improvements: HUD `[Hud]`
- `PlayerInfo=1`: shows your **Musou bars (current/max)** and **HP numbers (current/max)** as text (F9 toggles it; position `InfoX` / `InfoY`).
- `HpBarMaxHP` / `HpBarWidth`: length of your HP gauge. In vanilla it stops growing at 1000 max HP; this lets it keep growing.
- `TargetBarMaxHP` / `TargetBarWidth`: length of the targeted enemy officer's HP gauge (also beyond 1000).
- `TargetBarEffectiveHP=1`: includes the `[HpScale]` Mode=1 (damage reduction) factor in the enemy officer's HP gauge (HP as you actually have to chip it away).

## Other
- `[Conquest]` `HideDamageForecast=1`: hides the territory "damage forecast" (it is wrong when territory damage is randomized). Added to the ini.
- `[StratagemOverride]`: the number and name of every stratagem is now listed in the ini (Japanese / English / Chinese).

## Known issues and limits
- `[Drop]` `WeaponDropChance` (weapon box drop chance from enemy officers) **does not work yet**. Please leave it as is.
- `[Display]` `Width` / `Height` (higher resolutions) **cannot be used yet**. Keep them at `0`.
- The `[Debug]` options (research into the leftover PS3 online screens) are for testing only. Keep them at `0`.
- `[EnemyPower]` `…Difficulty` / `…AiRow` have no effect. Keep them at `-1`.
- `[CommandSuccess]` does not accept a range (like `20-60`). Use a single number.
- `[Item]` `MusouBarMax=4` is experimental.
- There are only two times of day: day and night.

---

# Changes in earlier builds — 2026-11-09 (129)

### New features: conquest mode

All of these are in the `[Conquest]` section unless another section is named.

#### Experience and resources
- `ExpBattle` / `ExpCommand` / `ExpMission`: experience multipliers for returning from battle (all officers), commands such as training, and in-battle missions. `ExpBattleRandom` / `ExpCommandRandom` add a random spread (±%). The numbers on the result screen and the command screen change too.
- `MaxSupplies` / `MaxTroops` / `MaxGold` (and `EnemyMax…`): caps for your country and for all other countries (vanilla 999999).
- `Command…` / `Request…` / `Income…` / `EnemyIncome…`: multipliers and random spread for supplies, troops and gold from commands, officer requests and monthly income.

#### Execution and death
- `ExecuteChance`: chance (%) that an AI ruler executes a captured officer; the rest are released. This applies to enemy rulers, and to the ruler of your own country when you are not the ruler yourself. Officers the ruler recruits first are not affected.
- `DeathTroops` / `DeathChance`: officers defeated with fewer troops than `DeathTroops` die with `DeathChance` %. The re-sortie threshold (vanilla 2000 troops) is not changed.

#### Command cost and success rate `[CommandCost]` `[CommandSuccess]`
- Set the cost (supplies, troops, gold) of each command by its number, for example `40=5000,10000,20000`. `-1` keeps the vanilla value for that item.
- Set the success chance of commands that show a chance on screen (recruit, headhunt, diplomacy…), for example `35=30`. The chance on screen changes too, and the result always matches it.
- `CommandLog=1` writes the number, result, cost and reward of every command to `mod_log.txt`. The ini lists the numbers of all commands.

#### Invasions and territory damage
- `InvasionChance`: chance (%) per month that each AI country plans an invasion. `InvasionChanceMul` multiplies the vanilla chance instead. The current chance is shown in `mod_status.txt`.
- `TerritoryDamageMin` / `TerritoryDamageMax` (and `EnemyTerritoryDamage…`): random multiplier for the territory damage from raids and invasions. For example, `1` and `4` = 1–4 times. Repairs and monthly recovery are not changed.
- `FixSortieDamage=1`: fixes the official bug where attacking a damaged territory also reduced **your** sortie slots. Only the defender loses slots, as intended.

#### Alliances
- `AllianceMonths` / `AllianceMonthsMax`: alliance length in months for new alliances (vanilla 6). Set both for a random length, for example 6–12.

#### Auto save `[AutoSave]`
- When the month changes and you are back on the strategy menu, the mod quick-saves automatically (the same as pressing Delete). Returning from battle also changes the month, so it is saved too.
- Every quick save (including your own) is copied to `Savedata\ModAutoSave` with the date and time. `Keep` sets how many copies to keep.
- To go back to an older state: close the game, copy a backup over `Savedata\QuickSave.dat`, start the game and quick-load.
- `BlockSoftReset`: blocks the soft reset (Space + Delete / Start + Select). `1` = only in conquest mode (strategy screen and battles), `2` = always. With auto save, you can no longer undo a failed command or a fallen officer by resetting.

#### Conquest parameter table `[ConquestParam]` (advanced)
- Write values directly into the game's conquest parameter table (`P<offset>=value`). Most values are stored ×1000.

### New features: screen

#### HUD positions and power-up timers `[Hud]`
- `GaugeOffset` / `OrderOffset` / `KoOffset` / `MapOffset` / `BarOffset` / `ChainOffset` / `TargetOffset` / `BaseOffset`: move the player gauge, order panel, K.O. count, map (whole map and local map), battle situation bar, combo counter, target officer gauge and base durability box. Write "x,y" in 1280×720 coordinates (right / down = +). `0,0` = vanilla.
- `BuffTime=1`: shows the remaining seconds of power-ups at the bottom of the screen, for example "ATK x3.0 25   SPD x1.8 90   INVINCIBLE 12". Items (Ares' Axe / Armor, Speed Boots) and stratagems (attack, defense, speed, attack speed, regeneration, invincibility, Rakshasa, Asura, super armor, bind) are shown. When several power-ups of the same kind are active, the one with the longest time left is shown. Weapon effects are not shown. `BuffX` / `BuffY` set the position.

### New features: battlefield

#### Auto battle `[AutoBattle]`
- Press **F8 to turn on / F10 to turn off** to let the officer AI control your character (set `Enable=1`).
- It marches to bases and captures them, and goes back when your main camp is attacked. It uses normal attacks, guard, charge attacks and Musou.
- When there are no bases left to attack, it goes after the enemy commander (e.g. at the end of an invasion battle).
- When it gets stuck on objects, it steps sideways to get free (`Unstuck`).
- While it is on, "AutoBattle" is shown above the HP list and you take 1/`PlayerDamageScale` damage (default 3).
- The camera does not follow automatically. It turns toward the target when the AI guards.

#### Main camp defense `[MainCamp]`
- When enemy officers come near a main camp (`Radius`), that army's officers (except the player) are ordered to defend it. Allies and enemies can be turned on separately.
- Officers off-screen or in combat also come back. Their previous orders are restored `ReleaseSeconds` seconds after the attack stops.
- With `NoFightOnWay=1`, officers going back do not get stopped by enemies on the way.

#### Off-screen march speed `[OffscreenMarch]`
- Multiplier for the march speed of officers that are off-screen (not spawned on the field), for allies and enemies separately.
- An extra multiplier applies while they return to defend their main camp (`AllyDefendSpeed` / `EnemyDefendSpeed`).

#### Base capture speed `[BaseCapture]`
- How fast AI officers reduce the durability of a base off-screen, for allies and enemies separately (the player is not affected).

#### AI targeting `[AITarget]`
- `OfficerNearest=1`: after the officer that attacked it, an officer's AI targets the nearest enemy officer within `NearRange`.
- `StuckSeconds`: if an officer keeps a target it cannot reach and stands still, that target is skipped for a while.
- `NearIncludePlayer=0`: the player is not counted when looking for the nearest enemy officer (stops officers far behind from crowding around the player).
- `EnemyPlayerFirst`: lowers the chance that enemy AI looks for the player first, so enemies also fight allied officers and soldiers.

#### AI level and aerial actions `[AI]`
- `AllyLevelAdd` / `AllyLevelMin` / `EnemyLevelAdd` / `EnemyLevelMin`: AI level (0-15). Officers are already at the max, so this mainly affects soldiers.
- `AirRecoverChance`: chance that a juggled officer air-recovers (vanilla 0-50 % depending on AI level).
- Aerial actions (officers only): after an air recovery (`AirAfter…`) and when juggled without recovering (`AirStuck…`), chances for an aerial Musou, charge attack, normal attack and jump.

#### Enemy captain / soldier damage `[EnemyPower]`
- Damage multiplier for enemy base captains, roaming captains and soldiers (escorts and base soldiers). Officers are not affected.

#### Officer stats and gauges `[Status]`
- `GaugeMax`: max value of the gauges on the officer info screen (vanilla 1000), so stats above 1000 are visible.
- `StatMax`: stat cap (vanilla 1000, up to 30000). Stat-up items can raise stats above 1000.
- `StatRank`: minimum stat rank. Write a range like `1-6` to pick one per battle (makes both enemies and allies stronger).

#### Weapon affinity `[Affinity]`
- `FlinchAtDisadvantage=1`: enemies flinch from your attacks even when you are at an affinity disadvantage.

#### Allied HP display `[AllyHP]`
- `ShowCamp=1`: shows your main camp durability on the first line ("Base 321"; "本陣 321" on the Japanese version, "本阵 321" on the Chinese version).

### Fixes and improvements
- English / Chinese versions: auto save (detecting the month change) and the own/other country check of territory damage did not work because two addresses were missing. Fixed.
- Conquest mode: fixed the official bug where attacking a damaged territory also reduced the attacker's sortie slots (`FixSortieDamage`).
- Added a guard against the crash caused by broken officer motion command data (part of `CrashGuardMotion`).
- Fixed defense orders staying active after an army lost its main camp in invasion battles, which left the commander and others idle.
- `GaugeMax` now also applies to the third officer info screen.

### Known limits
- The **attack frequency** of enemy captains and soldiers cannot be changed yet (only the damage multiplier works).
- Only **off-screen** officers return to the main camp faster. Officers on the field move at normal speed.
- The camera does not follow automatically during auto battle.
- Auto save waits until you are back on the strategy menu. Events and invasion reports at the start of a month come first.
