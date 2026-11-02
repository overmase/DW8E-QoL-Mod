# What's new (since the previous release)

These are the main changes since the first GitHub release. Most new features are **off (vanilla)** in the included `version.ini`; change only the values you want to use.

> **English / Traditional Chinese versions:** the new features were tested on the Japanese version. They are ported to the English and Chinese versions automatically but have not been tested in-game there yet. Please report any problem or crash.

## New features

### Auto battle `[AutoBattle]`
- Press **F8 to turn on / F10 to turn off** to let the officer AI control your character (set `Enable=1`).
- It marches to bases and captures them, and goes back when your main camp is attacked. It uses normal attacks, guard, charge attacks and Musou.
- When there are no bases left to attack, it goes after the enemy commander (e.g. at the end of an invasion battle).
- When it gets stuck on objects, it steps sideways to get free (`Unstuck`).
- While it is on, "AutoBattle" is shown above the HP list and you take 1/`PlayerDamageScale` damage (default 3).
- The camera does not follow automatically. It turns toward the target when the AI guards.

### Main camp defense `[MainCamp]`
- When enemy officers come near a main camp (`Radius`), that army's officers (except the player) are ordered to defend it. Allies and enemies can be turned on separately.
- Officers off-screen or in combat also come back. Their previous orders are restored `ReleaseSeconds` seconds after the attack stops.
- With `NoFightOnWay=1`, officers going back do not get stopped by enemies on the way.

### Off-screen march speed `[OffscreenMarch]`
- Multiplier for the march speed of officers that are off-screen (not spawned on the field), for allies and enemies separately.
- An extra multiplier applies while they return to defend their main camp (`AllyDefendSpeed` / `EnemyDefendSpeed`).

### Base capture speed `[BaseCapture]`
- How fast AI officers reduce the durability of a base off-screen, for allies and enemies separately (the player is not affected).

### AI targeting `[AITarget]`
- `OfficerNearest=1`: after the officer that attacked it, an officer's AI targets the nearest enemy officer within `NearRange`.
- `StuckSeconds`: if an officer keeps a target it cannot reach and stands still, that target is skipped for a while.
- `NearIncludePlayer=0`: the player is not counted when looking for the nearest enemy officer (stops officers far behind from crowding around the player).
- `EnemyPlayerFirst`: lowers the chance that enemy AI looks for the player first, so enemies also fight allied officers and soldiers.

### AI level and aerial actions `[AI]`
- `AllyLevelAdd` / `AllyLevelMin` / `EnemyLevelAdd` / `EnemyLevelMin`: AI level (0-15). Officers are already at the max, so this mainly affects soldiers.
- `AirRecoverChance`: chance that a juggled officer air-recovers (vanilla 0-50 % depending on AI level).
- Aerial actions (officers only): after an air recovery (`AirAfter…`) and when juggled without recovering (`AirStuck…`), chances for an aerial Musou, charge attack, normal attack and jump.

### Enemy captain / soldier damage `[EnemyPower]`
- Damage multiplier for enemy base captains, roaming captains and soldiers (escorts and base soldiers). Officers are not affected.

### Officer stats and gauges `[Status]`
- `GaugeMax`: max value of the gauges on the officer info screen (vanilla 1000), so stats above 1000 are visible.
- `StatMax`: stat cap (vanilla 1000, up to 30000). Stat-up items can raise stats above 1000.
- `StatRank`: minimum stat rank. Write a range like `1-6` to pick one per battle (makes both enemies and allies stronger).

### Weapon affinity `[Affinity]`
- `FlinchAtDisadvantage=1`: enemies flinch from your attacks even when you are at an affinity disadvantage.

### Allied HP display `[AllyHP]`
- `ShowCamp=1`: shows your main camp durability on the first line ("Base 321"; "本陣 321" on the Japanese version, "本阵 321" on the Chinese version).

## Fixes and improvements
- Added a guard against the crash caused by broken officer motion command data (part of `CrashGuardMotion`).
- Fixed defense orders staying active after an army lost its main camp in invasion battles, which left the commander and others idle.
- `GaugeMax` now also applies to the third officer info screen.

## Known limits
- The **attack frequency** of enemy captains and soldiers cannot be changed yet (only the damage multiplier works).
- Only **off-screen** officers return to the main camp faster. Officers on the field move at normal speed.
- The camera does not follow automatically during auto battle.
