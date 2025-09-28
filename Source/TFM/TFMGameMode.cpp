// Copyright Epic Games, Inc. All Rights Reserved.

#include "TFMGameMode.h"
#include "TFMCharacter.h"
#include "UObject/ConstructorHelpers.h"

ATFMGameMode::ATFMGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
