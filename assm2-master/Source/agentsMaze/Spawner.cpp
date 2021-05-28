// Fill out your copyright notice in the Description page of Project Settings.

#include "Spawner.h"


// Sets default values
ASpawner::ASpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StartText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("StartNumber"));
	StartText->SetHorizontalAlignment(EHTA_Center);
	StartText->SetWorldSize(150.0f);
	RootComponent = StartText;

	//default values
	SpawnCountdown = 0;
	numObject1 = 0;
	numObject2 = 0;
	numObject3 = 0;

	SpawnInterval = 5;
	StartTime = 2;


}

// Called when the game starts or when spawned
void ASpawner::BeginPlay()
{
	Super::BeginPlay();
	UpdateTimerDisplay();
	GetWorldTimerManager().SetTimer(StartTimerHandle, this, &ASpawner::AdvanceTimer, 1.0f, true);


	//Arrays of path data objects
	AllPathMaxObj = { maxObjPath1,maxObjPath2,maxObjPath3 };

	AllPathPoints = { Path1,Path2,Path3 };

	AllPathNumObj = { numObject1,numObject2,numObject3 };
}

// Called every frame
void ASpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (StartSpawning)
	{
		SpawnCountdown -= DeltaTime;
		if (SpawnCountdown <= 0)
		{

			//for each path (in parallel)
				//check num obj path x
				//randomly choose agent type per spawntime

				//spawn
				//add checkpoints

			
			//loop version, warning : SpawnActor failed because no class was specified
			for (int i = 0; i < 3; i++)
			{

				if (AllPathNumObj[i] < AllPathMaxObj[i])
				{
					int myAgent = FMath::RandRange(0, 1);
					int attackStyle = FMath::RandRange(0, 1);

					if (!myAgent)
					{
						AtinyAgent* tempRef = GetWorld()->UWorld::SpawnActor<AtinyAgent>(AtinyAgent::StaticClass(), this->GetActorLocation(), FRotator::ZeroRotator);

						tempRef->AttackType = attackStyle;

						for (int j = 0; j < AllPathPoints[i].Num(); j++)
						{
							tempRef->addCheckPoint(AllPathPoints[i][j]);
						}

					}
					else
					{
						AbigAgent* tempRef2 = GetWorld()->UWorld::SpawnActor<AbigAgent>(AbigAgent::StaticClass(), this->GetActorLocation(), FRotator::ZeroRotator);

						tempRef2->AttackType = attackStyle;
						tempRef2->myProjectile = ProjectileClass;

						tempRef2->MinDamage = MinDamage;
						tempRef2->MaxDamage = MaxDamage;
						tempRef2->AttackFreq = AttackFreq;
						tempRef2->AttackRadius = AttackRadius;

						for (int k = 0; k < AllPathPoints[i].Num(); k++)
						{
							tempRef2->addCheckPoint(AllPathPoints[i][k]);
						}
					}

					AllPathNumObj[i]++;
				}
			}

			//reset countdown
			SpawnCountdown = SpawnInterval;
			
		}

	}
}

void ASpawner::UpdateTimerDisplay()
{
	StartText->SetText(FString::FromInt(FMath::Max(StartTime, 0)));
}

void ASpawner::AdvanceTimer()
{
	--StartTime;
	UpdateTimerDisplay();

	if (StartTime < 1)
	{
		//completed counting down, stop timer handle
		GetWorldTimerManager().ClearTimer(StartTimerHandle);

		//Perform actions after time finished
		CountdownHasFinished();
	}
}

void ASpawner::CountdownHasFinished()
{
	//change readout
	StartText->SetText(TEXT("RUN!!!"));
	StartSpawning = true;
}