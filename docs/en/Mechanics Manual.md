# MaxsuPoise Mechanics Manual

This document introduces you to the detailed mechanics of MaxsuPoise, including Requiem-specific enhancements and TrueHUD integration.

## Table of contents

- [**What's New**](#Whats-New)
- [**Calculating Total Poise Health**](#Calculating-Total-Poise-Health)
- [**Calculating Poise Damage**](#Calculating-Poise-Damage)
- [**Calculating Total Immune Level**](#Calculating-Total-Immune-Level)
- [**Trigger Stagger**](#Trigger-Stagger)
- [**Poise Health Regen**](#Poise-Health-Regen)
- [**Infinite StaggerLock Prevention**](#Infinite-StaggerLock-Prevention)
- [**TrueHUD Integration**](#TrueHUD-Integration)

<br/>

## What's New

### Recent Enhancements (v0.4.0)

#### **TrueHUD Integration** 🎯
- Poise bar displayed as special resource in TrueHUD
- Real-time poise visualization with phantom bar effect
- Visual flash feedback on poise damage (long flash on break, short flash on medium+ staggers)
- Completely optional - works with or without TrueHUD installed

#### **Animated Armoury Support** ⚔️
- Keyword-based weapon detection system
- Support for custom weapon types (Rapiers, Spears, Pikes, Halberds, Quarterstaffs, Claws, Whips, Javelins)
- Extensible via INI configuration for any mod's weapon keywords

#### **Requiem Balance Improvements** 🛡️
1. **Armor Rating Scaling** - High armor rating provides bonus poise (AR × 0.5)
2. **Stamina-Based Poise** - Low stamina reduces effective poise (minimum 50% at 0 stamina)
3. **Weapon Weight Impact** - Heavier weapons deal more poise damage
4. **Weapon Damage Scaling** - Better materials/sharpness affects poise damage
5. **Critical Hit Bonus** - Critical hits deal 2× poise damage
6. **Velocity/Momentum** - Moving/sprinting attacks deal 30% more poise damage

<br/>

## Calculating Total Poise Health

### List of Variables:

1. **BasePoiseHealth**: Global value defined in "MaxsuPoise.ini", represents the base poise health of the target (default: 40).
2. **BaseMass**: Value defined in the race form, varies for actors of different races (e.g., Human=1, Giant=10).
3. **Scale**: A standalone value for each actor, represents the body scale of the target.
4. **BaseArmorPoiseHealth**: Global value defined in "MaxsuPoise.ini", represents the extra poise health per armor piece (default: 50).
5. **HeavyArmorPoiseBonus**: Global value defined in "MaxsuPoise.ini", represents the extra poise bonus for heavy armor (default: 0.5 = +50%).
6. **ArmorRating**: ⭐ NEW - The target's total armor rating (damage resist actor value).
7. **ArmorRatingScale**: ⭐ NEW - Conversion factor for armor rating to poise (default: 0.5).
8. **CurrentStamina**: ⭐ NEW - The target's current stamina value.
9. **MaxStamina**: ⭐ NEW - The target's maximum stamina value.
10. **MinStaminaMult**: ⭐ NEW - Minimum poise multiplier when exhausted (default: 0.5 = 50%).

### Formula:

```
ActorPoiseHealth = BasePoiseHealth × BaseMass × Scale

ArmorPoiseHealth = Σ(BaseArmorPoiseHealth × SlotMultiplier × [1 + HeavyArmorBonus if heavy])

ArmorRatingBonus = ArmorRating × ArmorRatingScale

StaminaPenalty = MAX(MinStaminaMult, CurrentStamina / MaxStamina)

TotalPoiseHealth = (ActorPoiseHealth + ArmorPoiseHealth + ArmorRatingBonus) × StaminaPenalty
```

#### Core Variables:
1. **BaseMeleePoiseDamage**: Base melee poise damage value (default: 10.5)
2. **BaseRangePoiseDamage**: Base ranged poise damage value (default: 10.5)
3. **BaseMagicPoiseDamage**: Base magic poise damage value (default: 100.0)

#### Weapon Identification (Priority Order):
1. **MaxsuPoise_UniqueWeapStagger** keyword → Uses weapon's custom stagger value
2. **Weapon Keywords** ⭐ NEW → Checks `[WeaponKeywordMult]` section (Animated Armoury support)
3. **Weapon Type** → Falls back to `[WeaponTypeMult]` section (vanilla weapons)

#### Standard Variables:
4. **AnimationDamageMult**: Multiplier from attack animation annotations (currently 0.0, reserved)
5. **AttackDataStagger**: Stagger offset from attack data (power attacks, etc.)
6. **StrengthMult**: `(AttackerMass × AttackerScale) / (TargetMass × TargetScale)`
7. **ModTargetStagger**: Perk multiplier affecting target stagger (attacker's perks)
8. **ModIncomingStagger**: Perk multiplier for incoming stagger (target's perks)
9. **BlockingMult**: Blocking reduction (0 if fully blocked, 1-percentBlocked if partial)

#### New Requiem Variables:
10. **WeaponWeight** ⭐ NEW: Physical weight of the weapon in kg
11. **WeaponWeightScale** ⭐ NEW: Multiplier for weight impact (default: 0.05)
12. **WeaponDamage** ⭐ NEW: Base damage stat of the weapon
13. **WeaponDamageScale** ⭐ NEW: Multiplier for damage impact (default: 0.015)
14. **CriticalHit** ⭐ NEW: Whether the hit was a critical hit
15. **CriticalHitMult** ⭐ NEW: Critical hit multiplier (default: 2.0)
16. **VelocityMult** ⭐ NEW: Bonus for moving/sprinting (default: 1.3)

### Formulas:

#### **Physical Damage** (Melee/Ranged):

```
WeaponMaterialMult = 1.0 + (WeaponWeight × WeaponWeightScale) + (WeaponDamage × WeaponDamageScale)

PoiseDamage = BaseDamage × 
              (WeaponTypeMult + StrengthMult + AttackDataStagger) × 
              WeaponMaterialMult × 
              (1 + AnimationDamageMult) × 
              VelocityMult × 
              CriticalHitMult × 
              ModTargetStagger × 
              ModIncomingStagger

If blocked: PoiseDamage *= BlockingMult
```

**Example Calculation (Daedric Warhammer Sprint Critical):**
- Base: 10.5
- Type: 2.0 (TwoHandAxe/Warhammer)
- Strength: 1.0 (equal mass)
- Material: 1.0 + (27×0.05) + (27×0.015) = 2.755
- Velocity: 1.3 (sprinting)
- Critical: 2.0
- **Result: 10.5 × 3.0 × 2.755 × 1.3 × 2.0 = 225 poise damage!**

#### **Magic Damage**:

```
MagicPoiseDamage = BaseMagicPoiseDamage × StaggerMagnitude × ModTargetStagger × ModIncomingStagger
```

**Note:** Only magic effects with **Stagger archetype** deal poise damage!

#### **Weapon Keyword Support** ⭐ NEW:

Custom weapons (Animated Armoury, etc.) can be configured via keywords:
```ini
[WeaponKeywordMult]
WeapTypeRapier = 0.800000
WeapTypeSpear = 1.500000
WeapTypePike = 1.800000
WeapTypeHalberd = 1.800000
```

Priority: Unique Keywords > Custom Keywords > Vanilla Type > Default (0)ale) / (target BaseMass * target Scale)`

### Formula:

- **Physicial Damage**:

```
Melee/RangePoiseDamage = BaseMelee/RangePoiseDamage * (WeaponDamageMult + StrengthMult + AttackDataStagger) * (AnimationDamageMult + 1) * ModTargetStagger * ModIncomingStagger
```

If the hit is blocked, then `PoiseDamage *= BlockingMult`
<br/>

- **Magic Damage**:

```
  MagicPoiseDamage = BaseMagicPoiseDamage * MagicMagnitude
```

**Only works for magic effect with stagger archetype!**

---

<br/>

## Calculating Total Immune Level

### List of Variables:

1. **KeywordImmuneLevel**：The hit target`s immunity to different levels of stagger is set through special keywords:

   - MaxsuPoise_ImmuneSmall
   - MaxsuPoise_ImmuneMedium
   - MaxsuPoise_ImmuneLarge
   - MaxsuPoise_ImmuneLargest

They represent the four levels of stagger the hit target would be immune to. Immunity keywords can be added directly to the actor or by casting a magic effect with the corresponding keyword on the actor.
If there are multiple Immune keywords at the same time, the keyword with the highest Immune level will be used as the final value.

2. **AnimationImmuneLevel**: Immune Level that read from the current activated animation annotations of the hit target.

### Formula:

```
TotalImmuneLevel = MAX(KeywordImmuneLevel,AnimationImmuneLevel)
```

Get the maximum value between KeywordImmuneLevel and AnimationImmuneLevel.

---

<br/>

## Trigger Stagger

### List of Variables:

1. **TotalPoiseHealth**：See the above for the specific calculation method.

2. **CurrentPoiseHealth**: A graphVairbaleFloat value injected through BDI, indicating the current remaining poise health of the hit target.
3. **PoiseDamage**: See the above for the specific calculation method.
4. **SmallStaggerTHLD**: Global value that defined in "MaxsuPoise.ini", represents the threshold of the poise damage that could drive the hit target trigger small stagger.
5. **MediumStaggerTHLD**: Global value that defined in "MaxsuPoise.ini", represents the threshold of the poise damage that could drive the hit target trigger medium stagger.
6. **LargeStaggerTHLD**: Global value that defined in "MaxsuPoise.ini", represents the threshold of the poise damage that could drive the hit target trigger large stagger.
7. **TotalImmuneLevel**: See the above for the specific calculation method.

### Stagger Trigger Mechanics:

1. The inital value of the _CurrentPoiseHealth_ for a hit target is equal to it _TotalPoiseHealth_.

2. Once the hit target receive damage from the attacker within an attack:  
   `CurrentPoiseHealth -= PoiseDamage`
3. If `CurrentPoiseHealth <= 0`, largest level of stagger animation would be triggered on the hit target.
4. If `CurrentPoiseHealth > 0`, calculating the result of `PoiseDamage / TotalPoiseHealth`:
   - If `result < SmallStaggerTHLD`，Not stagger animation would be triggered.
   - If `SmallStaggerTHLD <= result < MediumStaggerTHLD` and `TotalImmuneLevel < 1`, small level of stagger animation would be triggered on the hit target.
   - If `MediumStaggerTHLD <= result < LargeStaggerTHLD` and `TotalImmuneLevel < 2`, medium level of stagger animation would be triggered on the hit target.
   - If `result >= LargeStaggerTHLD` and `TotalImmuneLevel < 3`, large level of stagger animation would be triggered on the hit target.

---

<br/>

## Poise Health Regen

### List of Variables:

1. **PoiseRegen**: Global value that defined in "MaxsuPoise.ini", represents the poise health regen ratio of the hit target.

### Implement Methods:

1. If `0 < CurrentPoiseHealth < TotalPoiseHealth`，for each second: `CurrentPoiseHealth += PoiseRegen`
2. If `CurrentPoiseHealth > TotalPoiseHealth` then `CurrentPoiseHealth = TotalPoiseHealth`
3. If `CurrentPoiseHealth <= 0`,
   - If hit stagger is staggering, Immune to any Poise Damage and stagger.
   - If hit stagger is not staggering，`currentPoiseHealth = TotalPoiseHealth`

---

<br/>

## Infinite StaggerLock Prevention

### List of Variables:

1. **StaggerProtectTime**: Global value that defined in "MaxsuPoise.ini", represents the stagger protect time after the hit target recovered from largest stagger state.

### Implement Methods:

- When `CurrentPoiseHealth <= 0` and the hit target is playing largest stagger animation，the actor would Immune to any Poise Damage and stagger.

- After the hit target recovered from largest stagger state, he will gained a stagger protect effect equivalents to `TotalImmuneLevel = 3`, the effect would lasting until StaggerProtectTime end.

---

<br/>

## TrueHUD Integration

### Overview

MaxsuPoise integrates with [TrueHUD](https://www.nexusmods.com/skyrimspecialedition/mods/62775) to display poise as a special resource bar. This is **completely optional** - MaxsuPoise works with or without TrueHUD.

### Features:

1. **Poise Bar Display**
   - Shows current poise / maximum poise in real-time
   - Updates dynamically based on armor, stamina, and other factors
   - Visible for player and all NPCs with info bars

2. **Phantom Bar Effect**
   - When poise decreases, a "phantom" bar shows the lost amount
   - Fades away gradually for visual feedback
   - Helps players understand how much poise was just lost

3. **Visual Feedback**
   - **Long Flash**: Triggered when poise breaks completely (largest stagger)
   - **Short Flash**: Triggered on medium or large staggers (without breaking)
   - **No Flash**: Small staggers don't trigger visual effects (clarity)

### Configuration:

Users can customize bar colors in TrueHUD's MCM:
- **Bar Color**: Main poise bar color
- **Phantom Color**: Lost poise visualization
- **Background Color**: Empty portion of bar
- **Flash Color**: Flash effect color

### Technical Details:

- Uses TrueHUD's Special Resource Bar API (v3/v4)
- Automatically requests control on game load
- Falls back gracefully if another mod uses the special bar
- No performance impact - leverages existing TrueHUD rendering

See [TrueHUD Integration Documentation](../TrueHUD_Integration.md) for more details.

---
