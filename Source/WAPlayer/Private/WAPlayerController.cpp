#include "WAPlayer/WAPlayerController.h"

#include "Components/InputComponent.h"
#include "Engine/GameInstance.h"
#include "InputCoreTypes.h"
#include "WACore/WAInputRouter.h"

void AWAPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (InputComponent && IsLocalPlayerController())
	{
		InputComponent->BindKey(EKeys::Escape, IE_Pressed, this, &AWAPlayerController::RequestGameMenu);
	}
}

void AWAPlayerController::RequestGameMenu()
{
	GetGameInstance()->GetSubsystem<UWAInputRouter>()->RequestGameMenu();
}
