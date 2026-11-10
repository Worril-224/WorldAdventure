#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "WAPlayerController.generated.h"

UCLASS()
class WAPLAYER_API AWAPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void SetupInputComponent() override;

private:
	void RequestGameMenu();
};
