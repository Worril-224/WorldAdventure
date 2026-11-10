#include "WAAbilities/WAHealthAttributeSet.h"

#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"
#include "GameFramework/GameModeBase.h"
#include "Net/UnrealNetwork.h"
#include "WACore/WAGameRules.h"
#include "WACore/WAGameplayTags.h"
#include "WAAbilities/WAAbilityAvatar.h"

UWAHealthAttributeSet::UWAHealthAttributeSet()
{
	InitHealth(100.0f);
	InitMaxHealth(100.0f);
}

void UWAHealthAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(UWAHealthAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UWAHealthAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
}

void UWAHealthAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	else if (Attribute == GetMaxHealthAttribute())
	{
		NewValue = FMath::Max(1.0f, NewValue);
	}
}

void UWAHealthAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute != GetHealthAttribute())
	{
		return;
	}

	const float OldHealth = GetHealth();
	const float NewHealth = FMath::Clamp(OldHealth, 0.0f, GetMaxHealth());
	SetHealth(NewHealth);
	OnHealthChanged.Broadcast(NewHealth, GetMaxHealth());

	UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
	if (!ASC || NewHealth > 0.0f || ASC->HasMatchingGameplayTag(TAG_WA_State_Dead))
	{
		return;
	}

	ASC->AddLooseGameplayTag(TAG_WA_State_Dead, 1, EGameplayTagReplicationState::TagOnly);
	AActor* Avatar = ASC->GetAvatarActor();
	if (IWAAbilityAvatar* AbilityAvatar = Cast<IWAAbilityAvatar>(Avatar))
	{
		AbilityAvatar->HandleDeath();
	}

	if (UWorld* World = ASC->GetWorld())
	{
		if (IWAGameRules* Rules = Cast<IWAGameRules>(World->GetAuthGameMode()))
		{
			Rules->HandlePlayerDeath(Avatar);
		}
	}
}

void UWAHealthAttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UWAHealthAttributeSet, Health, OldValue);
	OnHealthChanged.Broadcast(GetHealth(), GetMaxHealth());
}

void UWAHealthAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UWAHealthAttributeSet, MaxHealth, OldValue);
	OnHealthChanged.Broadcast(GetHealth(), GetMaxHealth());
}
