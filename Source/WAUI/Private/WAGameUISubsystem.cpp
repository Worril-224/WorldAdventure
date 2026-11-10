#include "WAUI/WAGameUISubsystem.h"

#include "Blueprint/UserWidget.h"
#include "Containers/Ticker.h"
#include "Engine/GameInstance.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "WACore/WAInputRouter.h"
#include "WAUI/WAHUDWidget.h"
#include "WAUI/WAMainMenuWidget.h"
#include "WAUI/WAPauseMenuWidget.h"

void UWAGameUISubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	GetLocalPlayer()->OnPlayerControllerChanged().AddUObject(this, &UWAGameUISubsystem::HandlePlayerControllerChanged);
	if (UGameInstance* GameInstance = GetLocalPlayer()->GetGameInstance())
	{
		GameInstance->GetSubsystem<UWAInputRouter>()->OnGameMenuRequested.AddUObject(this, &UWAGameUISubsystem::ToggleGameMenu);
	}
}

void UWAGameUISubsystem::Deinitialize()
{
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		LocalPlayer->OnPlayerControllerChanged().RemoveAll(this);
		if (UGameInstance* GameInstance = LocalPlayer->GetGameInstance())
		{
			GameInstance->GetSubsystem<UWAInputRouter>()->OnGameMenuRequested.RemoveAll(this);
		}
	}
	RootWidget = nullptr;
	GameMenuWidget = nullptr;
	Super::Deinitialize();
}

void UWAGameUISubsystem::ToggleGameMenu()
{
	UWorld* World = GetLocalPlayer()->GetWorld();
	APlayerController* PlayerController = World ? World->GetFirstPlayerController() : nullptr;
	if (!PlayerController || !World || World->GetMapName().Contains(TEXT("L_MainMenu")))
	{
		return;
	}

	if (!GameMenuWidget)
	{
		GameMenuWidget = CreateWidget<UWAPauseMenuWidget>(PlayerController);
	}
	if (!GameMenuWidget)
	{
		return;
	}

	bGameMenuVisible = !bGameMenuVisible;
	if (bGameMenuVisible)
	{
		GameMenuWidget->SetIsFocusable(true);
		GameMenuWidget->AddToViewport(100);
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(GameMenuWidget->TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PlayerController->SetInputMode(InputMode);
		PlayerController->SetShowMouseCursor(true);
	}
	else
	{
		GameMenuWidget->RemoveFromParent();
		PlayerController->SetInputMode(FInputModeGameOnly());
		PlayerController->SetShowMouseCursor(false);
	}
}

void UWAGameUISubsystem::HandlePlayerControllerChanged(APlayerController* NewPlayerController)
{
	if (!NewPlayerController)
	{
		return;
	}

	bGameMenuVisible = false;
	GameMenuWidget = nullptr;
	TWeakObjectPtr<APlayerController> Controller = NewPlayerController;
	FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateWeakLambda(this, [this, Controller](float)
	{
		if (!Controller.IsValid())
		{
			return false;
		}

		APlayerController* PlayerController = Controller.Get();
		if (RootWidget)
		{
			RootWidget->RemoveFromParent();
			RootWidget = nullptr;
		}

		UWorld* World = PlayerController->GetWorld();
		const bool bMainMenu = World && World->GetMapName().Contains(TEXT("L_MainMenu"));
		RootWidget = CreateWidget<UUserWidget>(PlayerController, bMainMenu ? UWAMainMenuWidget::StaticClass() : UWAHUDWidget::StaticClass());
		if (!RootWidget)
		{
			return false;
		}
		RootWidget->AddToViewport();

		if (bMainMenu)
		{
			RootWidget->SetIsFocusable(true);
			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(RootWidget->TakeWidget());
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerController->SetInputMode(InputMode);
			PlayerController->SetShowMouseCursor(true);
		}
		else
		{
			PlayerController->SetInputMode(FInputModeGameOnly());
			PlayerController->SetShowMouseCursor(false);
		}
		return false;
	}), 0.1f);
}
