#include "WAUI/WAHUDWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Engine/World.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/PlayerState.h"
#include "WACore/WAPlayerInfo.h"
#include "WAGame/WAGameState.h"

void UWAHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UCanvasPanel* Canvas = WidgetTree->ConstructWidget<UCanvasPanel>();
	WidgetTree->RootWidget = Canvas;
	UVerticalBox* Panel = WidgetTree->ConstructWidget<UVerticalBox>();
	UCanvasPanelSlot* PanelSlot = Canvas->AddChildToCanvas(Panel);
	PanelSlot->SetPosition(FVector2D(24.0, 24.0));
	PanelSlot->SetAutoSize(true);

	auto AddLine = [this, Panel](const FText& Text)
	{
		UTextBlock* Block = WidgetTree->ConstructWidget<UTextBlock>();
		Block->SetText(Text);
		Block->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		Panel->AddChild(Block);
		return Block;
	};

	HealthText = AddLine(NSLOCTEXT("WAUI", "HealthInitial", "Health: 100 / 100"));
	ModeText = AddLine(NSLOCTEXT("WAUI", "ModeInitial", "Mode: Connecting"));
	ConnectionText = AddLine(FText::GetEmpty());
	PlayersText = AddLine(FText::GetEmpty());
	AddLine(NSLOCTEXT("WAUI", "Controls", "WASD Move | Mouse Look | Space Jump | LMB Test Attack | E Interact"));
	TryBindState();
	RefreshPlayersAndConnection();
}

void UWAHUDWidget::NativeDestruct()
{
	if (IWAPlayerInfo* PlayerInfo = Cast<IWAPlayerInfo>(BoundPlayerState.Get()))
	{
		PlayerInfo->GetHealthChangedDelegate().RemoveAll(this);
	}
	if (BoundGameState.IsValid())
	{
		BoundGameState->OnModeChanged.RemoveAll(this);
	}
	Super::NativeDestruct();
}

void UWAHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	TryBindState();
	RefreshRemaining -= InDeltaTime;
	if (RefreshRemaining <= 0.0f)
	{
		RefreshRemaining = 0.5f;
		RefreshPlayersAndConnection();
	}
}

void UWAHUDWidget::TryBindState()
{
	APlayerController* Controller = GetOwningPlayer();
	APlayerState* PlayerState = Controller ? Controller->GetPlayerState<APlayerState>() : nullptr;
	if (PlayerState && BoundPlayerState.Get() != PlayerState)
	{
		if (IWAPlayerInfo* OldInfo = Cast<IWAPlayerInfo>(BoundPlayerState.Get()))
		{
			OldInfo->GetHealthChangedDelegate().RemoveAll(this);
		}
		BoundPlayerState = PlayerState;
		if (IWAPlayerInfo* NewInfo = Cast<IWAPlayerInfo>(PlayerState))
		{
			NewInfo->GetHealthChangedDelegate().AddUObject(this, &UWAHUDWidget::UpdateHealth);
			UpdateHealth(NewInfo->GetHealth(), NewInfo->GetMaxHealth());
		}
	}

	AWAGameState* GameState = GetWorld() ? GetWorld()->GetGameState<AWAGameState>() : nullptr;
	if (GameState && BoundGameState.Get() != GameState)
	{
		if (BoundGameState.IsValid())
		{
			BoundGameState->OnModeChanged.RemoveAll(this);
		}
		BoundGameState = GameState;
		GameState->OnModeChanged.AddUObject(this, &UWAHUDWidget::UpdateMode);
		UpdateMode(GameState->GetMatchMode());
	}
}

void UWAHUDWidget::UpdateHealth(float Health, float MaxHealth)
{
	if (HealthText)
	{
		HealthText->SetText(FText::Format(NSLOCTEXT("WAUI", "HealthFormat", "Health: {0} / {1}"), FMath::RoundToInt(Health), FMath::RoundToInt(MaxHealth)));
	}
}

void UWAHUDWidget::UpdateMode(EWAGameMode Mode)
{
	if (ModeText)
	{
		ModeText->SetText(FText::Format(NSLOCTEXT("WAUI", "ModeFormat", "Mode: {0}"), Mode == EWAGameMode::PlayerVersusPlayer
			? NSLOCTEXT("WAUI", "PvP", "LAN PvP")
			: NSLOCTEXT("WAUI", "Coop", "LAN Cooperative")));
	}
}

void UWAHUDWidget::RefreshPlayersAndConnection()
{
	const UWorld* World = GetWorld();
	const AGameStateBase* GameState = World ? World->GetGameState() : nullptr;
	if (!GameState)
	{
		return;
	}

	TArray<FString> PlayerLines;
	for (const APlayerState* PlayerState : GameState->PlayerArray)
	{
		if (!PlayerState)
		{
			continue;
		}
		FString Line = PlayerState->GetPlayerName();
		if (const IWAPlayerInfo* Info = Cast<IWAPlayerInfo>(PlayerState))
		{
			Line += FString::Printf(TEXT("  HP %.0f"), Info->GetHealth());
		}
		PlayerLines.Add(Line);
	}
	PlayersText->SetText(FText::FromString(FString::Printf(TEXT("Players (%d):\n%s"), PlayerLines.Num(), *FString::Join(PlayerLines, TEXT("\n")))));

	FString Connection = TEXT("Standalone");
	if (World->GetNetMode() == NM_Client)
	{
		Connection = TEXT("LAN Client");
	}
	else if (World->GetNetMode() == NM_ListenServer)
	{
		Connection = TEXT("LAN Listen Host");
	}
	ConnectionText->SetText(FText::Format(NSLOCTEXT("WAUI", "ConnectionFormat", "Connection: {0}"), FText::FromString(Connection)));
}
