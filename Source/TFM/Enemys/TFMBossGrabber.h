#pragma once

#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "CoreMinimal.h"
#include "TFMBossGrabber.generated.h"



	UENUM(BlueprintType)
	enum class EBossState : uint8
{
	Idle,
	Searching,
	Attacking,
	Damaged,
	Dead
};

UENUM(BlueprintType)
enum class EAttackType : uint8
{
	None,
	ForwardThrust,
	Sweep
};

UCLASS()
class TFM_API ATFMBossGrabber : public APawn
{
	GENERATED_BODY()

public:
	ATFMBossGrabber();


protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* BossMesh;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* AttackHitbox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bHasHitPlayer;

private:

	// ===== STATES =====
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EBossState CurrentState;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EAttackType CurrentAttack;

	// ===== PLAYER =====
	UPROPERTY()
	APawn* PlayerRef;

	// ===== TIME =====
	FTimerHandle AttackTimer;

	// ===== FUNCTIONS =====
	void RotateTowardsPlayer(float DeltaTime);
	void ChooseRandomAttack();
	void PerformAttack(EAttackType AttackType);
	void ResetToIdle();

	

	/*UFUNCTION()
	void OnHitboxOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);*/

public:
	// AnimNotify
	UFUNCTION(BlueprintCallable)
	void EnableHitbox();
	UFUNCTION(BlueprintCallable)
	void DisableHitbox();
	UFUNCTION(BlueprintCallable)
	void OnAttackFinished();
	UFUNCTION(BlueprintCallable)
	void SetManualState(EBossState state);
};
