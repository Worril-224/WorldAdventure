#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "WAAttackAbility.generated.h"

UCLASS()
class WAABILITIES_API UWAAttackAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UWAAttackAbility();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	float LastServerAttackTime = -100.0f;
};
