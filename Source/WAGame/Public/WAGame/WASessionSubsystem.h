#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "WACore/WATypes.h"
#include "WASessionSubsystem.generated.h"

class UNetDriver;

DECLARE_MULTICAST_DELEGATE_OneParam(FWASessionMessage, const FText&);

UCLASS()
class WAGAME_API UWASessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void HostGame(EWAGameMode Mode);
	void JoinGame(const FString& Address);
	void LeaveSession();

	FWASessionMessage OnSessionMessage;

private:
	void HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString);
	void HandleTravelFailure(UWorld* World, ETravelFailure::Type FailureType, const FString& ErrorString);
};
