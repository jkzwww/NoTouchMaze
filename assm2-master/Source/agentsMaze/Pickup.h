// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Materials/Material.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "agentsMazeCharacter.h"
#include "Pickup.generated.h"

UCLASS()
class AGENTSMAZE_API APickup : public AActor
{
	GENERATED_BODY()
	

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* VisibleComponent;

	UPROPERTY(EditAnywhere)
		int PickupType; //0 - speed , 1 - health , 2 - shield

	UPROPERTY(EditAnywhere)
		UMaterial* SpeedMaterial;

	UPROPERTY(EditAnywhere)
		UMaterial* HealthMaterial;

	UPROPERTY(EditAnywhere)
		UMaterial* ShieldMaterial;

	UPROPERTY(EditAnywhere)
		UMaterial* CoinMaterial;

	//player target
	AagentsMazeCharacter* Target;
	float WalkSpeed;

	//track current time
	float currentSecond;

	//track start time of effects
	float startSpeedSec;
	float startShieldSec;

	//effect time
	UPROPERTY(EditAnywhere)
		float EffectTime = 5;

	//currency value
	int myValue;

public:	
	// Sets default values for this actor's properties
	APickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	//hit event
	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	//sound effect
	USoundBase* CollectSound;

	//particle effect
	UPROPERTY(EditAnywhere, Category = Effect)
		UParticleSystem* PickedUpParticle;

};
