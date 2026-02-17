#include "TFMBossGrabber.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/ActorComponent.h"
#include "Components/BoxComponent.h"
#include "Blueprint/UserWidget.h"




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


}

void ATFMBossGrabber::BeginPlay()
{
	Super::BeginPlay();


	


	CurrentState = EBossState::Idle;
	CurrentAttack = EAttackType::None;

	PlayerRef = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);


	RestartAttackTimer();

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

void ATFMBossGrabber::CheckPhaseTransition(int CurrentHealth) {

		if (CurrentState == EBossState::Dead)
			return;

		if (CurrentHealth == 0) {
			CurrentState = EBossState::Dead;
			return;
		}
		float HealthPercent = (float)CurrentHealth / (float)MaxHealth;

		// ===== PHASE 2 =====
		if (CurrentPhase == 1 && HealthPercent <= Phase2Threshold)
		{
			CurrentPhase = 2;
			CurrentAttackSpeedMultiplier = Phase2AttackSpeedMultiplier;

			SetManualState(EBossState::Damaged);
			RestartAttackTimer();
		}

		// ===== PHASE 3 =====
		else if (CurrentPhase == 2 && HealthPercent <= Phase3Threshold)
		{
			CurrentPhase = 3;
			CurrentAttackSpeedMultiplier = Phase3AttackSpeedMultiplier;

			SetManualState(EBossState::Damaged);
			RestartAttackTimer();
		}

		if (CurrentPhase == 3)
		{
			AttacksBeforePatrol = 3;
		}
		


}


void ATFMBossGrabber::RestartAttackTimer()
{
	GetWorldTimerManager().ClearTimer(AttackTimer);

	GetWorldTimerManager().SetTimer(
		AttackTimer,
		this,
		&ATFMBossGrabber::ChooseRandomAttack,
		BaseAttackInterval / CurrentAttackSpeedMultiplier,
		true
	);
}


void ATFMBossGrabber::ChooseRandomAttack()
{
	if (CurrentState != EBossState::Idle)
		return;

	// enough attacks - patrol
	if (CurrentAttackCount >= AttacksBeforePatrol)
	{
		CurrentAttackCount = 0;
		SetManualState(EBossState::Searching);
		return;
	}

	SetManualState(EBossState::Attacking);

	int32 Random = FMath::RandRange(0, 1);

	EAttackType SelectedAttack = (Random == 0)
		? EAttackType::ForwardThrust
		: EAttackType::Sweep;

	PerformAttack(SelectedAttack);

	CurrentAttackCount++;
}



void ATFMBossGrabber::PerformAttack(EAttackType AttackType)
{
	CurrentAttack = AttackType;

	// Only variable set
	// The Animation Blueprint plays the animation

}

void ATFMBossGrabber::ActivateBossFight()
{
	RestartAttackTimer();

	if (BossHealthWidgetClass && !BossHealthWidget)
	{
		BossHealthWidget = CreateWidget<UUserWidget>(
			GetWorld(),
			BossHealthWidgetClass
		);

		if (BossHealthWidget)
		{
			BossHealthWidget->AddToViewport();
		}
	}
}




void ATFMBossGrabber::PLayerHit() {
	bHasHitPlayer = true;
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

void ATFMBossGrabber::OnPatrolFinished()
{
	if (CurrentState == EBossState::Searching)
	{
		SetManualState(EBossState::Idle);
	}
}

void ATFMBossGrabber::OnPhaseTransitionFinished()
{
	if (CurrentState == EBossState::Damaged)
	{
		SetManualState(EBossState::Idle);
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

	// if damaged, pause states
	if (state == EBossState::Damaged)
	{
		GetWorldTimerManager().PauseTimer(AttackTimer);
	}

	// if iddle, resume states
	if (state == EBossState::Idle)
	{
		GetWorldTimerManager().UnPauseTimer(AttackTimer);
	}
}

