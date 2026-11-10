#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WAGameRules.generated.h"

UINTERFACE(MinimalAPI, NotBlueprintable)
class UWAGameRules : public UInterface
{
	GENERATED_BODY()
};

class WACORE_API IWAGameRules
{
	GENERATED_IINTERFACE_BODY()

public:
	virtual bool AllowsPlayerDamage(const AActor* Source, const AActor* Target) const = 0;
	virtual void HandlePlayerDeath(AActor* Avatar) = 0;
};
