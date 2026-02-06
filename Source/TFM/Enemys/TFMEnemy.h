#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Engine/TargetPoint.h"
#include "TFMEnemy.generated.h"

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	Patrol,
	Charge,
	Attack
};

UCLASS()
class TFM_API ATFMEnemy : public APawn
{
	GENERATED_BODY()

public:
	ATFMEnemy();

protected:
	virtual void BeginPlay() override;
	void RotateTowards(const FVector& TargetLocation, float DeltaTime);

public:
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY()
	APawn* PlayerPawn;

	// ===== STATE MACHINE =====
	UPROPERTY(BlueprintReadOnly, Category = "Enemy")
	EEnemyState CurrentState;

	virtual void Patrol(float DeltaTime);
	virtual void Charge(float DeltaTime);
	virtual void Attack(float DeltaTime);

	virtual void ChangeState(EEnemyState NewState);

	// ===== PATROL SYSTEM (GENERIC) =====
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Patrol")
	TArray<ATargetPoint*> PatrolPoints;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Patrol")
	float PatrolSpeed = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Patrol")
	float PatrolWaitTime = 2.f;

	int32 CurrentPatrolIndex = 0;
	float WaitTime = 0.f;
	FVector CurrentTarget;

	FVector LastLocation;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float CurrentSpeed;
};
