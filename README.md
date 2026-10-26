# Dynasty Warriors 8 Empires Mod (dinput8.dll)

[日本語](README.ja.md) | **English** | [繁體中文](README.zh-TW.md)

An unofficial mod for **Dynasty Warriors 8 Empires** (Steam version). It works as a single `dinput8.dll` and is configured with `version.ini`. It fixes several bugs in the game, prevents crashes, and lets you adjust battle balance and the on-screen layout.

> **Beta for the English / Chinese versions.** The Japanese version has been tested a lot, but the English and Traditional Chinese versions have only been tested a few times. Please send a report if you find a problem (see [Reporting problems](#reporting-problems)).


## Supported versions

The DLL checks the build stamp of `Launch.exe` and picks the right addresses automatically. One `dinput8.dll` and one `version.ini` work for all three.

| Version | Launch.exe build stamp |
|---|---|
| Japanese 1.0.5.0 | `0x56CEA82C` |
| English | `0x56CE9E3F` |
| Traditional Chinese | `0x56CEAB10` |

With any other `Launch.exe`, the DLL changes nothing. It writes `[停止]` (stopped) to `mod_log.txt` and the game runs normally.

## Installation

1. Download `dinput8.dll` and `version.ini` `overmase-AI-V1.4.exe`.
2. Put files in the game folder (the folder that contains `Launch.exe`).
   Example: `C:\Program Files (x86)\Steam\steamapps\common\Dynasty Warriors 8 - Empires\`

3. If you launch `overmase-AI-V1.4.exe`, the window will scroll by, and if it says "Success!" at the end, the installation was successful.   
4. Start the game as usual.

**To uninstall:** delete `dinput8.dll` (and `version.ini` if you like).

If you download this file, Windows may detect it as a virus, but this is a false positive. Please allow Windows Defender to accept the file. It is open source on GitHub, and mods are created using non-malicious code.
overmase-AI-V1.4.exe is a patch file created with 357E Editor, and it may also be mistakenly detected as a virus. The source files for the tool and 357E Editor can be found here:
https://www.downxia.com/downinfo/382042.html

Do not use Cheat Engine scripts that change the same code at the same time. If the DLL finds that a place was already changed by something else, it skips that feature and writes `[中止]` (skipped) to the log.

## Features

Every feature can be turned on or off, or adjusted, in `version.ini`. The file has comments in Japanese and English.

| Section | What it does |
|---|---|
| `[Mod]` StealPatch | Fixes allied officers not appearing on the battlefield when the unit table is full |
| `[Mod]` AllyStrayZako / AllyEscort / EnemyEscort / EnemyLinkedZakoLevel | Turns allied stray soldiers and officers' escort squads on or off, and reduces the soldiers linked to enemy escorts |
| `[Mod]` EnemyEscortRewrite / BaseEscortRewrite / RoamingEscortRewrite | Changes the unit type of enemy escort squads (normal officers / base captains / roaming captains) |
| `[Mod]` UniqueModelBoxes | Raises how many edit / unique officers can show their own model at the same time (vanilla 6 → 14) |
| `[Mod]` LoadTableGuard | Prevents the crash when the officer load table (90 entries) becomes full |
| `[Mod]` CrashGuard… | 7 guards against crashes caused by bugs in the original game |
| `[HpScale]` | HP multiplier per TypeID (damage reduction, or a longer HP gauge) |
| `[Message]` | Moves the center banner (mission messages) and the dialogue window |
| `[Map]` | Adds a "hidden" step to the map toggle; moves and resizes the local map |
| `[Affinity]` | Limits the slow-motion when you are at a weapon affinity disadvantage; can remove the counter window |
| `[AllyHP]` | Shows allied officers' names and HP at the left edge of the screen (F9 toggles it) |
| `[Stratagem]` | Stratagem cooldown (player / allied AI / enemy), AI wait times, AI point gain, effect duration, area of ally stratagems, max value of the player's gauge |
| `[StratagemOverride]` | Cooldown and duration multipliers for individual stratagems (default: Provoke — 挑発, TypeID 60 — cooldown ×10, duration ×20) |
| `[Camera]` | Camera distance and angle (FOV) for the normal and guard cameras |
| `[RawValue]` | Experimental values, for advanced users |

Most values in the included `version.ini` are the mod's recommended settings. Set a value back to the vanilla value written in the comment to return that part to normal.

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

## Credits

- **overmase** (main developer)
- **もりね** (Morine)
- **絶対助けるマン** (Zettai Tasukeru Man)

## License

[MIT License](LICENSE)
