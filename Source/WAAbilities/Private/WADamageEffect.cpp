#include "WAAbilities/WADamageEffect.h"

#include "WAAbilities/WAHealthAttributeSet.h"

UWADamageEffect::UWADamageEffect()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FGameplayModifierInfo HealthModifier;
	HealthModifier.Attribute = UWAHealthAttributeSet::GetHealthAttribute();
	HealthModifier.ModifierOp = EGameplayModOp::Additive;
	HealthModifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(-25.0f));
	Modifiers.Add(HealthModifier);
}
