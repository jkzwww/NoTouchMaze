// Copyright Epic Games, Inc. All Rights Reserved.

#include "agentsMazeGameMode.h"
#include "agentsMazeHUD.h"
#include "agentsMazeCharacter.h"
#include "UObject/ConstructorHelpers.h"

AagentsMazeGameMode::AagentsMazeGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AagentsMazeHUD::StaticClass();

	PrimaryActorTick.bCanEverTick = true;

	//default value
	ElapsedTime = 0.0f;

}

void AagentsMazeGameMode::Tick(float DeltaTime)
{
	//count
	ElapsedTime += DeltaTime;
}
