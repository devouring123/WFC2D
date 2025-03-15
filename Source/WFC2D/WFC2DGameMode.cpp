// Copyright Epic Games, Inc. All Rights Reserved.

#include "WFC2DGameMode.h"
#include "WFC2DCharacter.h"
#include "UObject/ConstructorHelpers.h"

AWFC2DGameMode::AWFC2DGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
