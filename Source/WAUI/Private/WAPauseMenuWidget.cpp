#include "WAUI/WAPauseMenuWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Engine/GameInstance.h"
#include "Kismet/KismetSystemLibrary.h"
#include "WAGame/WASessionSubsystem.h"
#include "WAUI/WAGameUISubsystem.h"

void UWAPauseMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	UCanvasPanel* Canvas = WidgetTree->ConstructWidget<UCanvasPanel>();
	WidgetTree->RootWidget = Canvas;
	UBorder* Background = WidgetTree->ConstructWidget<UBorder>();
	Background->SetBrushColor(FLinearColor(0.02f, 0.03f, 0.05f, 0.9f));
	UCanvasPanelSlot* BackgroundSlot = Canvas->AddChildToCanvas(Background);
	BackgroundSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
	BackgroundSlot->SetOffsets(FMargin(0.0f));

	UVerticalBox* Menu = WidgetTree->ConstructWidget<UVerticalBox>();
	UCanvasPanelSlot* MenuSlot = Canvas->AddChildToCanvas(Menu);
	MenuSlot->SetAnchors(FAnchors(0.5f));
	MenuSlot->SetAlignment(FVector2D(0.5f));
	MenuSlot->SetAutoSize(true);

	UTextBlock* Title = WidgetTree->ConstructWidget<UTextBlock>();
	Title->SetText(NSLOCTEXT("WAUI", "GameMenuTitle", "GAME MENU"));
	Menu->AddChild(Title);

	auto AddButton = [this, Menu](const FText& Text)
	{
		UButton* Button = WidgetTree->ConstructWidget<UButton>();
		UTextBlock* Label = WidgetTree->ConstructWidget<UTextBlock>();
		Label->SetText(Text);
		Button->AddChild(Label);
		Menu->AddChild(Button);
		return Button;
	};

	UButton* ResumeButton = AddButton(NSLOCTEXT("WAUI", "Resume", "Resume"));
	UButton* MainMenuButton = AddButton(NSLOCTEXT("WAUI", "ReturnMainMenu", "Return to Main Menu"));
	UButton* QuitButton = AddButton(NSLOCTEXT("WAUI", "Quit", "Quit Game"));
	ResumeButton->OnClicked.AddDynamic(this, &UWAPauseMenuWidget::Resume);
	MainMenuButton->OnClicked.AddDynamic(this, &UWAPauseMenuWidget::ReturnToMainMenu);
	QuitButton->OnClicked.AddDynamic(this, &UWAPauseMenuWidget::Quit);
}

void UWAPauseMenuWidget::Resume()
{
	GetOwningLocalPlayer()->GetSubsystem<UWAGameUISubsystem>()->ToggleGameMenu();
}

void UWAPauseMenuWidget::ReturnToMainMenu()
{
	GetGameInstance()->GetSubsystem<UWASessionSubsystem>()->LeaveSession();
}

void UWAPauseMenuWidget::Quit()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, false);
}
