#include "WAPlayer/WACharacter.h"

#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "Materials/MaterialInterface.h"
#include "Net/UnrealNetwork.h"
#include "UObject/ConstructorHelpers.h"
#include "WACore/WAInteractable.h"
#include "WAAbilities/WAAttackAbility.h"
#include "WAPlayer/WAPlayerState.h"

AWACharacter::AWACharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	GetCharacterMovement()->JumpZVelocity = 500.0f;
	GetCharacterMovement()->AirControl = 0.35f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 450.0f;
	CameraBoom->SocketOffset = FVector(0.0f, 0.0f, 70.0f);
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	AdventurerVisual = CreateDefaultSubobject<USceneComponent>(TEXT("AdventurerVisual"));
	AdventurerVisual->SetupAttachment(RootComponent);

	const auto AddPivot = [this](const FName Name, const FVector& Location)
	{
		USceneComponent* Pivot = CreateDefaultSubobject<USceneComponent>(Name);
		Pivot->SetupAttachment(AdventurerVisual);
		Pivot->SetRelativeLocation(Location);
		return Pivot;
	};

	LeftArmPivot = AddPivot(TEXT("LeftArmPivot"), FVector(0.0, -30.0, 47.0));
	RightArmPivot = AddPivot(TEXT("RightArmPivot"), FVector(0.0, 30.0, 47.0));
	LeftLegPivot = AddPivot(TEXT("LeftLegPivot"), FVector(0.0, -12.0, -12.0));
	RightLegPivot = AddPivot(TEXT("RightLegPivot"), FVector(0.0, 12.0, -12.0));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> TunicMaterial(TEXT("/Game/WA/Characters/Prototype/M_Prototype_Tunic.M_Prototype_Tunic"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> PantsMaterial(TEXT("/Game/WA/Characters/Prototype/M_Prototype_Pants.M_Prototype_Pants"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> LeatherMaterial(TEXT("/Game/WA/Characters/Prototype/M_Prototype_Leather.M_Prototype_Leather"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> SkinMaterial(TEXT("/Game/WA/Characters/Prototype/M_Prototype_Skin.M_Prototype_Skin"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> HairMaterial(TEXT("/Game/WA/Characters/Prototype/M_Prototype_Hair.M_Prototype_Hair"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> AccentMaterial(TEXT("/Game/WA/Characters/Prototype/M_Prototype_Accent.M_Prototype_Accent"));

	const auto AddPart = [this](const FName Name, USceneComponent* Parent, UMaterialInterface* Material, const FVector& Location, const FVector& Scale)
	{
		UStaticMeshComponent* Part = CreateDefaultSubobject<UStaticMeshComponent>(Name);
		Part->SetupAttachment(Parent);
		Part->SetStaticMesh(CubeMesh.Object);
		Part->SetMaterial(0, Material);
		Part->SetRelativeLocation(Location);
		Part->SetRelativeScale3D(Scale);
		Part->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Part->SetGenerateOverlapEvents(false);
		Part->SetCanEverAffectNavigation(false);
	};

	AddPart(TEXT("Torso"), AdventurerVisual, TunicMaterial.Object, FVector(0.0, 0.0, 25.0), FVector(0.30, 0.46, 0.58));
	AddPart(TEXT("Belt"), AdventurerVisual, LeatherMaterial.Object, FVector(0.0, 0.0, -4.0), FVector(0.32, 0.48, 0.08));
	AddPart(TEXT("BeltBuckle"), AdventurerVisual, AccentMaterial.Object, FVector(17.0, 0.0, -4.0), FVector(0.04, 0.12, 0.12));
	AddPart(TEXT("Hips"), AdventurerVisual, PantsMaterial.Object, FVector(0.0, 0.0, -11.0), FVector(0.29, 0.42, 0.16));
	AddPart(TEXT("Head"), AdventurerVisual, SkinMaterial.Object, FVector(2.0, 0.0, 69.0), FVector(0.32, 0.32, 0.30));
	AddPart(TEXT("HairCap"), AdventurerVisual, HairMaterial.Object, FVector(1.0, 0.0, 85.0), FVector(0.36, 0.36, 0.10));
	AddPart(TEXT("HairBack"), AdventurerVisual, HairMaterial.Object, FVector(-14.0, 0.0, 70.0), FVector(0.06, 0.34, 0.30));
	AddPart(TEXT("Backpack"), AdventurerVisual, LeatherMaterial.Object, FVector(-23.0, 0.0, 28.0), FVector(0.20, 0.36, 0.38));
	AddPart(TEXT("BackpackFlap"), AdventurerVisual, AccentMaterial.Object, FVector(-33.5, 0.0, 35.0), FVector(0.03, 0.30, 0.10));

	AddPart(TEXT("LeftSleeve"), LeftArmPivot, TunicMaterial.Object, FVector(0.0, 0.0, -17.0), FVector(0.20, 0.20, 0.36));
	AddPart(TEXT("LeftHand"), LeftArmPivot, SkinMaterial.Object, FVector(0.0, 0.0, -42.0), FVector(0.16, 0.16, 0.14));
	AddPart(TEXT("RightSleeve"), RightArmPivot, TunicMaterial.Object, FVector(0.0, 0.0, -17.0), FVector(0.20, 0.20, 0.36));
	AddPart(TEXT("RightHand"), RightArmPivot, SkinMaterial.Object, FVector(0.0, 0.0, -42.0), FVector(0.16, 0.16, 0.14));
	AddPart(TEXT("LeftLeg"), LeftLegPivot, PantsMaterial.Object, FVector(0.0, 0.0, -31.0), FVector(0.22, 0.21, 0.56));
	AddPart(TEXT("LeftBoot"), LeftLegPivot, LeatherMaterial.Object, FVector(5.0, 0.0, -71.0), FVector(0.32, 0.22, 0.26));
	AddPart(TEXT("RightLeg"), RightLegPivot, PantsMaterial.Object, FVector(0.0, 0.0, -31.0), FVector(0.22, 0.21, 0.56));
	AddPart(TEXT("RightBoot"), RightLegPivot, LeatherMaterial.Object, FVector(5.0, 0.0, -71.0), FVector(0.32, 0.22, 0.26));

	InputMappingContext = TSoftObjectPtr<UInputMappingContext>(FSoftObjectPath(TEXT("/Game/WA/Characters/Input/IMC_ThirdPerson.IMC_ThirdPerson")));
	MoveAction = TSoftObjectPtr<UInputAction>(FSoftObjectPath(TEXT("/Game/WA/Characters/Input/IA_Move.IA_Move")));
	LookAction = TSoftObjectPtr<UInputAction>(FSoftObjectPath(TEXT("/Game/WA/Characters/Input/IA_Look.IA_Look")));
	JumpAction = TSoftObjectPtr<UInputAction>(FSoftObjectPath(TEXT("/Game/WA/Characters/Input/IA_Jump.IA_Jump")));
	AttackAction = TSoftObjectPtr<UInputAction>(FSoftObjectPath(TEXT("/Game/WA/Characters/Input/IA_Attack.IA_Attack")));
	InteractAction = TSoftObjectPtr<UInputAction>(FSoftObjectPath(TEXT("/Game/WA/Characters/Input/IA_Interact.IA_Interact")));
}

void AWACharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	const float MaxWalkSpeed = FMath::Max(GetCharacterMovement()->MaxWalkSpeed, 1.0f);
	const float SpeedRatio = bDead ? 0.0f : FMath::Clamp(GetVelocity().Size2D() / MaxWalkSpeed, 0.0f, 1.0f);
	RunBlend = FMath::FInterpTo(RunBlend, SpeedRatio, DeltaSeconds, 10.0f);
	if (SpeedRatio > 0.01f)
	{
		RunCyclePhase = FMath::Fmod(RunCyclePhase + DeltaSeconds * FMath::Lerp(5.0f, 11.0f, SpeedRatio), 2.0f * PI);
	}

	const float Swing = bDead ? 0.0f : FMath::Sin(RunCyclePhase) * 32.0f * RunBlend;
	LeftArmPivot->SetRelativeRotation(FRotator(Swing, 0.0, 0.0));
	RightArmPivot->SetRelativeRotation(FRotator(-Swing, 0.0, 0.0));
	LeftLegPivot->SetRelativeRotation(FRotator(-Swing * 0.85f, 0.0, 0.0));
	RightLegPivot->SetRelativeRotation(FRotator(Swing * 0.85f, 0.0, 0.0));
}

void AWACharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWACharacter, bDead);
}

void AWACharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	InitializeAbilityActorInfo();
}

void AWACharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	InitializeAbilityActorInfo();
}

UAbilitySystemComponent* AWACharacter::GetAbilitySystemComponent() const
{
	const AWAPlayerState* WAPlayerState = GetPlayerState<AWAPlayerState>();
	return WAPlayerState ? WAPlayerState->GetAbilitySystemComponent() : nullptr;
}

void AWACharacter::InitializeAbilityActorInfo()
{
	if (AWAPlayerState* WAPlayerState = GetPlayerState<AWAPlayerState>())
	{
		WAPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(WAPlayerState, this);
	}
}

void AWACharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!Input)
	{
		return;
	}

	if (APlayerController* PC = GetController<APlayerController>())
	{
		if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				if (UInputMappingContext* Context = InputMappingContext.LoadSynchronous())
				{
					InputSubsystem->AddMappingContext(Context, 0);
				}
			}
		}
	}

	if (UInputAction* Action = MoveAction.LoadSynchronous())
	{
		Input->BindAction(Action, ETriggerEvent::Triggered, this, &AWACharacter::Move);
	}
	if (UInputAction* Action = LookAction.LoadSynchronous())
	{
		Input->BindAction(Action, ETriggerEvent::Triggered, this, &AWACharacter::Look);
	}
	if (UInputAction* Action = JumpAction.LoadSynchronous())
	{
		Input->BindAction(Action, ETriggerEvent::Started, this, &AWACharacter::Jump);
		Input->BindAction(Action, ETriggerEvent::Completed, this, &AWACharacter::StopJumping);
	}
	if (UInputAction* Action = AttackAction.LoadSynchronous())
	{
		Input->BindAction(Action, ETriggerEvent::Started, this, &AWACharacter::TryAttack);
	}
	if (UInputAction* Action = InteractAction.LoadSynchronous())
	{
		Input->BindAction(Action, ETriggerEvent::Started, this, &AWACharacter::TryInteract);
	}
}

void AWACharacter::Move(const FInputActionValue& Value)
{
	if (bDead || !Controller)
	{
		return;
	}

	const FVector2D Axis = Value.Get<FVector2D>();
	const FRotator YawRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
	AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X), Axis.Y);
	AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y), Axis.X);
}

void AWACharacter::Look(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>();
	AddControllerYawInput(Axis.X);
	AddControllerPitchInput(Axis.Y);
}

void AWACharacter::TryAttack()
{
	if (!bDead)
	{
		if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
		{
			ASC->TryActivateAbilityByClass(UWAAttackAbility::StaticClass());
		}
	}
}

void AWACharacter::TryInteract()
{
	if (bDead || !FollowCamera)
	{
		return;
	}

	const FVector Start = FollowCamera->GetComponentLocation();
	const FVector End = Start + FollowCamera->GetForwardVector() * 600.0f;
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(WAInteract), false, this);
	FHitResult Hit;
	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, QueryParams))
	{
		ServerInteract(Hit.GetActor());
	}
}

void AWACharacter::ServerInteract_Implementation(AActor* Target)
{
	const UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const double CurrentTime = World->GetTimeSeconds();
	if (CurrentTime < NextServerInteractTime)
	{
		return;
	}
	NextServerInteractTime = CurrentTime + 0.2;

	if (bDead || !IsValid(Target) || FVector::DistSquared(GetActorLocation(), Target->GetActorLocation()) > FMath::Square(300.0f))
	{
		return;
	}

	if (IWAInteractable* Interactable = Cast<IWAInteractable>(Target))
	{
		Interactable->Interact(this);
	}
}

void AWACharacter::PlayTestAttackFeedback(const FVector& End)
{
	if (HasAuthority())
	{
		MulticastAttackFeedback(End);
	}
}

void AWACharacter::MulticastAttackFeedback_Implementation(FVector End)
{
	const FVector Start = GetActorLocation() + FVector(0.0f, 0.0f, 40.0f);
	DrawDebugLine(GetWorld(), Start, End, FColor::Yellow, false, 0.2f, 0, 3.0f);
}

void AWACharacter::HandleDeath()
{
	if (HasAuthority())
	{
		bDead = true;
		OnRep_Dead();
	}
}

void AWACharacter::OnRep_Dead()
{
	GetCharacterMovement()->DisableMovement();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AdventurerVisual->SetVisibility(false, true);
}
