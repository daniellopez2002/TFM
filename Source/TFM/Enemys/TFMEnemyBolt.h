#pragma once
#include "TFMEnemy.h"
#include "CoreMinimal.h"
#include "TFMEnemyBolt.generated.h"

class UStaticMeshComponent;
class UArrowComponent;
class UMaterialInstanceDynamic;

UCLASS()
class TFM_API ATFMEnemyBolt : public ATFMEnemy
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaTime) override;
	ATFMEnemyBolt();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
    bool IsActivated = false;

protected:
	virtual void BeginPlay() override;
	
	// ---- Overrides de estados ----
	virtual void Patrol(float DeltaTime) override;
	virtual void Charge(float DeltaTime) override;
	virtual void Attack(float DeltaTime) override;
	virtual void ChangeState(EEnemyState NewState) override;

	// ---- Components ----
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComp;


	UArrowComponent* ArrowComp;

	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterial;



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

	UPROPERTY(BlueprintReadWrite)
	bool ImpactAttack = false;

	// ---- Utils ----
	void UpdateColor(FLinearColor NewColor); //changes the color depending on the bolt enemy state
};
