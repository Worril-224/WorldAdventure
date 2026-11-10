#include "WAUI/WAGameUISubsystem.h"

#include "Blueprint/UserWidget.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "WAUI/WAHUDWidget.h"
#include "WAUI/WAMainMenuWidget.h"

void UWAGameUISubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	GetLocalPlayer()->OnPlayerControllerChanged().AddUObject(this, &UWAGameUISubsystem::HandlePlayerControllerChanged);
}

void UWAGameUISubsystem::Deinitialize()
{
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		LocalPlayer->OnPlayerControllerChanged().RemoveAll(this);
	}
	RootWidget = nullptr;
	Super::Deinitialize();
}

void UWAGameUISubsystem::HandlePlayerControllerChanged(APlayerController* NewPlayerController)
{
	if (!NewPlayerController)
	{
		return;
	}

	if (RootWidget)
	{
		RootWidget->RemoveFromParent();
		RootWidget = nullptr;
	}

	UWorld* World = NewPlayerController->GetWorld();
	const bool bMainMenu = World && World->GetMapName().Contains(TEXT("L_MainMenu"));
	RootWidget = CreateWidget<UUserWidget>(NewPlayerController, bMainMenu ? UWAMainMenuWidget::StaticClass() : UWAHUDWidget::StaticClass());
	if (!RootWidget)
	{
		return;
	}
	RootWidget->AddToViewport();

	if (bMainMenu)
	{
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(RootWidget->TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		NewPlayerController->SetInputMode(InputMode);
		NewPlayerController->SetShowMouseCursor(true);
	}
	else
	{
		NewPlayerController->SetInputMode(FInputModeGameOnly());
		NewPlayerController->SetShowMouseCursor(false);
	}
}
