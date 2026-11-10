#include "WAPlayer/WACharacter.h"

#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "Net/UnrealNetwork.h"
#include "UObject/ConstructorHelpers.h"
#include "WACore/WAInteractable.h"
#include "WAAbilities/WAAttackAbility.h"
#include "WAPlayer/WAPlayerState.h"

AWACharacter::AWACharacter()
{
	PrimaryActorTick.bCanEverTick = false;
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

	DebugVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DebugVisual"));
	DebugVisual->SetupAttachment(RootComponent);
	DebugVisual->SetRelativeScale3D(FVector(0.65, 0.65, 1.75));
	DebugVisual->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		DebugVisual->SetStaticMesh(CubeMesh.Object);
	}

	InputMappingContext = TSoftObjectPtr<UInputMappingContext>(FSoftObjectPath(TEXT("/Game/WA/Characters/Input/IMC_ThirdPerson.IMC_ThirdPerson")));
	MoveAction = TSoftObjectPtr<UInputAction>(FSoftObjectPath(TEXT("/Game/WA/Characters/Input/IA_Move.IA_Move")));
	LookAction = TSoftObjectPtr<UInputAction>(FSoftObjectPath(TEXT("/Game/WA/Characters/Input/IA_Look.IA_Look")));
	JumpAction = TSoftObjectPtr<UInputAction>(FSoftObjectPath(TEXT("/Game/WA/Characters/Input/IA_Jump.IA_Jump")));
	AttackAction = TSoftObjectPtr<UInputAction>(FSoftObjectPath(TEXT("/Game/WA/Characters/Input/IA_Attack.IA_Attack")));
	InteractAction = TSoftObjectPtr<UInputAction>(FSoftObjectPath(TEXT("/Game/WA/Characters/Input/IA_Interact.IA_Interact")));
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
	DebugVisual->SetVisibility(false);
}
