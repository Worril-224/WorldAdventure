#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "WACore/WAGameRules.h"
#include "WACore/WATypes.h"
#include "WAGameMode.generated.h"

UCLASS()
class WAGAME_API AWAGameMode : public AGameModeBase, public IWAGameRules
{
	GENERATED_BODY()

public:
	AWAGameMode();
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void InitGameState() override;
	virtual bool AllowsPlayerDamage(const AActor* Source, const AActor* Target) const override;
	virtual void HandlePlayerDeath(AActor* Avatar) override;

private:
	EWAGameMode MatchMode = EWAGameMode::Cooperative;
	void RespawnPlayer(TWeakObjectPtr<AController> Controller);
};
