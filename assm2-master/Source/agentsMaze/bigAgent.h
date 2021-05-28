// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "Materials/Material.h"
#include "agentsMazeCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Components/SphereComponent.h"
#include "DrawDebugHelpers.h"
#include "agentsMazeProjectile.h"
#include "bigAgent.generated.h"

UCLASS()
class AGENTSMAZE_API AbigAgent : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AbigAgent();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//static mesh
	UStaticMeshComponent* VisibleComponent;

	//store mesh
	UStaticMesh* ConeMesh;
	UStaticMesh* CapsuleMesh;

	//arrow component
	UPROPERTY(VisibleAnywhere)
		UArrowComponent* ForwardArrow;

	//trigger component
	USphereComponent* TriggerSphere;


	//material
	UMaterial* StoredMaterial;

	UMaterialInstanceDynamic* DynamicMaterialInst;

	//checkpoint to move to
	int TargetCheckpoint;

	//array of checkpoints
	UPROPERTY(EditAnywhere)
		TArray<FVector> Checkpoints;

	//add checkpoint to array
	void addCheckPoint(FVector givenCheckpoint);

	//travel speed
	UPROPERTY(EditAnywhere)
		float speed;

	//default speed;
	float DefSpeed;

	//agent attack type
		//0 - radius , 1 - bullet
	UPROPERTY(EditAnywhere, Category = Attack)
		int AttackType;

	//agent radial damage
	int MinDamage;
	int MaxDamage;
	int AttackFreq;
	float AttackRadius;

	//agent hp
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int HP;

	//default HP
	int defHP;

	//HP percent
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float HPpercent;

	//get agent health points
	UFUNCTION(BlueprintPure)
		int GetAgentHP() { return HP; };

	//track current time
	float currentSecond;

	//track start time of effects
	float startStunSec;

	//stun time
	UPROPERTY(EditAnywhere)
		float stunTime;

	//hit event
	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	//overlap events

	// declare overlap begin function
	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// declare overlap end function
	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//track attack
	bool startRadialAttack;
	bool startShooting;

	float attackInterval;
	float lastAttackSec;
	float myDistance;
	AagentsMazeCharacter* myPlayer;

	/** Projectile class to spawn */
	UPROPERTY(EditAnywhere, Category = Projectile)
		TSubclassOf<class AagentsMazeProjectile> myProjectile;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		USoundBase* FireSound;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		FVector GunOffset;

	void Shoot(AagentsMazeCharacter* target);
};
