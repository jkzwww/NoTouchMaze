// Fill out your copyright notice in the Description page of Project Settings.


#include "GoalTrigger.h"


AGoalTrigger::AGoalTrigger()
{
    //Register Events
    OnActorBeginOverlap.AddDynamic(this, &AGoalTrigger::OnOverlapBegin);
    OnActorEndOverlap.AddDynamic(this, &AGoalTrigger::OnOverlapEnd);
}

//draw trigger box for debugging
void AGoalTrigger::BeginPlay()
{
	Super::BeginPlay();

	//DrawDebugBox(GetWorld(), GetActorLocation(), GetActorScale() * 100, FColor::Turquoise, true, -1, 0, 5);

}


void AGoalTrigger::OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor)
{
    //if the overlapping actor is the specific actor we identified in the editor
    if (OtherActor && (OtherActor != this) && OtherActor == SpecificActor)
    {
        if (GEngine)
        {
            GEngine->ClearOnScreenDebugMessages();
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("Yayyy!!! you're safe now!!"));
        }

        UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Background, true);

    }
}

//not needed in our case as game will end after overlap
void AGoalTrigger::OnOverlapEnd(class AActor* OverlappedActor, class AActor* OtherActor)
{
    //if the overlapping actor is the specific actor we identified in the editor
    /*if (OtherActor && (OtherActor != this) && OtherActor == SpecificActor)
    {
        if (GEngine)
        {
            GEngine->ClearOnScreenDebugMessages();
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("Target left"));
        }
    }*/
}