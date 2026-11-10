#include "WAAbilities/WAAttackAbility.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/GameModeBase.h"
#include "WACore/WAGameRules.h"
#include "WACore/WAGameplayTags.h"
#include "WAAbilities/WAAbilityAvatar.h"
#include "WAAbilities/WADamageEffect.h"
#include "WAAbilities/WAHealthAttributeSet.h"

UWAAttackAbility::UWAAttackAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	SetAssetTags(FGameplayTagContainer(TAG_WA_Ability_TestAttack));
	ActivationBlockedTags.AddTag(TAG_WA_State_Dead);
}

void UWAAttackAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!ActorInfo || !ActorInfo->IsNetAuthority())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	AActor* Avatar = GetAvatarActorFromActorInfo();
	UWorld* World = Avatar ? Avatar->GetWorld() : nullptr;
	if (!World)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	const float CurrentTime = World->GetTimeSeconds();
	if (CurrentTime - LastServerAttackTime < 0.5f)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	LastServerAttackTime = CurrentTime;

	const FVector Start = Avatar->GetActorLocation() + FVector(0.0f, 0.0f, 40.0f);
	const FVector End = Start + Avatar->GetActorForwardVector() * 300.0f;
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(WATestAttack), false, Avatar);
	FHitResult Hit;
	const bool bHit = World->SweepSingleByChannel(Hit, Start, End, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(40.0f), QueryParams);
	const FVector FeedbackEnd = bHit ? Hit.ImpactPoint : End;

	if (IWAAbilityAvatar* AbilityAvatar = Cast<IWAAbilityAvatar>(Avatar))
	{
		AbilityAvatar->PlayTestAttackFeedback(FeedbackEnd);
	}

	AActor* Target = bHit ? Hit.GetActor() : nullptr;
	IWAGameRules* Rules = Cast<IWAGameRules>(World->GetAuthGameMode());
	if (!Target || Target == Avatar || !Rules || !Rules->AllowsPlayerDamage(Avatar, Target))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	UAbilitySystemComponent* TargetASC = Cast<IAbilitySystemInterface>(Target) ? Cast<IAbilitySystemInterface>(Target)->GetAbilitySystemComponent() : nullptr;
	if (!SourceASC || !TargetASC || TargetASC->HasMatchingGameplayTag(TAG_WA_State_Dead))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	const FGameplayEffectSpecHandle DamageSpec = SourceASC->MakeOutgoingSpec(UWADamageEffect::StaticClass(), 1.0f, SourceASC->MakeEffectContext());
	if (DamageSpec.IsValid())
	{
		TargetASC->ApplyGameplayEffectSpecToSelf(*DamageSpec.Data.Get());
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
