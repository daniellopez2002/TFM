#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "NutCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UNiagaraSystem;
struct FInputActionValue;

UCLASS(config=Game)
class ANutCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Rollig Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RollAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ChangeFocusAction;


public:
	ANutCharacter();
	

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void Attack(const FInputActionValue& Value);

	void HandleFocus(float DeltaTime);
	
	UFUNCTION(BlueprintCallable)
	void Roll(const FInputActionValue& Value);

	void EndRoll();
	void SpawnRollTrail(FVector Start, FVector End);

	void Tick(float DeltaTime) override;
			

protected:

	virtual void NotifyControllerChanged() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	/** Duración del dash en segundos */
	UPROPERTY(EditAnywhere, Category = "Roll")
	float RollDuration = 0.3f;

	UFUNCTION(BlueprintImplementableEvent, Category = "Roll")
	void OnRoll();

	UPROPERTY(EditAnywhere, Category = "Roll")
	float Energy;

	UFUNCTION(BlueprintCallable)
	bool CheckEnergy(float nrg);

	UFUNCTION(BlueprintCallable)
	void SetEnergy(float nrg);

	// Variables internas para el movimiento progresivo
	FVector DashDirection;
	float DistanceTraveled;
	bool bDashFinished;
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UPROPERTY(BlueprintReadWrite) 
	FVector CheckpointLocation;

	UPROPERTY(BlueprintReadOnly)
	bool IsRolling;

	UPROPERTY(BlueprintReadOnly)
	bool bRollOnCooldown = false;

	FTimerHandle RollCooldownHandle;

	void ResetRollCooldown();

	UPROPERTY(BlueprintReadWrite)
	bool IsStuned = false;

	UPROPERTY(BlueprintReadWrite)
	float StunedTimer = 2.5f;

	float StunerCounter = 0.0f;

	UFUNCTION(BlueprintImplementableEvent)
	void HandleStuned();

	UFUNCTION(BlueprintCallable)
	void ActivateGodMode();

	UFUNCTION(BlueprintCallable)
	void DeactivateGodMode();

	UFUNCTION(BlueprintCallable)
	void SetCheckpoint(FVector location);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Respawn();

	virtual void Respawn_Implementation();

	UFUNCTION(BlueprintCallable)
	void ChangeFocus(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable)
	void RemoveActorFromFocus(AActor* actor);

	UPROPERTY(BlueprintReadWrite, Category = "Focus")
	TArray<AActor*> ActorsFocus;

	UPROPERTY(BlueprintReadWrite, Category = "Focus")
	int FocusIndex = 0;

	UPROPERTY(EditAnywhere, Category = "Focus")
	float FocusInterpSpeed = 10.f;

	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	bool IsAttacking = false;

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	bool bComboInputQueued = false;

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	int32 ComboIndex = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	int32 MaxCombo = 3;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UAnimMontage* AttackMontage;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void StartCombo();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void AdvanceCombo();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void EndCombo();

	// Roll
	UPROPERTY(EditAnywhere, Category = "Roll")
	float RollDistance = 600.f;

	UPROPERTY(EditAnywhere, Category = "Roll")
	float RollCooldown = 0.8f;

	//UPROPERTY(EditAnywhere, Category = "Roll")
	//UNiagaraSystem* RollTrailFX;

	FTimerHandle RollTimerHandle;

};

