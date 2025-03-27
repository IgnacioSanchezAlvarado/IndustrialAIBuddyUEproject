// Copyright Epic Games, Inc. All Rights Reserved.

#include "Industrial_AI_buddyGameMode.h"
#include "Industrial_AI_buddyCharacter.h"
#include "UObject/ConstructorHelpers.h"

AIndustrial_AI_buddyGameMode::AIndustrial_AI_buddyGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/MetaHumans/Oskar/BP_Oskar"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
