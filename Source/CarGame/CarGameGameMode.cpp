// Copyright Epic Games, Inc. All Rights Reserved.

#include "CarGameGameMode.h"
#include "CarGameCharacter.h"
#include "UObject/ConstructorHelpers.h"

ACarGameGameMode::ACarGameGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
