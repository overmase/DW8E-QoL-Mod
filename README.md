# Dynasty Warriors 8 Empires Mod (dinput8.dll)

[日本語](README.ja.md) | **English** | [繁體中文](README.zh-TW.md)

overmase-AI-V~.exe installs a different mod than dinput8.dll. Please check the changes here to see what changes it makes.
[日本語](overmaseAI.ja_1.md) | [English](overmseAI.md)

An unofficial mod for **Dynasty Warriors 8 Empires** (Steam version). It works as a single `dinput8.dll` and is configured with `version.ini`. It fixes several bugs in the game, prevents crashes, and lets you adjust battle balance, AI behavior and the on-screen layout. It also has an **auto battle** mode that hands your character to the AI.

#When playing the game with this mod, it is recommended to play the game once with the default settings in the downloaded version.ini file.

#Please note that the author enjoys playing on a high difficulty level, and this mod will improve the overall quality of the game and increase its difficulty.

An unofficial mod for **Dynasty Warriors 8 Empires** (Steam version). It works as a single `dinput8.dll` and is configured with `version.ini`. It fixes several bugs in the game, prevents crashes, and lets you adjust battle balance, AI behavior and the on-screen layout. It also has an **auto battle** mode that hands your character to the AI.

Downloading this file may cause Windows to detect it as a virus, but this is a false positive. Please allow Windows Defender to process the file. It is open source on GitHub, and the mod is created using non-malicious code. overmase-AI-V~.exe is a patch file created with 357E Editor; this may also be falsely detected as a virus. The source files for the tool and 357E Editor can be found here: https://www.downxia.com/downinfo/382042.html

> **Beta for the English / Chinese versions.** The Japanese version has been tested a lot, but the English and Traditional Chinese versions have only been tested a few times. The newer features (`[Status]` and after, all conquest mode features, and `[Drop]` and after) have not been tested in-game on the English / Chinese versions yet. Please send a report if you find a problem (see [Reporting problems](#reporting-problems)).

See [RELEASE_NOTES.md](RELEASE_NOTES.md) for the changes since the previous release.

## Supported versions

The DLL checks the build stamp of `Launch.exe` and picks the right addresses automatically. One `dinput8.dll` and one `version.ini` work for all three.

| Version | Launch.exe build stamp |
|---|---|
| Japanese 1.0.5.0 | `0x56CEA82C` |
| English | `0x56CE9E3F` |
| Traditional Chinese | `0x56CEAB10` |

With any other `Launch.exe`, the DLL changes nothing. It writes `[停止]` (stopped) to `mod_log.txt` and the game runs normally.

Differences between the Files
The difference between overmase-AI-V~.exe and overmase-AI-V~anim_cut_musou is that the latter triggers the Musou Attack instantly without zooming in on the screen during activation.

Note: With overmase-AI-V~anim_cut_musou, Pang De's Musou Attack "Desert Storm" will not function properly. He will not mount his horse and will instead get stuck in the ground while swinging his weapon.

**Installation Guide

1. Download dinput8.dll, version.ini, and either overmase-AI-V~.exe or overmase-AI-V~anim_cut_musou depending on your preference.

2. Place all three files into your game directory where Launch.exe is located.
Example: C:\Program Files (x86)\Steam\steamapps\common\Dynasty Warriors 8 - Empires\

3. Run overmase-AI-V~.exe. If the window scrolls through text and displays "Success!" at the very end, the installation was successful.

4. Launch the game as you normally would.

**To update:** replace `dinput8.dll`. Either replace `version.ini` with the new one, or add the new sections to your own ini (missing keys keep the vanilla behavior).

**To uninstall:** delete `dinput8.dll` (and `version.ini` if you like).

Do not use Cheat Engine scripts that change the same code at the same time. If the DLL finds that a place was already changed by something else, it skips that feature and writes `[中止]` (skipped) to the log.

## Features

Every feature can be turned on or off, or adjusted, in `version.ini`. The file has comments in Japanese and English.

### Bug fixes and crash guards

| Section | What it does |
|---|---|
| `[Mod]` StealPatch | Fixes allied officers not appearing on the battlefield when the unit table is full |
| `[Mod]` UniqueModelBoxes | Raises how many edit / unique officers can show their own model at the same time (vanilla 6 → 14) |
| `[Mod]` LoadTableGuard | Prevents the crash when the officer load table (90 entries) becomes full |
| `[Mod]` CrashGuard… | Guards against crashes caused by bugs in the original game (including broken officer motion command data) |

### Soldiers and escorts

| Section | What it does |
|---|---|
| `[Mod]` AllyStrayZako / AllyEscort / EnemyEscort / EnemyLinkedZakoLevel | Turns allied stray soldiers and officers' escort squads on or off, and reduces the soldiers linked to enemy escorts |
| `[Mod]` EnemyEscortRewrite / BaseEscortRewrite / RoamingEscortRewrite | Changes the unit type of enemy escort squads (normal officers / base captains / roaming captains) |
| `[EnemyPower]` | Damage multiplier for enemy base captains, roaming captains and soldiers (officers are not affected) |

### Battle balance and stats

| Section | What it does |
|---|---|
| `[HpScale]` | HP multiplier per TypeID (damage reduction, or a longer HP gauge) |
| `[Affinity]` | Limits the slow-motion at a weapon affinity disadvantage, can remove the counter window, and can make enemies flinch even at a disadvantage (`FlinchAtDisadvantage`) |
| `[Status]` | Max value of the officer info gauges (`GaugeMax`), stat cap (`StatMax`, up to 30000), minimum stat rank (`StatRank`, a range like `1-6` also works) |
| `[Stratagem]` | Stratagem cooldown (player / allied AI / enemy), AI wait times, AI point gain, effect duration, area of ally stratagems, max value of the player's gauge |
| `[StratagemOverride]` | Cooldown and duration multipliers for individual stratagems (the number and name of every stratagem are listed in the ini) |
| `[WeaponAffinity]` | Attack bonus for each weapon affinity level (vanilla 2 / 5 / 7 / 10), optionally for the player only |

### AI and the battlefield

| Section | What it does |
|---|---|
| `[AI]` | AI level (allies / enemies), air recovery chance, and chances of an aerial Musou / charge / attack / jump after an air recovery or while being juggled |
| `[AITarget]` | Officer AI targets the nearest enemy officer, skips unreachable targets, and enemy AI stops focusing only on the player |
| `[BaseCapture]` | How fast AI officers reduce base durability off-screen (allies / enemies) |
| `[OffscreenMarch]` | March speed of off-screen officers (allies / enemies), plus extra speed when returning to defend the main camp |
| `[MainCamp]` | When enemy officers come near a main camp, that army's officers go back to defend it (allies / enemies). They can also ignore enemies on the way |
| `[AutoBattle]` | **Auto battle.** F8 = on / F10 = off (a pad button combination also works). The AI marches, captures bases, defends the main camp, attacks, guards and uses Musou. You can take less damage while it is on |

### Controls and target lock

| Section | What it does |
|---|---|
| `[Target]` | Double-tap guard (L1) to target the nearest enemy officer, double-tap again to release. Keep facing the target (`FaceTarget`: while guarding / always). Enemy and allied AI also face their target while guarding (`FaceTargetAI`) |

### Items and weapons

| Section | What it does |
|---|---|
| `[Drop]` | Soldiers drop items in conquest mode battles too (`ConquestDrop`), drop chance (`DropChance`), every squad member rolls (`AllMembers`) |
| `[BonusItem]` | Bonus item values: True Phoenix Wing / True White Tiger Fang / True Black Tortoise Shell, Bu Guang sword, Fish Scale Armor and Gan Jiang |
| `[Item]` | Up to 4 Musou bars (`MusouBarMax`, experimental). Air recovery with L1 at any time with the Wind Pearl (`UkemiAnytime`) |
| `[ItemParam]` | Directly changes effect values of equipment and bonus items (advanced) |

### Screen

| Section | What it does |
|---|---|
| `[Message]` | Moves the center banner (mission messages) and the dialogue window |
| `[Map]` | Adds a "hidden" step to the map toggle; moves and resizes the local map |
| `[AllyHP]` | Shows allied officers' names and HP at the left edge of the screen (F9 toggles it). Can also show your main camp durability on the first line (`ShowCamp`) |
| `[Hud]` | Moves HUD parts (player gauge, order panel, K.O. count, map, battle situation bar, combo counter, target officer gauge, base durability box), and shows the remaining time of power-ups (items and stratagems) at the bottom of the screen (`BuffTime`). Shows Musou bars and HP numbers as text (`PlayerInfo`, F9). Lets your HP gauge and the enemy officer's HP gauge grow beyond 1000 max HP (`HpBar…` / `TargetBar…`) |
| `[Camera]` | Camera distance and angle (FOV) for the normal and guard cameras |
| `[RawValue]` | Experimental values, for advanced users |

### Battlefield environment and graphics

| Section | What it does |
|---|---|
| `[Weather]` | During battle the weather (sunny, rain, snow, strong wind, storm, blizzard) and day / night change over time. Interval (minutes) and weather weights are adjustable |
| `[Display]` | Depth of field (blur) strength or off (`DepthOfField`, F11 toggles with vanilla), anisotropic filtering (`Anisotropy`) |
| `[Frame]` | Writes frame-time stats (average, max, late frames) to `mod_status.txt` (`Stats`), finer frame limiter (`PreciseWait`) |

### Conquest mode (Empires)

| Section | What it does |
|---|---|
| `[Conquest]` ExpBattle / ExpCommand / ExpMission | Experience multipliers for returning from battle, commands (training etc.) and in-battle missions, with optional random spread (`…Random`) |
| `[Conquest]` Max… / EnemyMax… | Caps for supplies, troops and gold (your country / other countries) |
| `[Conquest]` Command… / Request… / Income… / EnemyIncome… | Multipliers and random spread for supplies, troops and gold from commands, officer requests and monthly income |
| `[Conquest]` ExecuteChance | Chance that an AI ruler executes a captured officer (the rest are released) |
| `[Conquest]` DeathTroops / DeathChance | Officers defeated with fewer troops than `DeathTroops` die with `DeathChance` % (the re-sortie rule is not changed) |
| `[CommandCost]` / `[CommandSuccess]` | Cost (supplies, troops, gold) and success chance of each command, including diplomacy. `CommandLog=1` writes each command's number to `mod_log.txt` |
| `[Conquest]` InvasionChance / InvasionChanceMul | Chance per month that an AI country plans an invasion (fixed %, or a multiplier on the vanilla chance) |
| `[Conquest]` TerritoryDamageMin/Max / EnemyTerritoryDamageMin/Max | Random multiplier for territory damage from raids and invasions (your territories / other countries') |
| `[Conquest]` FixSortieDamage | Fixes the official bug where attacking a damaged territory also reduced **your** sortie slots |
| `[Conquest]` AllianceMonths / AllianceMonthsMax | Alliance length in months, fixed or random (e.g. 6–12) |
| `[Conquest]` HideDamageForecast | Hides the territory "damage forecast" (for randomized territory damage) |
| `[AutoSave]` | Quick-saves automatically when the month changes (also after returning from battle) and keeps timestamped backups in `Savedata\ModAutoSave`. `BlockSoftReset` blocks the soft reset (Space + Delete / Start + Select) in conquest mode or always |
| `[ConquestParam]` | Direct access to the conquest parameter table (advanced) |

The included `version.ini` is the author's own play settings (many features are on). Set a value back to the vanilla value written in the comment to return that part to normal.

**Not working yet:** `[Drop]` `WeaponDropChance`, `[Display]` `Width` / `Height`, everything in `[Debug]`, and `[EnemyPower]` `…Difficulty` / `…AiRow`. The ini also marks them with "These don't work now". Please leave them as they are.

## Log files

The DLL writes these files to the game folder. They are written in Japanese.

| File | Contents |
|---|---|
| `mod_log.txt` | Startup log. `[OK] … を入れました` = feature enabled, `[中止]` = skipped, `[設定で無効]` = turned off in the ini, `[エラー]` = invalid value in the ini |
| `mod_status.txt` | Counters for each feature, updated every 30 seconds |
| `mod_crash.txt` | Written only if the game crashes inside `Launch.exe` |

The second line of `mod_log.txt` should say `英語版 として動きます` (running as the English version) or `中国語版 として動きます` (running as the Chinese version).

## Reporting problems

Please open an Issue and attach:

- Your game version (Japanese / English / Traditional Chinese)
- `mod_log.txt`, `mod_status.txt` and, if it exists, `mod_crash.txt`
- Your `version.ini`
- What you were doing when the problem happened (mode, map, how long into the battle)

## Notes

- This is an unofficial fan mod. It is not affiliated with or endorsed by KOEI TECMO GAMES.
- The DLL changes the game only in memory while it runs. It does not change `Launch.exe` or the game data files.
- Use it at your own risk. Back up your save data before using it.
- `[AutoSave]` uses the game's own quick save. It overwrites `QuickSave.dat`; your normal save slots are not touched.
- To stop resetting right after a failed command or a fallen officer, use `[AutoSave]` together with `BlockSoftReset=1` (blocks the soft reset only in conquest mode).

## Credits

- **overmase** (main developer) https://steamcommunity.com/profiles/76561198400651705
- **もりね** (Morine)
- **絶対助けるマン** (Zettai Tasukeru Man)

## License

[MIT License](LICENSE)
