#include "WAGame/WAMainMenuGameMode.h"

#include "WAPlayer/WAPlayerController.h"

AWAMainMenuGameMode::AWAMainMenuGameMode()
{
	DefaultPawnClass = nullptr;
	PlayerControllerClass = AWAPlayerController::StaticClass();
	bStartPlayersAsSpectators = true;
}
