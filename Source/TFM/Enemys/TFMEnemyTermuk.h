#pragma once

#include "CoreMinimal.h"
#include "TFMEnemy.h"
#include "TFMEnemyTermuk.generated.h"

UCLASS()
class TFM_API ATFMEnemyTermuk : public ATFMEnemy
{
	GENERATED_BODY()

public:
	ATFMEnemyTermuk();

	virtual void BeginPlay() override;

	// FSM overrides
	virtual void Patrol(float DeltaTime) override;
	virtual void Charge(float DeltaTime) override;
	virtual void Attack(float DeltaTime) override;

protected:

	// ---- components ---
	UArrowComponent* ArrowComp;

	// ---- Attack ranges ----
	UPROPERTY(EditAnywhere, Category = "Attack")
	float AttackRange = 600.f;

	// ---- Angles ----
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float LaunchPitch = 45.f; // 


	// ---- Charge ----
	UPROPERTY(EditAnywhere, Category = "Attack")
	float ChargeTime = 1.2f;

	float ChargeTimer = 0.f;

	// ---- Cooldown ----
	UPROPERTY(EditAnywhere, Category = "Attack")
	float AttackCooldownMax = 3.f;

	float AttackCooldown = 0.f;

	UPROPERTY(BlueprintReadOnly)
	bool bIsAttacking = false;

	// ---- Projectiles ----

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	USceneComponent* NailSpawnPoint;

	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<AActor> NailProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Attack")
	int32 NailsPerAttack = 5;

	UPROPERTY(EditAnywhere, Category = "Attack")
	float NailSpawnRadius = 200.f;

	UPROPERTY(EditAnywhere, Category = "Attack")
	float NailSpawnHeight = 120.f;

	UPROPERTY(EditAnywhere, Category = "Attack")
	float NailForwardOffset = 100.f;

	void SpawnNails();
	void SpawnSingleNail();

	// --- Nail Burst ---
	UPROPERTY(EditAnywhere, Category = "Attack")
	float TimeBetweenNails = 0.08f;

	int32 NailsSpawned = 0;

	FTimerHandle NailBurstTimer;


public:
	// AnimNotify
	UFUNCTION(BlueprintCallable)
	void OnAttackFinished();
};
