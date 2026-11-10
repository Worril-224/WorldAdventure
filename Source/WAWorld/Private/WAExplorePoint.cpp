#include "WAWorld/WAExplorePoint.h"

#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Net/UnrealNetwork.h"
#include "UObject/ConstructorHelpers.h"
#include "WAWorld/WAWorldLog.h"

AWAExplorePoint::AWAExplorePoint()
{
	bReplicates = true;
	SetReplicateMovement(false);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
	Mesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	Mesh->SetRelativeScale3D(FVector(0.75, 0.75, 1.5));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (CylinderMesh.Succeeded())
	{
		Mesh->SetStaticMesh(CylinderMesh.Object);
	}
}

void AWAExplorePoint::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWAExplorePoint, bActivated);
}

void AWAExplorePoint::BeginPlay()
{
	Super::BeginPlay();
	UpdateVisual();
}

FText AWAExplorePoint::GetInteractionPrompt() const
{
	return bActivated
		? NSLOCTEXT("WAWorld", "ExplorePointActivated", "Exploration point activated")
		: NSLOCTEXT("WAWorld", "ExplorePointPrompt", "E - Activate exploration point");
}

void AWAExplorePoint::Interact(AActor* Interactor)
{
	if (!HasAuthority() || bActivated || !Interactor)
	{
		return;
	}

	if (FVector::DistSquared(GetActorLocation(), Interactor->GetActorLocation()) > FMath::Square(350.0f))
	{
		return;
	}

	bActivated = true;
	UpdateVisual();
	ForceNetUpdate();
	UE_LOG(LogWAWorld, Log, TEXT("Exploration point activated by %s"), *Interactor->GetName());
}

void AWAExplorePoint::OnRep_Activated()
{
	UpdateVisual();
}

void AWAExplorePoint::UpdateVisual()
{
	if (!Mesh)
	{
		return;
	}

	UMaterialInstanceDynamic* Material = Mesh->CreateAndSetMaterialInstanceDynamic(0);
	if (Material)
	{
		Material->SetVectorParameterValue(TEXT("Color"), bActivated ? FLinearColor::Green : FLinearColor(0.1f, 0.4f, 1.0f));
	}
}
