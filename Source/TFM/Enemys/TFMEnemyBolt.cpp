#include "TFMEnemyBolt.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"

ATFMEnemyBolt::ATFMEnemyBolt()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);

	ArrowComp = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	ArrowComp->SetupAttachment(RootComponent);
}

void ATFMEnemyBolt::BeginPlay()
{
	Super::BeginPlay();

    if (PatrolPoints.Num() > 0)
        CurrentTarget = PatrolPoints[CurrentPatrolIndex]->GetActorLocation();

    PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

    if (MeshComp && MeshComp->GetMaterial(0))
    {
        DynamicMaterial = UMaterialInstanceDynamic::Create(MeshComp->GetMaterial(0), this);
        MeshComp->SetMaterial(0, DynamicMaterial);
    }

    // Initial state yellow
    UpdateColor(FLinearColor::Yellow);
}

void ATFMEnemyBolt::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    if (!IsActivated) return;

    switch (CurrentState)
    {
    case EEnemyState::Patrol:
        Patrol(DeltaTime);
        break;

    case EEnemyState::Charge:
        Charge(DeltaTime);

        // Switch red and white
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

	UpdateColor(FLinearColor::Yellow);

}

void ATFMEnemyBolt::Patrol(float DeltaTime)
{
	//  Use base patrol movement
	Super::Patrol(DeltaTime);

	//  Bolt-specific detection logic
	if (!PlayerPawn) return;

	float Distance =
		FVector::Dist(GetActorLocation(), PlayerPawn->GetActorLocation());

	if (Distance < DetectionRadius)
	{
		ChangeState(EEnemyState::Charge);
	}
}

void ATFMEnemyBolt::Charge(float DeltaTime)
{
	Super::Charge(DeltaTime);
	ChargeTimer += DeltaTime;

	if (DynamicMaterial)
	{
		float Pulse =
			(FMath::Sin(GetWorld()->GetTimeSeconds() * 8.f) + 1.f) * 0.5f;

		UpdateColor(FLinearColor::LerpUsingHSV(
			FLinearColor::White, FLinearColor::Red, Pulse));
	}

	if (ChargeTimer >= ChargeTime && PlayerPawn)
	{
		ChargeTimer = 0.f;
		DashDirection =
			(PlayerPawn->GetActorLocation() - GetActorLocation()).GetSafeNormal();

		ChangeState(EEnemyState::Attack);
	}
}

void ATFMEnemyBolt::Attack(float DeltaTime)
{
	Super::Attack(DeltaTime);
	DashTimer += DeltaTime;

	if (DashTimer < DashDuration)
	{
		UpdateColor(FLinearColor::Blue);

		FHitResult Hit;
		SetActorLocation(
			GetActorLocation() +
			DashDirection * (DashDistance / DashDuration) * DeltaTime,
			true,   
			&Hit
		);

	}
	else
	{
		DashTimer = 0.f;

		float Distance =
			FVector::Dist(GetActorLocation(), PlayerPawn->GetActorLocation());

		ChangeState(
			Distance < DetectionRadius
			? EEnemyState::Charge
			: EEnemyState::Patrol
		);
	}
}

void ATFMEnemyBolt::ChangeState(EEnemyState NewState)
{
	Super::ChangeState(NewState);

	ChargeTimer = 0.f;
	DashTimer = 0.f;

	if (NewState == EEnemyState::Patrol)
	{
		UpdateColor(FLinearColor::Yellow);
	}
}

void ATFMEnemyBolt::UpdateColor(FLinearColor NewColor)
{
	if (DynamicMaterial)
	{
		DynamicMaterial->SetVectorParameterValue("BaseColor", NewColor);
	}
}
