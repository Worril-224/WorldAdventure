#include "WAGame/WAGameState.h"

#include "Net/UnrealNetwork.h"

void AWAGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWAGameState, MatchMode);
}

void AWAGameState::SetMatchMode(EWAGameMode NewMode)
{
	if (!HasAuthority())
	{
		return;
	}

	MatchMode = NewMode;
	OnModeChanged.Broadcast(MatchMode);
	ForceNetUpdate();
}

FText AWAGameState::GetModeText() const
{
	return MatchMode == EWAGameMode::PlayerVersusPlayer
		? NSLOCTEXT("WAGame", "PvPMode", "LAN PvP")
		: NSLOCTEXT("WAGame", "CoopMode", "LAN Cooperative");
}

void AWAGameState::OnRep_MatchMode()
{
	OnModeChanged.Broadcast(MatchMode);
}
