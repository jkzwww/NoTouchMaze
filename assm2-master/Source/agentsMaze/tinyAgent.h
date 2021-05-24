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
#include "tinyAgent.generated.h"

UCLASS()
class AGENTSMAZE_API AtinyAgent : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AtinyAgent();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//static mesh
	UStaticMeshComponent* ConeVisual;
	
	//arrow component
	UPROPERTY(VisibleAnywhere)
		UArrowComponent* ForwardArrow;

	//materials
	UMaterial* StoredMaterial;

	UMaterialInstanceDynamic* DynamicMaterialInst;

	//checkpoint to move to
	int32 TargetCheckpoint;

	//array of checkpoints
	UPROPERTY(EditAnywhere)
		TArray<FVector> Checkpoints;
	
	//to add checkpoint to our checkpoint list
	void addCheckPoint(FVector givenCheckpoint);

	//travel speed
	UPROPERTY(EditAnywhere)
		float speed;

	//hit event
	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	//agent attack type 
	//0 - radius , 1 - bullet
	UPROPERTY(EditAnywhere)
		int AttackType;

	//agent hp
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int HP;

	//get agent health points
	UFUNCTION(BlueprintPure)
		int GetAgentHP() { return HP; };

};
