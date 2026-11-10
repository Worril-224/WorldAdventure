#include "WAPlayer/WAPlayerState.h"

#include "WACore/WAGameplayTags.h"
#include "WAAbilities/WAAbilitySystemComponent.h"
#include "WAAbilities/WAAttackAbility.h"
#include "WAAbilities/WAHealthAttributeSet.h"

AWAPlayerState::AWAPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UWAAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	HealthAttributes = CreateDefaultSubobject<UWAHealthAttributeSet>(TEXT("HealthAttributes"));
	SetNetUpdateFrequency(20.0f);
}

void AWAPlayerState::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority() && !bAbilitiesGranted)
	{
		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(UWAAttackAbility::StaticClass(), 1, 0, this));
		bAbilitiesGranted = true;
	}
}

UAbilitySystemComponent* AWAPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

float AWAPlayerState::GetHealth() const
{
	return HealthAttributes ? HealthAttributes->GetHealth() : 0.0f;
}

float AWAPlayerState::GetMaxHealth() const
{
	return HealthAttributes ? HealthAttributes->GetMaxHealth() : 1.0f;
}

FWAHealthChanged& AWAPlayerState::GetHealthChangedDelegate()
{
	return HealthAttributes->OnHealthChanged;
}

void AWAPlayerState::ResetForRespawn()
{
	if (!HasAuthority())
	{
		return;
	}

	AbilitySystemComponent->RemoveLooseGameplayTag(TAG_WA_State_Dead, 1, EGameplayTagReplicationState::TagOnly);
	AbilitySystemComponent->SetNumericAttributeBase(UWAHealthAttributeSet::GetHealthAttribute(), HealthAttributes->GetMaxHealth());
	HealthAttributes->OnHealthChanged.Broadcast(GetHealth(), GetMaxHealth());
}
