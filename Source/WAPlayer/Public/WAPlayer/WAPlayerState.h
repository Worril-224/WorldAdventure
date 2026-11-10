#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/PlayerState.h"
#include "WACore/WAPlayerInfo.h"
#include "WAPlayerState.generated.h"

class UWAAbilitySystemComponent;
class UWAHealthAttributeSet;

UCLASS()
class WAPLAYER_API AWAPlayerState : public APlayerState, public IAbilitySystemInterface, public IWAPlayerInfo
{
	GENERATED_BODY()

public:
	AWAPlayerState();
	virtual void BeginPlay() override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual float GetHealth() const override;
	virtual float GetMaxHealth() const override;
	virtual FWAHealthChanged& GetHealthChangedDelegate() override;

	void ResetForRespawn();

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UWAAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UWAHealthAttributeSet> HealthAttributes;

	bool bAbilitiesGranted = false;
};
