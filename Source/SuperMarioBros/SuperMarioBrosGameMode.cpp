// Copyright Epic Games, Inc. All Rights Reserved.

#include "SuperMarioBrosGameMode.h"
#include "SuperMarioBrosCharacter.h"

ASuperMarioBrosGameMode::ASuperMarioBrosGameMode()
{
	// Set default pawn class to our character
	DefaultPawnClass = ASuperMarioBrosCharacter::StaticClass();	
}
