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
	// empty on purpose
}

void ATFMEnemy::Charge(float DeltaTime)
{
	// empty on purpose
}

void ATFMEnemy::Attack(float DeltaTime)
{
	// empty on purpose
}

void ATFMEnemy::ChangeState(EEnemyState NewState)
{
	CurrentState = NewState;
}
