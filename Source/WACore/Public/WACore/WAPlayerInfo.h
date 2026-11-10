#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WAPlayerInfo.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FWAHealthChanged, float, float);

UINTERFACE(MinimalAPI, NotBlueprintable)
class UWAPlayerInfo : public UInterface
{
	GENERATED_BODY()
};

class WACORE_API IWAPlayerInfo
{
	GENERATED_IINTERFACE_BODY()

public:
	virtual float GetHealth() const = 0;
	virtual float GetMaxHealth() const = 0;
	virtual FWAHealthChanged& GetHealthChangedDelegate() = 0;
};
