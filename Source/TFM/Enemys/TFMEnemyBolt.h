#pragma once

#include "CoreMinimal.h"
#include "TFMEnemy.h"
#include "TFMEnemyBolt.generated.h"

class UStaticMeshComponent;
class UArrowComponent;
class UMaterialInstanceDynamic;

UCLASS()
class TFM_API ATFMEnemyBolt : public ATFMEnemy
{
	GENERATED_BODY()

public:
	ATFMEnemyBolt();

protected:
	virtual void BeginPlay() override;

	// ---- Overrides de estados ----
	virtual void Patrol(float DeltaTime) override;
	virtual void Charge(float DeltaTime) override;
	virtual void Attack(float DeltaTime) override;
	virtual void ChangeState(EEnemyState NewState) override;

	// ---- Componentes ----
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComp;


	UPROPERTY(VisibleAnywhere)
	UArrowComponent* ArrowComp;

	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterial;




	// ---- Patrol ----
	UPROPERTY(EditAnywhere)
	TArray<AActor*> PatrolPoints;

	int32 CurrentPatrolIndex;
	FVector CurrentTarget;
	float WaitTime = 0.0f;

	UPROPERTY(EditAnywhere)
	float PatrolSpeed = 200.0f;

	// ---- Detection ----
	UPROPERTY(EditAnywhere)
	float DetectionRadius = 600.0f;

	APawn* PlayerPawn;

	// ---- Charge ----
	UPROPERTY(EditAnywhere)
	float ChargeTime = 1.5f;

	float ChargeTimer = 0.0f;

	// ---- Attack ----
	UPROPERTY(EditAnywhere)
	float DashDistance = 600.0f;

	UPROPERTY(EditAnywhere)
	float DashDuration = 0.3f;

	float DashTimer = 0.0f;
	FVector DashDirection;

	// ---- Utils ----
	void UpdateColor(FLinearColor NewColor);
};
