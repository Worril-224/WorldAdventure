#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "WAInputRouter.generated.h"

DECLARE_MULTICAST_DELEGATE(FWAGameMenuRequested);

UCLASS()
class WACORE_API UWAInputRouter : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	void RequestGameMenu() { OnGameMenuRequested.Broadcast(); }
	FWAGameMenuRequested OnGameMenuRequested;
};
