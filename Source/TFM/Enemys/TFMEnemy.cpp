#include "TFMEnemy.h"
#include "Engine/TargetPoint.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Components/SceneComponent.h"

ATFMEnemy::ATFMEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	AudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComp"));
	AudioComp->SetupAttachment(RootComponent);
	AudioComp->bAutoActivate = true;
}

void ATFMEnemy::BeginPlay()
{
	Super::BeginPlay();

	PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	CurrentState = EEnemyState::Patrol;
	LastLocation = GetActorLocation();
	_originalPosition = GetActorLocation();

	if (PatrolPoints.Num() > 0)
	{
		CurrentTarget = PatrolPoints[0]->GetActorLocation();
	}
}

void ATFMEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsKnockedBack)
	{
		AddActorWorldOffset(
			KnockbackVelocity * DeltaTime,
			true 
		);

		KnockbackVelocity = FMath::VInterpTo(
			KnockbackVelocity,
			FVector::ZeroVector,
			DeltaTime,
			KnockbackFriction
		);

		if (KnockbackVelocity.Size() < 10.f)
		{
			bIsKnockedBack = false;
			IsStuned = false;
		}

		return;
	}

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
	case EEnemyState::Stunned:
		// Mientras está aturdido no se mueve ni rota
		StunTimer -= DeltaTime;
		if (StunTimer <= 0.f)
		{
			ChangeState(_lastState);
		}
		break;
	}
}

void ATFMEnemy::RotateTowards(const FVector& TargetLocation, float DeltaTime)
{
	FVector Direction = TargetLocation - GetActorLocation();
	Direction.Z = 0.f; //look only at the front

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
	_lastState = CurrentState;
	CurrentState = NewState;
}

void ATFMEnemy::ReceiveAttack_Implementation(FVector AttackDirection, float Force)
{
	if (CurrentState == EEnemyState::Stunned)
		return;

	AttackDirection.Z = 0.f;

	KnockbackVelocity =
		AttackDirection.GetSafeNormal() * Force;

	bIsKnockedBack = true;
	IsStuned = true; 
	ChangeState(EEnemyState::Stunned);
}