#include "TFMEnemyTermuk.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/ProjectileMovementComponent.h"

ATFMEnemyTermuk::ATFMEnemyTermuk()
{
	PrimaryActorTick.bCanEverTick = true;

	NailSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("NailSpawnPoint"));
	NailSpawnPoint->SetupAttachment(RootComponent);
}


void ATFMEnemyTermuk::BeginPlay()
{
	Super::BeginPlay();


}

// -------------------- PATROL --------------------
void ATFMEnemyTermuk::Patrol(float DeltaTime)
{
	Super::Patrol(DeltaTime);

	if (!PlayerPawn || AttackCooldown > 0.f)
	{
		AttackCooldown -= DeltaTime;
		return;
	}

	float Distance =
		FVector::Dist(GetActorLocation(), PlayerPawn->GetActorLocation());

	if (Distance <= AttackRange)
	{
		ChargeTimer = 0.f;
		ChangeState(EEnemyState::Charge);
	}
}

// -------------------- CHARGE --------------------
void ATFMEnemyTermuk::Charge(float DeltaTime)
{
	Super::Charge(DeltaTime);
	if (!PlayerPawn) return;

	ChargeTimer += DeltaTime;


	if (ChargeTimer >= ChargeTime)
	{
		ChargeTimer = 0.f;
		bIsAttacking = true;
		ChangeState(EEnemyState::Attack);
	}
}

// -------------------- ATTACK --------------------
void ATFMEnemyTermuk::Attack(float DeltaTime)
{
	Super::Attack(DeltaTime);
}


// -------------------- NAILS --------------------
void ATFMEnemyTermuk::SpawnSingleNail()
{
	if (!NailProjectileClass || !PlayerPawn) return;

	FVector PlayerLocation = PlayerPawn->GetActorLocation();
	FVector SpawnLocation =
		NailSpawnPoint
		? NailSpawnPoint->GetComponentLocation()
		: GetActorLocation();

	FVector RandomOffset = FMath::VRand();
	RandomOffset.Z = 0.f;
	RandomOffset.Normalize();
	RandomOffset *= FMath::FRandRange(0.f, NailSpawnRadius);

	FVector TargetLocation = PlayerLocation + RandomOffset;
	FVector ToTarget = TargetLocation - SpawnLocation;

	FRotator SpawnRotation = ToTarget.Rotation();
	SpawnRotation.Pitch = LaunchPitch;

	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AActor* Projectile = GetWorld()->SpawnActor<AActor>(
		NailProjectileClass,
		SpawnLocation,
		SpawnRotation,
		Params
	);

	if (Projectile)
	{
		UProjectileMovementComponent* PMC =
			Projectile->FindComponentByClass<UProjectileMovementComponent>();

		if (PMC)
		{
			PMC->Velocity = SpawnRotation.Vector() * PMC->InitialSpeed;
		}
	}

	NailsSpawned++;

	// finished nails spawn
	if (NailsSpawned >= NailsPerAttack)
	{
		GetWorldTimerManager().ClearTimer(NailBurstTimer);
	}
}



void ATFMEnemyTermuk::SpawnNails()
{
	NailsSpawned = 0;

	GetWorldTimerManager().SetTimer(
		NailBurstTimer,
		this,
		&ATFMEnemyTermuk::SpawnSingleNail,
		TimeBetweenNails,
		true
	);
}



// -------------------- ANIMATION CALLBACK --------------------
void ATFMEnemyTermuk::OnAttackFinished()
{
	UE_LOG(LogTemp, Warning, TEXT("TERMUK ATTACK NOTIFY FIRED"));

	SpawnNails();

	bIsAttacking = false;
	AttackCooldown = AttackCooldownMax;

	ChangeState(EEnemyState::Patrol);
}
