#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WAMainMenuWidget.generated.h"

class UEditableTextBox;
class UTextBlock;

UCLASS()
class WAUI_API UWAMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeDestruct() override;

private:
	UPROPERTY(Transient)
	TObjectPtr<UEditableTextBox> AddressInput;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> MessageText;

	UFUNCTION()
	void HostCoop();

	UFUNCTION()
	void HostPvP();

	UFUNCTION()
	void Join();

	UFUNCTION()
	void Quit();
	void SetMessage(const FText& Message);
};
