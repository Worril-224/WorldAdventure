#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WACore/WAInteractable.h"
#include "WAExplorePoint.generated.h"

class UStaticMeshComponent;

UCLASS()
class WAWORLD_API AWAExplorePoint : public AActor, public IWAInteractable
{
	GENERATED_BODY()

public:
	AWAExplorePoint();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	virtual FText GetInteractionPrompt() const override;
	virtual void Interact(AActor* Interactor) override;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(ReplicatedUsing = OnRep_Activated)
	bool bActivated = false;

	UFUNCTION()
	void OnRep_Activated();

	void UpdateVisual();
};
