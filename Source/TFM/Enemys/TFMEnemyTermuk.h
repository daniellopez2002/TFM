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

protected:
	virtual void BeginPlay() override;

	// ---- State overrides ----
	virtual void Patrol(float DeltaTime) override;
	virtual void Charge(float DeltaTime) override;
	virtual void Attack(float DeltaTime) override;
	virtual void ChangeState(EEnemyState NewState) override;

	// ---- Detection & Attack ----
	UPROPERTY(EditAnywhere, Category = "Termuk|Combat")
	float AttackRange = 800.0f; // 8 meters

	UPROPERTY(EditAnywhere, Category = "Termuk|Combat")
	float NailSpawnRadius = 200.0f; // 2 meters

	UPROPERTY(EditAnywhere, Category = "Termuk|Combat")
	int32 NailsPerAttack = 5;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	float NailSpawnHeight = 120.f;

	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	float NailForwardOffset = 50.f;

	// ---- Cooldown ----
	UPROPERTY(EditAnywhere, Category = "Termuk|Combat")
	float CooldownAfterAttack = 5.0f;

	float CooldownTimer = 0.0f;

	// ---- References ----
	UPROPERTY()
	APawn* PlayerPawn;


	// Nail projectile class (Blueprint)
	UPROPERTY(EditAnywhere, Category = "Termuk|Combat")
	TSubclassOf<AActor> NailProjectileClass;

	// ---- Internal ----
	bool bHasAttacked = false;

	void SpawnNails();
};
