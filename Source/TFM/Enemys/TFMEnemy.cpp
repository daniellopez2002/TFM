#include "TFMEnemy.h"
#include "Engine/TargetPoint.h"
#include "Kismet/GameplayStatics.h"

ATFMEnemy::ATFMEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATFMEnemy::BeginPlay()
{
	Super::BeginPlay();

	PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	CurrentState = EEnemyState::Patrol;
	LastLocation = GetActorLocation();

	if (PatrolPoints.Num() > 0)
	{
		CurrentTarget = PatrolPoints[0]->GetActorLocation();
	}
}

void ATFMEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector CurrentLocation = GetActorLocation();

	CurrentSpeed = (CurrentLocation - LastLocation).Size() / DeltaTime;
	LastLocation = CurrentLocation;

	switch (CurrentState)
	{
	case EEnemyState::Patrol:
		Patrol(DeltaTime);
		break;

	case EEnemyState::Charge:
		Charge(DeltaTime);
		break;

	case EEnemyState::Attack:
		Attack(DeltaTime);
		break;
	}
}

void ATFMEnemy::RotateTowards(const FVector& TargetLocation, float DeltaTime)
{
	FVector Direction = TargetLocation - GetActorLocation();
	Direction.Z = 0.f; // evita que mire hacia arriba/abajo

	if (Direction.IsNearlyZero()) return;

	FRotator TargetRotation = Direction.Rotation();
	FRotator NewRotation = FMath::RInterpTo(
		GetActorRotation(),
		TargetRotation,
		DeltaTime,
		6.f //rotation speed
	);

	SetActorRotation(NewRotation);
}


void ATFMEnemy::Patrol(float DeltaTime)
{
	if (PatrolPoints.Num() == 0) return;

	FVector CurrentLocation = GetActorLocation();

	FVector Direction = (CurrentTarget - CurrentLocation).GetSafeNormal();

	SetActorLocation(CurrentLocation + Direction * PatrolSpeed * DeltaTime);

	RotateTowards(CurrentTarget, DeltaTime);

	if (FVector::Dist(CurrentLocation, CurrentTarget) < 50.f)
	{
		WaitTime += DeltaTime;

		if (WaitTime >= PatrolWaitTime)
		{
			CurrentPatrolIndex = (CurrentPatrolIndex + 1) % PatrolPoints.Num();
			CurrentTarget = PatrolPoints[CurrentPatrolIndex]->GetActorLocation();
			WaitTime = 0.f;
		}
	}
}


void ATFMEnemy::Charge(float DeltaTime)
{
	if (PlayerPawn)
	{
		RotateTowards(PlayerPawn->GetActorLocation(), DeltaTime);
	}
}

void ATFMEnemy::Attack(float DeltaTime)
{
	if (PlayerPawn)
	{
		RotateTowards(PlayerPawn->GetActorLocation(), DeltaTime);
	}
}

void ATFMEnemy::ChangeState(EEnemyState NewState)
{
	CurrentState = NewState;
}
