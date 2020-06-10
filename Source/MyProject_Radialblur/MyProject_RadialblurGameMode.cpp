// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "MyProject_RadialblurGameMode.h"
#include "MyProject_RadialblurCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMyProject_RadialblurGameMode::AMyProject_RadialblurGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
