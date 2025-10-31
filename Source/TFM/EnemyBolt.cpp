#include "EnemyBolt.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Components/StaticMeshComponent.h"

AEnemyBolt::AEnemyBolt()
{
    PrimaryActorTick.bCanEverTick = true;

    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    RootComponent = MeshComp;

    SphereComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SphereComp"));
    SphereComp->SetupAttachment(MeshComp);
}

void AEnemyBolt::BeginPlay()
{
    Super::BeginPlay();

    CurrentState = EEnemyState::Patrol;
    CurrentPatrolIndex = 0;

    if (PatrolPoints.Num() > 0)
        CurrentTarget = PatrolPoints[CurrentPatrolIndex]->GetActorLocation();

    PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

    if (SphereComp && SphereComp->GetMaterial(0))
    {
        DynamicMaterial = UMaterialInstanceDynamic::Create(SphereComp->GetMaterial(0), this);
        SphereComp->SetMaterial(0, DynamicMaterial);
    }

    // Estado inicial color amarillo
    UpdateColor(FLinearColor::Yellow);
}

void AEnemyBolt::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    switch (CurrentState)
    {
    case EEnemyState::Patrol:
        Patrol(DeltaTime);
        break;

    case EEnemyState::Charge:
        Charge(DeltaTime);

        // Parpadeo rojo blanco
        if (DynamicMaterial)
        {
            float Pulse = (FMath::Sin(GetWorld()->GetTimeSeconds() * 8.0f) + 1.0f) * 0.5f;
            FLinearColor BlinkColor = FLinearColor::LerpUsingHSV(FLinearColor::White, FLinearColor::Red, Pulse);
            UpdateColor(BlinkColor);
        }
        break;

    case EEnemyState::Attack:
        Attack(DeltaTime);
        break;
    }

    DrawDebugSphere(GetWorld(), GetActorLocation(), DetectionRadius, 16, FColor::Yellow);

    if (PlayerPawn)
    {
        float Distance = FVector::Dist(GetActorLocation(), PlayerPawn->GetActorLocation());
        if (Distance < DetectionRadius && CurrentState == EEnemyState::Patrol)
        {
            ChangeState(EEnemyState::Charge);
        }
    }
}

void AEnemyBolt::Patrol(float DeltaTime)
{
    if (PatrolPoints.Num() == 0) return;

    FVector Pos = GetActorLocation();
    FVector Direction = (CurrentTarget - Pos).GetSafeNormal();
    SetActorLocation(Pos + Direction * PatrolSpeed * DeltaTime);

    if (FVector::Dist(Pos, CurrentTarget) < 50.0f)
    {
        WaitTime += DeltaTime;
        if (WaitTime >= 2.0f)
        {
            CurrentPatrolIndex = (CurrentPatrolIndex + 1) % PatrolPoints.Num();
            CurrentTarget = PatrolPoints[CurrentPatrolIndex]->GetActorLocation();
            WaitTime = 0.0f;
        }
    }
}

void AEnemyBolt::Charge(float DeltaTime)
{
    ChargeTimer += DeltaTime;

    if (ChargeTimer >= ChargeTime)
    {
        ChargeTimer = 0.0f;
        if (PlayerPawn)
        {
            DashDirection = (PlayerPawn->GetActorLocation() - GetActorLocation()).GetSafeNormal();
            ChangeState(EEnemyState::Attack);
        }
    }
}

void AEnemyBolt::Attack(float DeltaTime)
{
    DashTimer += DeltaTime;
    float Progress = DashTimer / DashDuration;

    if (Progress < 1.0f)
    {
        UpdateColor(FLinearColor::Blue);
        FVector NewPos = GetActorLocation()
            + DashDirection * (DashDistance / DashDuration) * DeltaTime;
        SetActorLocation(NewPos);
    }
    else
    {
        DashTimer = 0.0f;

        float Distance = FVector::Dist(GetActorLocation(), PlayerPawn->GetActorLocation());
        if (Distance < DetectionRadius)
            ChangeState(EEnemyState::Charge);
        else
            ChangeState(EEnemyState::Patrol);
    }
}


void AEnemyBolt::ChangeState(EEnemyState NewState)
{
    CurrentState = NewState;
    ChargeTimer = 0.0f;
    DashTimer = 0.0f;

    // Color para cada estado
    switch (CurrentState)
    {
    case EEnemyState::Patrol:
        UpdateColor(FLinearColor::Yellow);
        break;

    case EEnemyState::Charge:
        // No seteamos color directamente, porque ya lo animamos en Tick
        break;

    case EEnemyState::Attack:
        UpdateColor(FLinearColor::Blue);
        break;
    }
}


void AEnemyBolt::UpdateColor(FLinearColor NewColor)
{
    if (DynamicMaterial)
    {
        DynamicMaterial->SetVectorParameterValue("BaseColor", NewColor);
    }
}
