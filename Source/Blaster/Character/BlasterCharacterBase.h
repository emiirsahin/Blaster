// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BlasterTypes/TurningInPlace.h"
#include "Interfaces/InteractWithCrosshairsInterface.h"
#include "Components/TimelineComponent.h"
#include "BlasterTypes/CombatState.h"
#include "BlasterCharacterBase.generated.h"

UCLASS()
class BLASTER_API ABlasterCharacterBase : public ACharacter, public IInteractWithCrosshairsInterface
{
	GENERATED_BODY()

public:
	ABlasterCharacterBase();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	void PlayFireMontage(bool bAiming);
	void PlayReloadMontage();
	void PlayElimMontage();
	virtual void OnRep_ReplicatedBasedMovement() override;
	void Elim();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastElim();
	virtual void Destroyed() override;
	
	// Optional: expose for tuning in editor
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Turn In Place")
	float SlowYawDegPerSec = 60.f;      // at/below this => play rate = 1.0

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Turn In Place")
	float FastYawDegPerSec = 240.f;     // at/above this => play rate = MaxTurnPlayRate

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Turn In Place")
	float MaxTurnPlayRate = 2.0f;       // upper clamp

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Turn In Place")
	float PlayRateSmoothing = 8.f;      // FInterpTo smoothing (higher = snappier)


	UPROPERTY(EditAnywhere, Category="LookAt|Head")
	float HeadPitchClampMin = -40.f;   // look down
	UPROPERTY(EditAnywhere, Category="LookAt|Head")
	float HeadPitchClampMax =  60.f;   // look up
	UPROPERTY(EditAnywhere, Category="LookAt|Head")
	float HeadYawClampAbs   =  60.f;   // left/right
	UPROPERTY(EditAnywhere, Category="LookAt|Head")
	float HeadInterpSpeed   =   8.f;   // smoothing
	
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void EquipButtonPressed();
	void CrouchButtonPressed();
	void ReloadButtonPressed();
	void AimButtonPressed();
	void AimButtonReleased();
	void CalculateAO_Pitch();
	void AimOffset(float DeltaSeconds);
	void SimProxiesTurn();
	virtual void Jump() override;
	virtual void Landed(const FHitResult& Hit) override;
	void FireButtonPressed();
	void FireButtonReleased();
	void PlayHitReactMontage();
	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser);
	void UpdateHUDHealth();
	void PollInit(); // Poll for any relevant classes and initialize our HUD

private:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* OverheadWidget;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeapon* OverlappingWeapon;

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastOverlappingWeapon);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCombatComponent* Combat;

	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();

	float AO_Yaw;
	float InterpAO_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation;

	ETurningInPlace TurningInPlace;
	void TurnInPlace(float DeltaSeconds);

	bool bJumped;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* FireWeaponMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* ReloadMontage;
	
	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* ElimMontage;
	
	UPROPERTY(EditAnywhere, Category = Weapon)
	float AO_Left_Yaw_Threshold;
	UPROPERTY(EditAnywhere, Category = Weapon)
	float AO_Right_Yaw_Threshold;

	UPROPERTY(EditAnywhere, Category = Sound)
	class USoundCue* JumpStopSound;

	UPROPERTY(EditAnywhere, Category = Sound)
	float JumpStopSoundVerticalVelocityThreshold;

	UPROPERTY(EditAnywhere, Category = Sound)
	float JumpStopSoundRangeLower;

	UPROPERTY(EditAnywhere, Category = Sound)
	float JumpStopSoundRangeUpper;

	float JumpStopSoundVelocityFactor;

	float LastAimYaw = 0.f;
	float TurnPlayRate = 1.f;
	bool  bHasLastAimYaw = false;

	void TurningSpeed(float DeltaTime);
	float CalcYawSpeedDegPerSec(float DeltaSeconds);
	
	float HeadLookPitch = 0.f;

	void HeadRotationToCrosshair(float DeltaTime);

	void HideCameraIfCharacterClose();

	UPROPERTY(EditAnywhere)
	float CameraThreshold = 200.f;

	bool bRotateRootBone;

	UPROPERTY(EditAnywhere)
	float TurnThreshold = 0.5f;
	
	FRotator ProxyRotationLastFrame;
	FRotator ProxyRotation;
	float ProxyYaw;
	float TimeSinceLastMovementReplication;
	float CalculateSpeed();


	// Player Health

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxHealth = 100.f;

	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "Player Stats")
	float Health = 100.f;

	UFUNCTION()
	void OnRep_Health();

	UPROPERTY()
	class ABlasterPlayerControllerBase* BlasterPlayerController;

	bool bElimmed = false;

	FTimerHandle ElimTimer;
	
	UPROPERTY(EditDefaultsOnly)
	float ElimDelay = 3.f;
	
	void ElimTimerFinished();


	// Dissolve effect

	UPROPERTY(VisibleAnywhere)
	UTimelineComponent* DissolveTimeline;
	FOnTimelineFloat DissolveTrack;
	
	UPROPERTY(EditAnywhere)
	UCurveFloat* DissolveCurve;

	UFUNCTION()
	void UpdateDissolveMaterial(float DissolveValue);
	void StartDissolve();

	UPROPERTY(VisibleAnywhere, Category = Elim)
	UMaterialInstanceDynamic* DynamicDissolveMaterialInstance;

	UPROPERTY(EditAnywhere, Category = Elim)
	UMaterialInstance* DissolveMaterialInstance;


	// Elim bot

	UPROPERTY(EditAnywhere)
	UParticleSystem* ElimBotEffect;

	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* ElimBotComponent;

	UPROPERTY(EditAnywhere)
	USoundCue* ElimBotSound;

	UPROPERTY()
	class ABlasterPlayerState* BlasterPlayerState;

public:
	void SetOverlappingWeapon(AWeapon* Weapon);
	bool IsWeaponEquipped();
	bool IsAiming();
	bool IsJumped();
	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	AWeapon* GetEquippedWeapon();
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }
	FORCEINLINE FVector GetHitTarget() const;
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE float GetTurnPlayRate() const { return TurnPlayRate; }
	FORCEINLINE float GetHeadLookPitch() const { return HeadLookPitch; }
	FORCEINLINE bool ShouldRotateRootBone() const { return bRotateRootBone; }
	FORCEINLINE bool IsElimmed() const { return bElimmed; }
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	ECombatState GetCombatState() const;
};
