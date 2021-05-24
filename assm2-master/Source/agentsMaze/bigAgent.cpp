// Fill out your copyright notice in the Description page of Project Settings.
#include "bigAgent.h"

// Sets default values
AbigAgent::AbigAgent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	CapsuleVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualRepresentation"));
	CapsuleVisual->SetupAttachment(RootComponent);

	//arrow component
	ForwardArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	ForwardArrow->SetupAttachment(RootComponent);
	ForwardArrow->bHiddenInGame = false;

	//set static mesh
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CapsuleVisualAsset(TEXT("/Game/StarterContent/Shapes/Shape_NarrowCapsule.Shape_NarrowCapsule"));

	if (CapsuleVisualAsset.Succeeded())
	{
		CapsuleVisual->SetStaticMesh(CapsuleVisualAsset.Object);
	}

	//scale up as big agent
	CapsuleVisual->SetWorldScale3D(FVector(4, 4, 4));


	//set material
	static ConstructorHelpers::FObjectFinder<UMaterial>FoundMaterial(TEXT("/Game/myContent/glow.glow"));

	if (FoundMaterial.Succeeded())
	{
		StoredMaterial = FoundMaterial.Object;
	}
	DynamicMaterialInst = UMaterialInstanceDynamic::Create(StoredMaterial, CapsuleVisual);

	CapsuleVisual->SetMaterial(0, DynamicMaterialInst);

	//collision set up
	//CapsuleVisual->SetSimulatePhysics(true);
	CapsuleVisual->SetNotifyRigidBodyCollision(true);
	CapsuleVisual->BodyInstance.SetCollisionProfileName("BlockAllDynamic");
	CapsuleVisual->OnComponentHit.AddDynamic(this, &AbigAgent::OnHit);


	//default variables
	TargetCheckpoint = 0;
	speed = 1.5;
	
}

// Called when the game starts or when spawned
void AbigAgent::BeginPlay()
{
	Super::BeginPlay();
	
	//reset hp
	HP = 80;
}

// Called every frame
void AbigAgent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//get rotation direction
	FVector RotateDirection = Checkpoints[TargetCheckpoint] - GetActorLocation();

	//FVector PitchDir = RotateDirection;

	FVector YawDir = RotateDirection;
	YawDir.Z = 0;
	YawDir.Normalize();

	//forward vector
	FVector Forward = FVector(1, 0, 0);

	//get yaw degree
	float Dot2 = FVector::DotProduct(Forward, YawDir);
	float Det2 = Forward.X * YawDir.Y + Forward.Y * YawDir.X;
	float Rad2 = FMath::Atan2(Det2, Dot2);
	float Degrees2 = FMath::RadiansToDegrees(Rad2);

	//rotate
	FRotator Rotator(0, Degrees2, 0);
	FQuat RotationQuaternion = FQuat(Rotator);
	SetActorRotation(RotationQuaternion);

	//move
	RootComponent->SetWorldLocation(FMath::Lerp(this->GetActorLocation(), Checkpoints[TargetCheckpoint], DeltaTime * speed));

	//with lerp object never reach target , just approaching so distance check needed
	if (FVector::Dist(this->GetActorLocation(), Checkpoints[TargetCheckpoint]) < 2.0f)
	{
		TargetCheckpoint++;

		if (TargetCheckpoint == Checkpoints.Num())
		{
			TargetCheckpoint = 0;
		}
	}

	
	//when hp drops below 0 by damage
	if (HP <= 0)
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("Enemy destroyed!!"));

		//add kill to player
		ACharacter* PlayerChar = UGameplayStatics::GetPlayerCharacter(this, 0);
		AagentsMazeCharacter* MyChar = Cast<AagentsMazeCharacter>(PlayerChar);

		MyChar->NumEnemy++;

		Destroy();
	}
	
}


void AbigAgent::addCheckPoint(FVector givenCheckpoint)
{
	Checkpoints.Add(givenCheckpoint);
}


void AbigAgent::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

	if (Cast<AagentsMazeCharacter>(OtherActor)) // the cast returns nullptr when failed
	{
		// AActor can be casted into UObject therefor AActor is child of UObject
		if (GEngine)
		{
			GEngine->ClearOnScreenDebugMessages();
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("You're caught by the agent!!"));
		}
		
		UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
	}
	else if (Cast<AagentsMazeProjectile>(OtherActor))
	{
		AagentsMazeProjectile* bullet = Cast<AagentsMazeProjectile>(OtherActor);
		HP -= bullet->Damage;
	}
	else
	{
		/*
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("not char"));
		*/
	}

	/*
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("HIT"));
	}
	*/

}

/*
void AbigAgent::ReceiveHit(UPrimitiveComponent* MyComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	GLog->Log(*OtherActor->GetName());
}
*/