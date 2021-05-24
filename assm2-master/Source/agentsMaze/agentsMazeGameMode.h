// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "agentsMazeGameMode.generated.h"

UCLASS(minimalapi)
class AagentsMazeGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AagentsMazeGameMode();

	UPROPERTY(EditAnywhere)
		float ElapsedTime;

	UFUNCTION(BlueprintPure)
		int32 GetElapsedTime() { return int32(ElapsedTime); };

	virtual void Tick(float DeltaTime) override;

	//reset timer
	UFUNCTION(BlueprintCallable)
		void ResetElapsedTime() { ElapsedTime = 0.0f; }

};



