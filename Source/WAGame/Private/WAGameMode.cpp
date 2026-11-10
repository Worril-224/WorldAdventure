#include "WAGame/WAGameMode.h"

#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "WAGame/WAGameLog.h"
#include "WAGame/WAGameState.h"
#include "WAPlayer/WACharacter.h"
#include "WAPlayer/WAPlayerController.h"
#include "WAPlayer/WAPlayerState.h"

AWAGameMode::AWAGameMode()
{
	DefaultPawnClass = AWACharacter::StaticClass();
	PlayerControllerClass = AWAPlayerController::StaticClass();
	PlayerStateClass = AWAPlayerState::StaticClass();
	GameStateClass = AWAGameState::StaticClass();
	HUDClass = nullptr;
}

void AWAGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	const FString ModeOption = UGameplayStatics::GetOption(Options, TEXT("Mode"));
	MatchMode = ModeOption.Equals(TEXT("PvP"), ESearchCase::IgnoreCase)
		? EWAGameMode::PlayerVersusPlayer
		: EWAGameMode::Cooperative;
	UE_LOG(LogWAGame, Log, TEXT("Server mode: %s"), ModeOption.IsEmpty() ? TEXT("Coop") : *ModeOption);
}

void AWAGameMode::InitGameState()
{
	Super::InitGameState();
	if (AWAGameState* WAGameState = GetGameState<AWAGameState>())
	{
		WAGameState->SetMatchMode(MatchMode);
	}
}

bool AWAGameMode::AllowsPlayerDamage(const AActor* Source, const AActor* Target) const
{
	if (MatchMode != EWAGameMode::PlayerVersusPlayer || !Source || Source == Target)
	{
		return false;
	}

	const APawn* SourcePawn = Cast<APawn>(Source);
	const APawn* TargetPawn = Cast<APawn>(Target);
	return SourcePawn && TargetPawn && SourcePawn->GetPlayerState<AWAPlayerState>() && TargetPawn->GetPlayerState<AWAPlayerState>();
}

void AWAGameMode::HandlePlayerDeath(AActor* Avatar)
{
	const APawn* Pawn = Cast<APawn>(Avatar);
	AController* Controller = Pawn ? Pawn->GetController() : nullptr;
	if (!HasAuthority() || !Controller)
	{
		return;
	}

	UE_LOG(LogWAGame, Log, TEXT("Player died; scheduling respawn: %s"), *Controller->GetName());
	FTimerHandle RespawnTimer;
	FTimerDelegate RespawnDelegate = FTimerDelegate::CreateUObject(this, &AWAGameMode::RespawnPlayer, TWeakObjectPtr<AController>(Controller));
	GetWorldTimerManager().SetTimer(RespawnTimer, RespawnDelegate, 2.0f, false);
}

void AWAGameMode::RespawnPlayer(TWeakObjectPtr<AController> Controller)
{
	if (!Controller.IsValid())
	{
		return;
	}

	AController* PlayerController = Controller.Get();
	APawn* OldPawn = PlayerController->GetPawn();
	if (AWAPlayerState* PlayerState = PlayerController->GetPlayerState<AWAPlayerState>())
	{
		PlayerState->ResetForRespawn();
	}

	PlayerController->UnPossess();
	if (OldPawn)
	{
		OldPawn->Destroy();
	}
	RestartPlayer(PlayerController);
}
