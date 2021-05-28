// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TextRenderComponent.h"
#include "Math/UnrealMathUtility.h"
#include "tinyAgent.h"
#include "bigAgent.h"
#include "TimerManager.h"
#include "AgentBullet.h"
#include "Kismet/GameplayStatics.h"
#include "Spawner.generated.h"

UCLASS()
class AGENTSMAZE_API ASpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//interval
	UPROPERTY(EditAnywhere)
		float SpawnInterval;
	
	//time to start in seconds
	UPROPERTY(EditAnywhere)
		int32 StartTime;

	//Object to spawn into the world
	UPROPERTY(EditAnywhere, Category = "SpawnObject")
		TSubclassOf<AActor> TinyObject;

	UPROPERTY(EditAnywhere, Category = "SpawnObject")
		TSubclassOf<AActor> BigObject;

	//Path checkpoints and max no of agents	
	UPROPERTY(EditAnywhere, Category = "Path1")
		int maxObjPath1;

	UPROPERTY(EditAnywhere, Category = "Path1")
		TArray<FVector> Path1;


	UPROPERTY(EditAnywhere, Category = "Path2")
		int maxObjPath2;

	UPROPERTY(EditAnywhere, Category = "Path2")
		TArray<FVector> Path2;


	UPROPERTY(EditAnywhere, Category = "Path3")
		int maxObjPath3;
	
	UPROPERTY(EditAnywhere, Category = "Path3")
		TArray<FVector> Path3;


	//Arrays of path data objects
	TArray<int>AllPathMaxObj;

	TArray<TArray<FVector>> AllPathPoints;

	TArray<int>AllPathNumObj; 


	//Variables to handle spawning
	bool StartSpawning = false;
	float SpawnCountdown;

	//keep track of current objects spawned
	int numObject1;
	int numObject2;
	int numObject3;

	//0 for small size, 1 for large size
	int spawnType;

	//Text Countdown
	UTextRenderComponent* StartText;

	//Countdown Functions
	void UpdateTimerDisplay();

	void AdvanceTimer();

	void CountdownHasFinished();

	//Timer Handle
	FTimerHandle StartTimerHandle;

	//agent projectile
	UPROPERTY(EditAnywhere, Category = Shoot)
		TSubclassOf<class AAgentBullet> ProjectileClass;

	//agent shooting 
	UPROPERTY(EditAnywhere, Category = Shoot)
		float FireRate;

	//agent radial damage
	UPROPERTY(EditAnywhere, Category = Attack)
		int MinDamage;

	UPROPERTY(EditAnywhere, Category = Attack)
		int MaxDamage;

	UPROPERTY(EditAnywhere, Category = Attack)
		int AttackFreq;

	UPROPERTY(EditAnywhere, Category = Attack)
		float AttackRadius;

	UPROPERTY(EditAnywhere, Category = Effect)
		UParticleSystem* ExplosionParticle;

	UPROPERTY(EditAnywhere, Category = Effect)
		UParticleSystem* SpawnParticle;

	//sound effect
	USoundBase* SpawnSound;
};
