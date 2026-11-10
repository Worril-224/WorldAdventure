#include "WAUI/WAGameUISubsystem.h"

#include "Blueprint/UserWidget.h"
#include "Containers/Ticker.h"
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
