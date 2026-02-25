#include "NutCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

ANutCharacter::ANutCharacter()
{
	IsRolling = false;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input

void ANutCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void ANutCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Attack 
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ANutCharacter::Attack);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ANutCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ANutCharacter::Look);

		//Rolling
		EnhancedInputComponent->BindAction(RollAction, ETriggerEvent::Started, this, &ANutCharacter::Roll);

		//Rolling
		EnhancedInputComponent->BindAction(ChangeFocusAction, ETriggerEvent::Triggered, this, &ANutCharacter::ChangeFocus);
		//EnhancedInputComponent->BindAction(RollAction, ETriggerEvent::Completed, this, &ANutCharacter::EndsRoll);
	}
}

// Movement Input
void ANutCharacter::Move(const FInputActionValue& Value)
{
	if (IsRolling || IsStuned)
		return;

	FVector2D MovementVector = Value.Get<FVector2D>();
	if (Controller == nullptr || MovementVector.IsNearlyZero())
	{
		return;
	}

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector DesiredDir = (FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X) * MovementVector.X) +
		(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y) * -MovementVector.Y);

	FRotator TaregtRot = DesiredDir.Rotation();
	FRotator SmothRot = FMath::RInterpTo(GetActorRotation(), TaregtRot, GetWorld()->GetDeltaSeconds(), 15.f);	

	if (ActorsFocus.IsEmpty() || IsAttacking)
	{
		SetActorRotation(SmothRot);
	}

	if(!IsAttacking)
		AddMovementInput(DesiredDir.GetSafeNormal());
}


void ANutCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ANutCharacter::Attack(const FInputActionValue& Value)
{
	if (IsRolling)
		return;

	if (IsAttacking)
	{
		bComboInputQueued = true;
		return;
	}

	StartCombo();
}


// God Mode Activation/Deactivation
void ANutCharacter::ActivateGodMode()
{
	//This is gonna be called in animation notify
	// Here we gonna activate god mode for the character to avoid damage 
	// 	
}

void ANutCharacter::DeactivateGodMode()
{
	//This is gonna be called in animation notify
	// Here we gonna deactivate god mode for the character to receive damage again
	//
}

// Rolling
void ANutCharacter::Roll(const FInputActionValue& Value)
{
	if (IsRolling || IsAttacking)
		return;

	IsRolling = true;
	ActivateGodMode();

	// Dirección del dash (forward del personaje, solo plano horizontal)
	FVector Forward = GetActorForwardVector();
	Forward.Z = 0.f;
	Forward.Normalize();

	// Configuración del dash
	DashDirection = Forward;
	DistanceTraveled = 0.f;
	bDashFinished = false;

	// (Opcional) Aquí podrías spawnear un efecto de partícula que siga al personaje
	// SpawnRollTrail(GetActorLocation(), GetActorLocation() + Forward * RollDistance);
}

void ANutCharacter::EndRoll()
{
	IsRolling = false;
	DeactivateGodMode();
}

void ANutCharacter::SpawnRollTrail(FVector Start, FVector End)
{
	/*if (!RollTrailFX)
		return;

	UNiagaraComponent* Niagara =
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			RollTrailFX,
			Start
		);

	if (Niagara)
	{
		Niagara->SetVectorParameter("EndPoint", End);
	}*/
}

// Checkpoint System
void ANutCharacter::SetCheckpoint(FVector location)
{
	CheckpointLocation = location;
}

void ANutCharacter::Respawn_Implementation()
{
	// Teleport the character to the checkpoint location
	SetActorLocation(CheckpointLocation);	
}

void ANutCharacter::HandleFocus(float DeltaTime)
{
	if (ActorsFocus.IsEmpty())
		return;

	FVector ToTarget =
		ActorsFocus[FocusIndex]->GetActorLocation() - GetActorLocation();

	ToTarget.Z = 0.f;

	FRotator TargetRot = ToTarget.Rotation();
	FRotator NewRot = FMath::RInterpTo(
		GetActorRotation(),
		TargetRot,
		DeltaTime,
		FocusInterpSpeed
	);

	SetActorRotation(NewRot);
}

void ANutCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IsStuned) {
		StunerCounter++;
		HandleStuned();
		if (StunerCounter >= StunedTimer)
		{
			IsStuned = false;
			StunerCounter = 0.0f;
		}
		return;
	}

	// --- Movimiento suave del dash ---
	if (IsRolling && !bDashFinished)
	{
		// Velocidad del dash = distancia total / duración
		float DashSpeed = RollDistance / RollDuration;
		float Step = DashSpeed * DeltaTime;

		FVector DesiredMovement = DashDirection * Step;
		FHitResult Hit;

		// Movemos con detección de colisión (sweep)
		AddActorWorldOffset(DesiredMovement, true, &Hit);

		// Actualizamos la distancia realmente recorrida (puede ser menor si chocó)
		float RealStep = Hit.bBlockingHit ? Hit.Distance : Step;
		DistanceTraveled += RealStep;

		// Si chocamos o alcanzamos la distancia máxima, finalizamos el dash
		if (Hit.bBlockingHit || DistanceTraveled >= RollDistance)
		{
			bDashFinished = true;

			// Iniciamos el cooldown (tiempo de recuperación)
			GetWorldTimerManager().SetTimer(
				RollTimerHandle,
				this,
				&ANutCharacter::EndRoll,
				RollCooldown,
				false
			);
		}
	}

	if (!IsRolling && !ActorsFocus.IsEmpty())
	{
		HandleFocus(DeltaTime);
	}
}

void ANutCharacter::StartCombo()
{
	if (IsAttacking || !AttackMontage)
		return;

	IsAttacking = true;
	ComboIndex = 0;

	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->Montage_Play(AttackMontage);
		AnimInstance->Montage_JumpToSection(TEXT("Attack_1"), AttackMontage);
	}
}

void ANutCharacter::AdvanceCombo()
{
	if (!bComboInputQueued)
	{
		EndCombo();
		return;
	}

	bComboInputQueued = false;
	ComboIndex++;

	if (ComboIndex >= MaxCombo)
	{
		EndCombo();
		return;
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance || !AttackMontage)
		return;

	FName SectionName = FName(*FString::Printf(TEXT("Attack_%d"), ComboIndex + 1));
	AnimInstance->Montage_JumpToSection(SectionName, AttackMontage);
}

void ANutCharacter::EndCombo()
{
	IsAttacking = false;
	bComboInputQueued = false;
	ComboIndex = 0;

	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->Montage_Stop(0.15f, AttackMontage);
	}
}

void ANutCharacter::RemoveActorFromFocus(AActor* actor)
{
	if (!ActorsFocus.Contains(actor))
		return;

	ActorsFocus.Remove(actor);
	FocusIndex = 0;
}

void ANutCharacter::ChangeFocus(const FInputActionValue& Value)
{

	if (ActorsFocus.IsEmpty())
	{
		FocusIndex = 0;
		return;
	}

	float Input = Value.Get<float>();

	if (Input > 0.f)
	{
		int32 Target = FocusIndex + 1;

		if (Target >= ActorsFocus.Num())
		{
			FocusIndex = 0;
		}
		else
		{
			FocusIndex = Target;
		}
	}
	else if (Input < 0.f)
	{
		int32 Target = FocusIndex - 1;

		if (Target < 0)
		{
			FocusIndex = ActorsFocus.Num() - 1;
		}
		else
		{
			FocusIndex = Target;
		}
	}
}