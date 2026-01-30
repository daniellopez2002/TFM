#include "TFMEnemyBolt.h"
#include "Engine/TargetPoint.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"

ATFMEnemyBolt::ATFMEnemyBolt()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	ArrowComp = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	ArrowComp->SetupAttachment(RootComponent);
}

void ATFMEnemyBolt::BeginPlay()
{
	Super::BeginPlay();

	CurrentPatrolIndex = 0;

	if (PatrolPoints.Num() > 0)
		CurrentTarget = PatrolPoints[CurrentPatrolIndex]->GetActorLocation();

	PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	if (MeshComp && MeshComp->GetMaterial(0))
	{
		DynamicMaterial = UMaterialInstanceDynamic::Create(
			MeshComp->GetMaterial(0), this);
		MeshComp->SetMaterial(0, DynamicMaterial);
	}

	UpdateColor(FLinearColor::Yellow);
}

void ATFMEnemyBolt::Patrol(float DeltaTime)
{
	if (PatrolPoints.Num() == 0) return;

	FVector Pos = GetActorLocation();
	FVector Dir = (CurrentTarget - Pos).GetSafeNormal();
	SetActorLocation(Pos + Dir * PatrolSpeed * DeltaTime);

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

	if (PlayerPawn)
	{
		float Dist = FVector::Dist(Pos, PlayerPawn->GetActorLocation());
		if (Dist < DetectionRadius)
			ChangeState(EEnemyState::Charge);
	}
}

void ATFMEnemyBolt::Charge(float DeltaTime)
{
	ChargeTimer += DeltaTime;

	if (DynamicMaterial)
	{
		float Pulse = (FMath::Sin(GetWorld()->GetTimeSeconds() * 8.f) + 1.f) * 0.5f;
		UpdateColor(FLinearColor::LerpUsingHSV(
			FLinearColor::White, FLinearColor::Red, Pulse));
	}

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

void ATFMEnemyBolt::Attack(float DeltaTime)
{
	DashTimer += DeltaTime;

	if (DashTimer < DashDuration)
	{
		UpdateColor(FLinearColor::Blue);
		SetActorLocation(
			GetActorLocation() +
			DashDirection * (DashDistance / DashDuration) * DeltaTime
		);
	}
	else
	{
		DashTimer = 0.0f;

		float Dist = FVector::Dist(GetActorLocation(), PlayerPawn->GetActorLocation());
		ChangeState(Dist < DetectionRadius ? EEnemyState::Charge : EEnemyState::Patrol);
	}
}

void ATFMEnemyBolt::ChangeState(EEnemyState NewState)
{
	Super::ChangeState(NewState);

	ChargeTimer = 0.0f;
	DashTimer = 0.0f;

	if (NewState == EEnemyState::Patrol)
		UpdateColor(FLinearColor::Yellow);
}

void ATFMEnemyBolt::UpdateColor(FLinearColor NewColor)
{
	if (DynamicMaterial)
	{
		DynamicMaterial->SetVectorParameterValue("BaseColor", NewColor);
	}
}
