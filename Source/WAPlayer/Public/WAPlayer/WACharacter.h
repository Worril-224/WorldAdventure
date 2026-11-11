#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "WAAbilities/WAAbilityAvatar.h"
#include "WACharacter.generated.h"

class UCameraComponent;
class UInputAction;
class UInputMappingContext;
class USceneComponent;
class USpringArmComponent;
struct FInputActionValue;

UCLASS()
class WAPLAYER_API AWACharacter : public ACharacter, public IAbilitySystemInterface, public IWAAbilityAvatar
{
	GENERATED_BODY()

public:
	AWACharacter();
	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual void PlayTestAttackFeedback(const FVector& End) override;
	virtual void HandleDeath() override;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> AdventurerVisual;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> LeftArmPivot;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> RightArmPivot;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> LeftLegPivot;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> RightLegPivot;

	float RunCyclePhase = 0.0f;
	float RunBlend = 0.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TSoftObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TSoftObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TSoftObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TSoftObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TSoftObjectPtr<UInputAction> AttackAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TSoftObjectPtr<UInputAction> InteractAction;

	UPROPERTY(ReplicatedUsing = OnRep_Dead)
	bool bDead = false;

	double NextServerInteractTime = 0.0;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void TryAttack();
	void TryInteract();
	void InitializeAbilityActorInfo();

	UFUNCTION()
	void OnRep_Dead();

	UFUNCTION(Server, Reliable)
	void ServerInteract(AActor* Target);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastAttackFeedback(FVector End);
};
