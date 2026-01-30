#include "TFMEnemy.h"

ATFMEnemy::ATFMEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATFMEnemy::BeginPlay()
{
	Super::BeginPlay();

	CurrentState = EEnemyState::Patrol;
}

void ATFMEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

void ATFMEnemy::Patrol(float DeltaTime)
{
	// Vacío a propósito
}

void ATFMEnemy::Charge(float DeltaTime)
{
	// Vacío a propósito
}

void ATFMEnemy::Attack(float DeltaTime)
{
	// Vacío a propósito
}

void ATFMEnemy::ChangeState(EEnemyState NewState)
{
	CurrentState = NewState;
}
