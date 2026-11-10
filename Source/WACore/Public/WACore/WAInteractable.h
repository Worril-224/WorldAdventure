#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WAInteractable.generated.h"

UINTERFACE(MinimalAPI, NotBlueprintable)
class UWAInteractable : public UInterface
{
	GENERATED_BODY()
};

class WACORE_API IWAInteractable
{
	GENERATED_IINTERFACE_BODY()

public:
	virtual FText GetInteractionPrompt() const = 0;
	virtual void Interact(AActor* Interactor) = 0;
};
