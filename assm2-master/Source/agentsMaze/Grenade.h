// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/Material.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Components/SphereComponent.h"
#include "ParticleDefinitions.h"
#include "bigAgent.h"
#include "tinyAgent.h"
#include "Grenade.generated.h"

UCLASS()
class AGENTSMAZE_API AGrenade : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGrenade();

	//static mesh
	UStaticMeshComponent* VisibleComponent;

	//trigger component
	USphereComponent* TriggerSphere;

	//material
	UMaterial* StoredMaterial;

	UMaterialInstanceDynamic* DynamicMaterialInst;

	//effect
	//sound effect
	USoundBase* BombEffect;

	//particle effect
	UParticleSystem* ExplodeParticleSystem;

	// declare overlap begin function
	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//damage
	int damage;

	float startSecond;

	float currentSecond;

	float bombDelay;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
