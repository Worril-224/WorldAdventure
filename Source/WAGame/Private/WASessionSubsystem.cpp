#include "WAGame/WASessionSubsystem.h"

#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "WAGame/WAGameLog.h"

namespace WA::MapPaths
{
	constexpr const TCHAR* MainMenu = TEXT("/Game/WA/Maps/L_MainMenu");
	constexpr const TCHAR* Demo = TEXT("/Game/WA/Maps/L_Demo");
}

void UWASessionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	if (GEngine)
	{
		GEngine->OnNetworkFailure().AddUObject(this, &UWASessionSubsystem::HandleNetworkFailure);
		GEngine->OnTravelFailure().AddUObject(this, &UWASessionSubsystem::HandleTravelFailure);
	}
}

void UWASessionSubsystem::Deinitialize()
{
	if (GEngine)
	{
		GEngine->OnNetworkFailure().RemoveAll(this);
		GEngine->OnTravelFailure().RemoveAll(this);
	}
	Super::Deinitialize();
}

void UWASessionSubsystem::HostGame(EWAGameMode Mode)
{
	UWorld* World = GetGameInstance()->GetWorld();
	if (!World)
	{
		OnSessionMessage.Broadcast(NSLOCTEXT("WAGame", "HostWithoutWorld", "Cannot host without a valid world."));
		return;
	}

	const TCHAR* ModeOption = Mode == EWAGameMode::PlayerVersusPlayer ? TEXT("PvP") : TEXT("Coop");
	const FString URL = FString::Printf(TEXT("%s?listen?Mode=%s"), WA::MapPaths::Demo, ModeOption);
	UE_LOG(LogWAGame, Log, TEXT("Hosting LAN session: %s"), *URL);
	World->ServerTravel(URL);
}

void UWASessionSubsystem::JoinGame(const FString& Address)
{
	const FString CleanAddress = Address.TrimStartAndEnd();
	UWorld* World = GetGameInstance()->GetWorld();
	APlayerController* Controller = World ? World->GetFirstPlayerController() : nullptr;
	if (CleanAddress.IsEmpty() || !Controller)
	{
		OnSessionMessage.Broadcast(NSLOCTEXT("WAGame", "InvalidJoinAddress", "Enter the host LAN IP address."));
		return;
	}

	UE_LOG(LogWAGame, Log, TEXT("Joining LAN session: %s"), *CleanAddress);
	Controller->ClientTravel(CleanAddress, TRAVEL_Absolute);
}

void UWASessionSubsystem::LeaveSession()
{
	UWorld* World = GetGameInstance()->GetWorld();
	if (!World)
	{
		return;
	}

	if (World->GetAuthGameMode())
	{
		World->ServerTravel(WA::MapPaths::MainMenu);
	}
	else
	{
		UGameplayStatics::OpenLevel(GetGameInstance(), FName(WA::MapPaths::MainMenu));
	}
}

void UWASessionSubsystem::HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString)
{
	UE_LOG(LogWAGame, Warning, TEXT("Network failure: %s"), *ErrorString);
	OnSessionMessage.Broadcast(FText::Format(NSLOCTEXT("WAGame", "NetworkFailure", "Network failure: {0}"), FText::FromString(ErrorString)));
}

void UWASessionSubsystem::HandleTravelFailure(UWorld* World, ETravelFailure::Type FailureType, const FString& ErrorString)
{
	UE_LOG(LogWAGame, Warning, TEXT("Travel failure: %s"), *ErrorString);
	OnSessionMessage.Broadcast(FText::Format(NSLOCTEXT("WAGame", "TravelFailure", "Connection failed: {0}"), FText::FromString(ErrorString)));
}
