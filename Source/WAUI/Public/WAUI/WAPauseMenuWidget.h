#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WAPauseMenuWidget.generated.h"

UCLASS()
class WAUI_API UWAPauseMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

private:
	UFUNCTION()
	void Resume();

	UFUNCTION()
	void ReturnToMainMenu();

	UFUNCTION()
	void Quit();
};
