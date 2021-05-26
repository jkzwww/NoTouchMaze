// Fill out your copyright notice in the Description page of Project Settings.
#include "tinyAgent.h"



// Sets default values
AtinyAgent::AtinyAgent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	VisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualRepresentation"));
	VisibleComponent->SetupAttachment(RootComponent);

	//arrow component
	ForwardArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	ForwardArrow->SetupAttachment(RootComponent);
	ForwardArrow->bHiddenInGame = false;
	
	//static mesh
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ConeVisualAsset(TEXT("/Game/StarterContent/Shapes/Shape_Cone.Shape_Cone"));

	if (ConeVisualAsset.Succeeded())
	{
		VisibleComponent->SetStaticMesh(ConeVisualAsset.Object);
	}

	//set material
	static ConstructorHelpers::FObjectFinder<UMaterial>FoundMaterial(TEXT("/Game/myContent/metallic.metallic"));

	if (FoundMaterial.Succeeded())
	{
		StoredMaterial = FoundMaterial.Object;
	}
	DynamicMaterialInst = UMaterialInstanceDynamic::Create(StoredMaterial, VisibleComponent);

	VisibleComponent->SetMaterial(0, DynamicMaterialInst);

	//collision set up
	VisibleComponent->SetNotifyRigidBodyCollision(true);
	VisibleComponent->BodyInstance.SetCollisionProfileName("BlockAllDynamic");
	VisibleComponent->OnComponentHit.AddDynamic(this, &AtinyAgent::OnHit);


	//default variables
	TargetCheckpoint = 0;
	speed = 2.5;
	DefSpeed = speed;
	stunTime = 0.5;
	startStunSec = 10000000;
}

// Called when the game starts or when spawned
void AtinyAgent::BeginPlay()
{
	Super::BeginPlay();


	//reset HP
	HP = 50;
	defHP = HP;
}

// Called every frame
void AtinyAgent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//update current second
	currentSecond = GetWorld()->UWorld::GetRealTimeSeconds();

	//get rotation direction
	FVector RotateDirection = Checkpoints[TargetCheckpoint] - GetActorLocation();

	FVector PitchDir = RotateDirection;
	
	FVector YawDir = RotateDirection;
	YawDir.Z = 0;
	YawDir.Normalize();

	//forward vector
	FVector Forward = FVector(1, 0, 0);

	/*
	//up vector
	FVector Up = FVector(0, 0, 1);

	//agent front
	FVector agentFront = GetActorForwardVector();
	PitchDir.X = agentFront.X;
	PitchDir.X = agentFront.Y;

	//get pitch degree
	float Dot = FVector::DotProduct(agentFront,PitchDir);
	float Det = agentFront.X * PitchDir.Y + agentFront.Y * PitchDir.X;
	float Rad = FMath::Atan2(Det, Dot);
	float Degrees = FMath::RadiansToDegrees(Rad);
	*/

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

	//HP under 0
	if (HP <= 0)
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("Enemy destroyed!!"));

		ACharacter* PlayerChar = UGameplayStatics::GetPlayerCharacter(this, 0);
		AagentsMazeCharacter* MyChar = Cast<AagentsMazeCharacter>(PlayerChar);

		MyChar->NumEnemy++;

		Destroy();
	}

	//stun effect over
	if (currentSecond - startStunSec > stunTime)
	{
		//resume speed
		speed = DefSpeed;

	}
	

}

void AtinyAgent::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
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
		//take damage
		HP -= bullet->Damage;

		//stun
		startStunSec = currentSecond;
		speed = 0;

		//update HP percent
		HPpercent = ((float)HP) / defHP;

		//update dynamic material base on hp percent
		DynamicMaterialInst->SetScalarParameterValue(FName("TinyAgentHP"), HPpercent);
	}
	else
	{
		/*
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("not char"));
		*/
	}



}

void AtinyAgent::addCheckPoint(FVector givenCheckpoint)
{
	Checkpoints.Add(givenCheckpoint);
}

