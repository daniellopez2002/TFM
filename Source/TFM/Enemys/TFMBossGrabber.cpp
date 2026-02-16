#include "TFMBossGrabber.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"



ATFMBossGrabber::ATFMBossGrabber()
{
	PrimaryActorTick.bCanEverTick = true;

	// Root
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	// Skeletal Mesh
	BossMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BossMesh"));
	BossMesh->SetupAttachment(RootComponent);

	// Hitbox
	AttackHitbox = CreateDefaultSubobject<UBoxComponent>(TEXT("AttackHitbox"));
	AttackHitbox->SetupAttachment(BossMesh, TEXT("AttackSocket"));

	AttackHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttackHitbox->SetCollisionObjectType(ECC_WorldDynamic);
	AttackHitbox->SetCollisionResponseToAllChannels(ECR_Ignore);
	AttackHitbox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	/*AttackHitbox->OnComponentBeginOverlap.AddDynamic(
		this,
		&ATFMBossGrabber::OnHitboxOverlap
	);*/
}

void ATFMBossGrabber::BeginPlay()
{
	Super::BeginPlay();

	CurrentState = EBossState::Idle;
	CurrentAttack = EAttackType::None;

	PlayerRef = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	// start attack cycle
	GetWorldTimerManager().SetTimer(
		AttackTimer,
		this,
		&ATFMBossGrabber::ChooseRandomAttack,
		3.f,
		true
	);
}

void ATFMBossGrabber::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentState != EBossState::Dead &&
		CurrentState != EBossState::Damaged)
	{
		RotateTowardsPlayer(DeltaTime);
	}
}

void ATFMBossGrabber::RotateTowardsPlayer(float DeltaTime)
{
	if (!PlayerRef) return;

	FVector Direction = PlayerRef->GetActorLocation() - GetActorLocation();
	Direction.Z = 0;

	FRotator TargetRotation = Direction.Rotation();
	FRotator NewRotation = FMath::RInterpTo(
		GetActorRotation(),
		TargetRotation,
		DeltaTime,
		2.f
	);

	SetActorRotation(NewRotation);
}

void ATFMBossGrabber::ChooseRandomAttack()
{
	if (CurrentState == EBossState::Dead ||
		CurrentState == EBossState::Damaged)
		return;

	CurrentState = EBossState::Attacking;

	int32 Random = FMath::RandRange(0, 1);

	EAttackType SelectedAttack = (Random == 0)
		? EAttackType::ForwardThrust
		: EAttackType::Sweep;

	PerformAttack(SelectedAttack);
}

void ATFMBossGrabber::PerformAttack(EAttackType AttackType)
{
	CurrentAttack = AttackType;

	// Only variable set
	// The Animation Blueprint plays the animation

}

void ATFMBossGrabber::EnableHitbox()
{
	bHasHitPlayer = false;
	AttackHitbox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ATFMBossGrabber::DisableHitbox()
{
	AttackHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}


void ATFMBossGrabber::OnAttackFinished()
{
	if (CurrentState == EBossState::Attacking)
	{
		ResetToIdle();
	}
}


void ATFMBossGrabber::ResetToIdle()
{
	CurrentState = EBossState::Idle;
	CurrentAttack = EAttackType::None;
}

void ATFMBossGrabber::SetManualState(EBossState state)
{
	CurrentState = state;

}
