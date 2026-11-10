#include "WAUI/WAMainMenuWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Engine/GameInstance.h"
#include "Kismet/KismetSystemLibrary.h"
#include "WAGame/WASessionSubsystem.h"
#include "WAUI/WAUILog.h"

namespace
{
	UTextBlock* AddText(UWidgetTree* Tree, UPanelWidget* Parent, const FText& Text)
	{
		UTextBlock* Block = Tree->ConstructWidget<UTextBlock>();
		Block->SetText(Text);
		Parent->AddChild(Block);
		return Block;
	}

	UButton* AddButton(UWidgetTree* Tree, UPanelWidget* Parent, const FText& Text)
	{
		UButton* Button = Tree->ConstructWidget<UButton>();
		UTextBlock* Label = Tree->ConstructWidget<UTextBlock>();
		Label->SetText(Text);
		Button->AddChild(Label);
		Parent->AddChild(Button);
		return Button;
	}
}

void UWAMainMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	UCanvasPanel* Canvas = WidgetTree->ConstructWidget<UCanvasPanel>();
	WidgetTree->RootWidget = Canvas;
	UBorder* Background = WidgetTree->ConstructWidget<UBorder>();
	Background->SetBrushColor(FLinearColor(0.75f, 0.82f, 0.9f));
	UCanvasPanelSlot* BackgroundSlot = Canvas->AddChildToCanvas(Background);
	BackgroundSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
	BackgroundSlot->SetOffsets(FMargin(0.0f));
	UVerticalBox* Menu = WidgetTree->ConstructWidget<UVerticalBox>();
	UCanvasPanelSlot* MenuSlot = Canvas->AddChildToCanvas(Menu);
	MenuSlot->SetAnchors(FAnchors(0.5f));
	MenuSlot->SetAlignment(FVector2D(0.5f));
	MenuSlot->SetAutoSize(true);

	AddText(WidgetTree, Menu, NSLOCTEXT("WAUI", "Title", "WORLD ADVENTURE - FRAMEWORK DEMO"));
	UButton* CoopButton = AddButton(WidgetTree, Menu, NSLOCTEXT("WAUI", "HostCoop", "Host LAN Cooperative"));
	UButton* PvPButton = AddButton(WidgetTree, Menu, NSLOCTEXT("WAUI", "HostPvP", "Host LAN PvP"));
	AddressInput = WidgetTree->ConstructWidget<UEditableTextBox>();
	AddressInput->SetText(FText::FromString(TEXT("127.0.0.1")));
	AddressInput->SetHintText(NSLOCTEXT("WAUI", "AddressHint", "Host LAN IP, for example 192.168.1.100"));
	Menu->AddChild(AddressInput);
	UButton* JoinButton = AddButton(WidgetTree, Menu, NSLOCTEXT("WAUI", "Join", "Join by IP"));
	UButton* QuitButton = AddButton(WidgetTree, Menu, NSLOCTEXT("WAUI", "Quit", "Quit"));
	MessageText = AddText(WidgetTree, Menu, FText::GetEmpty());

	CoopButton->OnClicked.AddDynamic(this, &UWAMainMenuWidget::HostCoop);
	PvPButton->OnClicked.AddDynamic(this, &UWAMainMenuWidget::HostPvP);
	JoinButton->OnClicked.AddDynamic(this, &UWAMainMenuWidget::Join);
	QuitButton->OnClicked.AddDynamic(this, &UWAMainMenuWidget::Quit);

	if (UWASessionSubsystem* Session = GetGameInstance()->GetSubsystem<UWASessionSubsystem>())
	{
		Session->OnSessionMessage.AddUObject(this, &UWAMainMenuWidget::SetMessage);
	}
}

void UWAMainMenuWidget::NativeDestruct()
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		if (UWASessionSubsystem* Session = GameInstance->GetSubsystem<UWASessionSubsystem>())
		{
			Session->OnSessionMessage.RemoveAll(this);
		}
	}
	Super::NativeDestruct();
}

void UWAMainMenuWidget::HostCoop()
{
	UE_LOG(LogWAUI, Log, TEXT("Host Cooperative clicked"));
	GetGameInstance()->GetSubsystem<UWASessionSubsystem>()->HostGame(EWAGameMode::Cooperative);
}

void UWAMainMenuWidget::HostPvP()
{
	GetGameInstance()->GetSubsystem<UWASessionSubsystem>()->HostGame(EWAGameMode::PlayerVersusPlayer);
}

void UWAMainMenuWidget::Join()
{
	GetGameInstance()->GetSubsystem<UWASessionSubsystem>()->JoinGame(AddressInput->GetText().ToString());
}

void UWAMainMenuWidget::Quit()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}

void UWAMainMenuWidget::SetMessage(const FText& Message)
{
	if (MessageText)
	{
		MessageText->SetText(Message);
	}
}
