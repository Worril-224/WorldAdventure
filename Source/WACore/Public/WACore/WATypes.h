#pragma once

#include "CoreMinimal.h"
#include "WATypes.generated.h"

UENUM(BlueprintType, meta = (ScriptName = "WAMatchMode"))
enum class EWAGameMode : uint8
{
	Cooperative UMETA(DisplayName = "Cooperative"),
	PlayerVersusPlayer UMETA(DisplayName = "Player versus Player")
};
