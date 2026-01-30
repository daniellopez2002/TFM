#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TFMEnemy.generated.h"

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	Patrol,
	Charge,
	Attack
};

UCLASS()
class TFM_API ATFMEnemy : public APawn
{
	GENERATED_BODY()

public:
	ATFMEnemy();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// ----- STATE -----
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy State")
	EEnemyState CurrentState;

	// ----- STATE FUNCTIONS -----
	virtual void Patrol(float DeltaTime);
	virtual void Charge(float DeltaTime);
	virtual void Attack(float DeltaTime);

	virtual void ChangeState(EEnemyState NewState);
};
