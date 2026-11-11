#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WACore/WATypes.h"
#include "WAHUDWidget.generated.h"

class AWAGameState;
class APlayerState;
class UTextBlock;

UCLASS()
class WAUI_API UWAHUDWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> HealthText;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> ModeText;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> PlayersText;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> ConnectionText;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> SessionMessageText;

	TWeakObjectPtr<APlayerState> BoundPlayerState;
	TWeakObjectPtr<AWAGameState> BoundGameState;
	float RefreshRemaining = 0.0f;

	void TryBindState();
	void UpdateHealth(float Health, float MaxHealth);
	void UpdateMode(EWAGameMode Mode);
	void SetSessionMessage(const FText& Message);
	void RefreshPlayersAndConnection();
};
