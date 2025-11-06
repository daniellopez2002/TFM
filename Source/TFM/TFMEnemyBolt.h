#pragma once

#include "Engine/TargetPoint.h"
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TFMEnemyBolt.generated.h"


UENUM(BlueprintType)
enum class EEnemyState : uint8
{
    Patrol,
    Charge,
    Attack
};

UCLASS()
class TFM_API ATFMEnemyBolt : public APawn
{
    GENERATED_BODY()

public:
    ATFMEnemyBolt();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // --- State ---
    EEnemyState CurrentState;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EnemyBolt|Components")
    UStaticMeshComponent* SphereComp;


    // --- Patrol ---
    UPROPERTY(EditAnywhere, Category = "EnemyBolt|Patrol")
    TArray<ATargetPoint*> PatrolPoints;

    int CurrentPatrolIndex;
    FVector CurrentTarget;

    UPROPERTY(EditAnywhere, Category = "EnemyBolt|Movement")
    float PatrolSpeed = 300.0f; // 3 m/s (300 cm/s)

    float WaitTime = 0.0f;

    // --- Player detection ---
    UPROPERTY(EditAnywhere, Category = "EnemyBolt|Detection")
    float DetectionRadius = 500.0f; // 5 m = 500 cm

    APawn* PlayerPawn;

    // --- Charge & Attack ---
    UPROPERTY(EditAnywhere, Category = "EnemyBolt|Attack")
    float ChargeTime = 3.0f;

    UPROPERTY(EditAnywhere, Category = "EnemyBolt|Attack")
    float DashDistance = 500.0f;

    UPROPERTY(EditAnywhere, Category = "EnemyBolt|Attack")
    float DashDuration = 0.5f;

    float ChargeTimer = 0.0f;
    FVector DashDirection;
    float DashTimer = 0.0f;

    // --- Visuals ---
    UPROPERTY(VisibleAnywhere, Category = "Visual")
    UStaticMeshComponent* MeshComp;

    UMaterialInstanceDynamic* DynamicMaterial;

    // --- Internal functions ---
    void Patrol(float DeltaTime);
    void Charge(float DeltaTime);
    void Attack(float DeltaTime);
    void ChangeState(EEnemyState NewState);

    void UpdateColor(FLinearColor NewColor);

};