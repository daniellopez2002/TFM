#include "TFMBossGrabber.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/ActorComponent.h"
#include "Components/BoxComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"




ATFMBossGrabber::ATFMBossGrabber()
{



	PrimaryActorTick.bCanEverTick = true;

	// Root
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	// Skeletal Mesh
	BossMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BossMesh"));
	BossMesh->SetupAttachment(RootComponent);

	// Hitbox Attack
	AttackHitbox = CreateDefaultSubobject<UBoxComponent>(TEXT("AttackHitbox"));
	AttackHitbox->SetupAttachment(BossMesh, TEXT("AttackSocket"));

	AttackHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttackHitbox->SetCollisionObjectType(ECC_WorldDynamic);
	AttackHitbox->SetCollisionResponseToAllChannels(ECR_Ignore);
	AttackHitbox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	StateAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("StateAudio"));
	StateAudioComponent->SetupAttachment(RootComponent);
	StateAudioComponent->bAutoActivate = false;
	StateAudioComponent->OnAudioFinished.AddDynamic(this, &ATFMBossGrabber::OnStateSoundFinished);

}

void ATFMBossGrabber::BeginPlay()
{
	Super::BeginPlay();

	IsBattleStarted = false;

	HealthPercent = 1.0;


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

void ATFMBossGrabber::CheckPhaseTransition(int CurrentHealth, int MaxHealth) {

		if (CurrentState == EBossState::Dead)
			return;

		if (CurrentHealth == 0) {
			Death();
			return;
		}
		HealthPercent = (float)CurrentHealth / (float)MaxHealth;

		// ===== PHASE 2 =====
		if (CurrentPhase == 1 && HealthPercent <= Phase2Threshold)
		{
			CurrentAttackCount = 0;
			CurrentPhase = 2;
			CurrentAttackSpeedMultiplier = Phase2AttackSpeedMultiplier;

			SetManualState(EBossState::Damaged);
			RestartAttackTimer();
		}

		// ===== PHASE 3 =====
		else if (CurrentPhase == 2 && HealthPercent <= Phase3Threshold)
		{
			CurrentAttackCount = 0;
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

	if (IsBattleStarted != true)
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

void ATFMBossGrabber::Death() {
	CurrentState = EBossState::Dead;
	HealthPercent = 0;
	BossHealthWidget->RemoveFromViewport();
}

void ATFMBossGrabber::PerformAttack(EAttackType AttackType)
{
	CurrentAttack = AttackType;

	// Only variable set
	// The Animation Blueprint plays the animation

}

void ATFMBossGrabber::ActivateBossFight_Implementation()
{
	IsBattleStarted = true;
	UpdateStateAudio();
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

void ATFMBossGrabber::UpdateStateAudio()
{
	if (!StateAudioComponent) return;
	if (IsBattleStarted == false) return;

	USoundBase* NewSound = nullptr;

	switch (CurrentState)
	{
	case EBossState::Idle:
		NewSound = PatrolSound;
		break;
	case EBossState::Searching:
		NewSound = PatrolSound;
		break;

	case EBossState::Attacking:
		NewSound = AttackSound;
		break;

	case EBossState::Damaged:
		NewSound = DamageSound;
		break;

	case EBossState::Dead:
		NewSound = DeathSound;
		break;
	}

	if (NewSound)
	{
		StateAudioComponent->SetSound(NewSound);
		StateAudioComponent->Play();
	}
}

void ATFMBossGrabber::OnStateSoundFinished()
{
	if (CurrentState == EBossState::Dead)
		return;

	UpdateStateAudio(); // vuelve a elegir sonido según estado
}

float ATFMBossGrabber::GetHealthPercent() const
{
	return HealthPercent;
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
	SetManualState(EBossState::Idle);
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

	UpdateStateAudio(); 

	if (state == EBossState::Damaged)
	{
		GetWorldTimerManager().PauseTimer(AttackTimer);
	}

	if (state == EBossState::Idle)
	{
		GetWorldTimerManager().UnPauseTimer(AttackTimer);
	}
}

