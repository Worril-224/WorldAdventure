#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "WACore/WATypes.h"
#include "WAGameState.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FWAModeChanged, EWAGameMode);

UCLASS()
class WAGAME_API AWAGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	EWAGameMode GetMatchMode() const { return MatchMode; }
	void SetMatchMode(EWAGameMode NewMode);
	FText GetModeText() const;

	FWAModeChanged OnModeChanged;

private:
	UPROPERTY(ReplicatedUsing = OnRep_MatchMode)
	EWAGameMode MatchMode = EWAGameMode::Cooperative;

	UFUNCTION()
	void OnRep_MatchMode();
};
