// Fill out your copyright notice in the Description page of Project Settings.

#include "myGoal.h"

// Sets default values
AmyGoal::AmyGoal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	CubeVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualRepresentation"));
	CubeVisual->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeVisualAsset(TEXT("/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube"));

	if (CubeVisualAsset.Succeeded())
	{
		CubeVisual->SetStaticMesh(CubeVisualAsset.Object);
	}

	//overlap event set up
	CubeVisual->SetGenerateOverlapEvents(true);
	CubeVisual->OnComponentBeginOverlap.AddDynamic(this, &AmyGoal::BeginOverlap);

}

// Called when the game starts or when spawned
void AmyGoal::BeginPlay()
{
	Super::BeginPlay();
	
	//CubeVisual->OnComponentBeginOverlap.AddDynamic(this, &AmyGoal::BeginOverlap);
}

// Called every frame
void AmyGoal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


//when overlap
void AmyGoal::BeginOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (Cast<AagentsMazeCharacter>(OtherActor)) // the cast returns nullptr when failed
	{
		// AActor can be casted into UObject therefor AActor is child of UObject
		if (GEngine)
		{
			GEngine->ClearOnScreenDebugMessages();
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("You're safe now!!"));
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("Not target"));
	}
}
