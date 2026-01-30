#include "TFMEnemyTermuk.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

ATFMEnemyTermuk::ATFMEnemyTermuk()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATFMEnemyTermuk::BeginPlay()
{
	Super::BeginPlay();

	PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
}

void ATFMEnemyTermuk::Patrol(float DeltaTime)
{
	// Termuk stays still and guards an area

	if (!PlayerPawn) return;

	float DistanceToPlayer =
		FVector::Dist(GetActorLocation(), PlayerPawn->GetActorLocation());

	DrawDebugSphere(
		GetWorld(),
		GetActorLocation(),
		AttackRange,
		16,
		FColor::Red
	);

	if (DistanceToPlayer <= AttackRange)
	{
		ChangeState(EEnemyState::Attack);
	}
}

void ATFMEnemyTermuk::Attack(float DeltaTime)
{
	if (bHasAttacked) return;

	SpawnNails();
	bHasAttacked = true;

	ChangeState(EEnemyState::Charge);
}

void ATFMEnemyTermuk::Charge(float DeltaTime)
{
	CooldownTimer += DeltaTime;

	if (CooldownTimer >= CooldownAfterAttack)
	{
		CooldownTimer = 0.0f;
		bHasAttacked = false;

		ChangeState(EEnemyState::Patrol);
	}
}

void ATFMEnemyTermuk::ChangeState(EEnemyState NewState)
{
	Super::ChangeState(NewState);

	// Reset timers when entering cooldown
	if (NewState == EEnemyState::Charge)
	{
		CooldownTimer = 0.0f;
	}
}

void ATFMEnemyTermuk::SpawnNails()
{
	if (!NailProjectileClass || !PlayerPawn) return;

	FVector PlayerLocation = PlayerPawn->GetActorLocation();
	FVector EnemyLocation = GetActorLocation();
	FVector Forward = GetActorForwardVector();

	for (int32 i = 0; i < NailsPerAttack; i++)
	{
		// Random offset around player (target area)
		FVector RandomOffset = FMath::VRand();
		RandomOffset.Z = 0.f;
		RandomOffset.Normalize();
		RandomOffset *= FMath::FRandRange(0.f, NailSpawnRadius);

		FVector TargetLocation = PlayerLocation + RandomOffset;

		// Spawn ABOVE and IN FRONT of Termuk
		FVector SpawnLocation =
			EnemyLocation +
			FVector(0.f, 0.f, NailSpawnHeight) +
			Forward * NailForwardOffset;

		FRotator SpawnRotation =
			(TargetLocation - SpawnLocation).Rotation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();
		SpawnParams.SpawnCollisionHandlingOverride =
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AActor* Nail = GetWorld()->SpawnActor<AActor>(
			NailProjectileClass,
			SpawnLocation,
			SpawnRotation,
			SpawnParams
		);

		if (Nail)
		{
			Nail->SetLifeSpan(5.f);
		}
	}
}

