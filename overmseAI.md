# overmase-AI — Dynasty Warriors 8 Empires AI Mod

**English** | [日本語](README.ja.md)

An AI / gameplay overhaul mod for **Dynasty Warriors 8 Empires** (Steam).
Includes features ported from **TAU AI**, the **Reaper mod**, and the **Extreme mod**, plus original changes.

With this mod installed, both allies and enemies attack much more aggressively.

- Steam discussion thread: https://steamcommunity.com/app/322520/discussions/0/573804739774661296/

> [!WARNING]
> **The Chinese version of the game is not supported.**
> If you are using the Chinese version, try the alternative mod made by TAU instead:
> https://steamcommunity.com/app/322520/discussions/0/3417682380495884007/

---

## Versions

There are two versions. Choose whichever you prefer.

| | `overmase-AI-V~.exe` | `overmase-AI-V~anim_cut_musou` |
|---|---|---|
| Musou activation | Normal camera zoom-in | Skips the zoom-in and goes straight into the Musou Attack |
| Nearby officers heal on Musou | Yes (10% HP) | No |
| Pang De's Musou "Sabou Daiseijin" (砂暴大征陣) | Works | **Broken** (he doesn't mount his horse and swings his weapon while sunk into the ground) |

---

## Installation

1. Download either `overmase-AI-V~.exe` or `overmase-AI-V~anim_cut_musou` from [Releases](../../releases).
2. Place it in the game folder (the folder that contains `Launch.exe`).
   ```
   C:\Program Files (x86)\Steam\steamapps\common\Dynasty Warriors 8 - Empires\
   ```
3. Run the file. A console window will scroll through some text — if it says **`Success!`** at the end, the installation was successful.
4. Launch the game as usual.

---

## Changes

### Controls & Actions

1. **Guard → Dash Attack:** Releasing Guard from neutral (no stick input) now performs a Dash Attack.
   C1 (Charge Attack 1) can no longer be performed from neutral — tilt the stick and press Charge Attack to perform C1.
2. **No slow motion on Musou startup:** Hitting an enemy during the Musou startup animation (the "flash" before the Musou Attack) no longer triggers slow motion.
3. **Musou heal:** Characters near an officer who performs a Musou Attack recover 10% HP.
4. **Steerable moves:** Moves that lock your facing direction, such as Musou Attacks, can now be turned.
5. **Longer buffs:** Moves that grant buffs last roughly 2.5x–7.5x longer.
6. **Horses:** You cannot attack while mounted (to prevent a bug). In exchange, horse speed and jump height have been increased.
7. **Easier EX Attacks:** Input conditions for EX Attacks have been changed or added so they are easier to perform.
   For weapons with two or more EX Attacks, the extra EX Attacks have been added to empty slots after Charge Attacks.
   - Example: Spears have four — Ma Chao's two, Jiang Wei's, and the generic EX Attack can all be performed.
8. **Action restrictions removed:** Action restrictions and the restriction on EX Attacks for non-favored weapons have been removed.
   - Example: A character whose favored (EX) weapon is the Podao could previously only use that weapon's EX Attack and Shadowless Kick. Now, even with a bow they are not proficient with, they can still use EX Attacks and Air Dash (Lightness Skill).
   - Only Evade (転身) behaves strangely.
9. **Invincibility → Super Armor:** Moves with invincibility frames have been changed to Super Armor, or had the invincibility removed.
10. **Landing recovery:** Improved landing recovery after jumps.

### Weapons & Combat

11. **Attributes from Tier 1:** Attributes such as Speed are now assigned to weapons starting from Tier 1.
12. **Weapon nerf:** Attack power of all weapons has been reduced.
13. **Larger attack range:** The range of certain attacks, Charge Attacks, and Musou Attacks has been increased by 1.5x–2.8x, and they send enemies flying quite far.
    - *2026/11/03: The knockback has been fixed once; only the remaining attacks still launch enemies far.*
14. **Unimplemented weapons added:** All weapons that exist in the game data but were never implemented have been added. ([Example](https://steamcommunity.com/sharedfiles/filedetails/?id=3637397639))
    - Unimplemented gold weapons
    - Weapon types that only went up to Tier 5
    - Weapon types that had no Unique weapon
    - Weapons without assigned data appear with placeholder names such as "予備" (Reserve), but they can still be used.
15. **Elemental normal attacks:** Some normal attacks now carry elemental attributes, and more Charge Attack types now reflect the weapon's attribute.

### AI

16. **Full move usage:** Enemies and allies unconditionally use EX Attacks, Switch Attacks, Storm Rush, and Rage Musou Attacks (Awakening Musou).
17. **Musou activation rate at max morale** (the thing that causes 1-vs-3 gang beatdowns):

    | Situation | Before | After |
    |---|---|---|
    | 1 vs multiple | 100% | 90% |
    | 1 vs 1 | 80% | 70% |

    This just gives you a bit of leeway to back off — they'll still spam Musou Attacks.
18. **Aura activation rate:** The AI's aura activation rate when entering combat mode has been changed from 90% → 40%–70% (the black-red aura at max morale, or that Lu Bu aura).

### Empire Mode

19. **Unique weapons in shops:** If you build 7 or more Weapon Shops, you can purchase Unique weapons.

---

## Known Issues

- **Generic officer AI:** The AI of officers using generic officers' weapons, and of officers without a Musou 2 or Aerial Musou, behaves oddly. (The officers who appear in Requests are pretty insane.)
- **Leftover parameters:** Some parameters I changed to make values easier to search for in 357E Editor have been left as-is, so things like weapon purchase prices remain increased.
- **Evade (転身):** Behaves strangely when used with non-favored weapons (see Changes #8).
- **Pang De (`anim_cut_musou` only):** His Musou "Sabou Daiseijin" does not work properly.

---

<sub>Tags: Dynasty Warriors 8 Empires AI / Dynasty Warriors 8 Empires AI mod / 真・三國無双7 Empires AI mod</sub>
