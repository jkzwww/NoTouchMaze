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

			//path 1
			if (numObject1 < maxObjPath1)
			{
				int myAgent = FMath::RandRange(0, 1);
				int attackStyle = FMath::RandRange(0, 1);

				if (!myAgent)
				{
					AtinyAgent* tempRef = GetWorld()->SpawnActor<AtinyAgent>(TinyObject, this->GetActorLocation(), FRotator::ZeroRotator);

					tempRef->AttackType = attackStyle;

					for (int i = 0; i < Path1.Num(); i++)
					{
						tempRef->addCheckPoint(Path1[i]);
					}

				}
				else
				{
					AbigAgent* tempRef2 = GetWorld()->SpawnActor<AbigAgent>(BigObject, this->GetActorLocation(), FRotator::ZeroRotator);

					tempRef2->AttackType = attackStyle;

					for (int i = 0; i < Path1.Num(); i++)
					{
						tempRef2->addCheckPoint(Path1[i]);
					}
				}

				numObject1++;
			}
		
			
			//path 2
			if (numObject2 < maxObjPath2)
			{
				int myAgent = FMath::RandRange(0, 1);
				int attackStyle = FMath::RandRange(0, 1);

				if (!myAgent)
				{
					AtinyAgent* tempRef = GetWorld()->SpawnActor<AtinyAgent>(TinyObject, this->GetActorLocation(), FRotator::ZeroRotator);

					tempRef->AttackType = attackStyle;

					for (int i = 0; i < Path2.Num(); i++)
					{
						tempRef->addCheckPoint(Path2[i]);
					}

				}
				else
				{
					AbigAgent* tempRef2 = GetWorld()->SpawnActor<AbigAgent>(BigObject, this->GetActorLocation(), FRotator::ZeroRotator);

					tempRef2->AttackType = attackStyle;

					for (int i = 0; i < Path2.Num(); i++)
					{
						tempRef2->addCheckPoint(Path2[i]);
					}
				}



				numObject2++;
			}
				

			//path 3
			if (numObject3 < maxObjPath3)
			{
				int myAgent = FMath::RandRange(0, 1);
				int attackStyle = FMath::RandRange(0, 1);

				if (!myAgent)
				{
					AtinyAgent* tempRef = GetWorld()->SpawnActor<AtinyAgent>(TinyObject, this->GetActorLocation(), FRotator::ZeroRotator);

					tempRef->AttackType = attackStyle;

					for (int i = 0; i < Path3.Num(); i++)
					{
						tempRef->addCheckPoint(Path3[i]);
					}

				}
				else
				{
					AbigAgent* tempRef2 = GetWorld()->SpawnActor<AbigAgent>(BigObject, this->GetActorLocation(), FRotator::ZeroRotator);

					tempRef2->AttackType = attackStyle;

					for (int i = 0; i < Path3.Num(); i++)
					{
						tempRef2->addCheckPoint(Path3[i]);
					}
				}

				numObject3++;
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