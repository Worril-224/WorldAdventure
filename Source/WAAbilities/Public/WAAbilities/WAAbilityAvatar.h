#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WAAbilityAvatar.generated.h"

UINTERFACE(MinimalAPI, NotBlueprintable)
class UWAAbilityAvatar : public UInterface
{
	GENERATED_BODY()
};

class WAABILITIES_API IWAAbilityAvatar
{
	GENERATED_IINTERFACE_BODY()

public:
	virtual void PlayTestAttackFeedback(const FVector& End) = 0;
	virtual void HandleDeath() = 0;
};
