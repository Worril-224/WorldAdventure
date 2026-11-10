#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "WAGameUISubsystem.generated.h"

class UUserWidget;

UCLASS()
class WAUI_API UWAGameUISubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	void ToggleGameMenu();

private:
	UPROPERTY(Transient)
	TObjectPtr<UUserWidget> RootWidget;

	UPROPERTY(Transient)
	TObjectPtr<UUserWidget> GameMenuWidget;

	bool bGameMenuVisible = false;

	void HandlePlayerControllerChanged(APlayerController* NewPlayerController);
};
