#pragma once
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "GameFramework/Pawn.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
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

	//================= SOUND =================
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
	UAudioComponent* StateAudioComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundBase* PatrolSound;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundBase* AttackSound;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundBase* DamageSound;

	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	USoundBase* DeathSound;

	// ================= UI =================
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<class UUserWidget> BossHealthWidgetClass;

	UPROPERTY(BlueprintReadOnly)
	UUserWidget* BossHealthWidget;

private:

	// ===== STATES =====
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EBossState CurrentState;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EAttackType CurrentAttack;

	// ===== PLAYER =====
	UPROPERTY()
	APawn* PlayerRef;

	UPROPERTY()
	bool IsBattleStarted;

	UPROPERTY(EditAnywhere, Category = "Boss|Health")
	float HealthPercent;

	// ===== TIME =====
	FTimerHandle AttackTimer;

	// ===== FUNCTIONS =====
	void RotateTowardsPlayer(float DeltaTime);
	void ChooseRandomAttack();
	void PerformAttack(EAttackType AttackType);
	void ResetToIdle();
	void RestartAttackTimer();
	void UpdateStateAudio();

	



	// ================= PHASE SYSTEM =================

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Phases", meta = (AllowPrivateAccess = "true"))
	int32 CurrentPhase = 1;


	UPROPERTY(EditAnywhere, Category = "Boss|Phases")
	float Phase2Threshold = 0.66f;

	UPROPERTY(EditAnywhere, Category = "Boss|Phases")
	float Phase3Threshold = 0.33f;

	UPROPERTY(EditAnywhere, Category = "Boss|Phases")
	float Phase2AttackSpeedMultiplier = 1.15f;

	UPROPERTY(EditAnywhere, Category = "Boss|Phases")
	float Phase3AttackSpeedMultiplier = 1.30f;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Phases", meta = (AllowPrivateAccess = "true"))
	float CurrentAttackSpeedMultiplier = 1.f;


	// ================= ATTACK LOOP =================

	UPROPERTY(EditAnywhere, Category = "Boss|Combat")
	float BaseAttackInterval = 3.f;

	UPROPERTY(EditAnywhere, Category = "Boss|Combat")
	int32 AttacksBeforePatrol = 2;

	int32 CurrentAttackCount = 0;




public:
	UFUNCTION()
	void OnStateSoundFinished();
	UFUNCTION(BlueprintCallable)
	void PLayerHit();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ActivateBossFight();

	UFUNCTION(BlueprintCallable)
	float GetHealthPercent() const;

	void Death();


	// AnimNotify
	UFUNCTION(BlueprintCallable)
	void OnPatrolFinished();
	UFUNCTION(BlueprintCallable)
	void OnPhaseTransitionFinished();
	UFUNCTION(BlueprintCallable)
	void EnableHitbox();
	UFUNCTION(BlueprintCallable)
	void DisableHitbox();
	UFUNCTION(BlueprintCallable)
	void OnAttackFinished();
	UFUNCTION(BlueprintCallable)
	void SetManualState(EBossState state);
	UFUNCTION(BlueprintCallable)
	void CheckPhaseTransition(int CurrentHealth, int MaxHealth);
};
