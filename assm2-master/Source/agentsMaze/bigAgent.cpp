// Fill out your copyright notice in the Description page of Project Settings.
#include "bigAgent.h"

// Sets default values
AbigAgent::AbigAgent()
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

	//sphere trigger component
	TriggerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerSphere"));
	TriggerSphere->InitSphereRadius(200.f);
	TriggerSphere->SetCollisionProfileName(TEXT("Trigger"));
	TriggerSphere->SetupAttachment(RootComponent);

	TriggerSphere->OnComponentBeginOverlap.AddDynamic(this, &AbigAgent::OnOverlapBegin);
	TriggerSphere->OnComponentEndOverlap.AddDynamic(this, &AbigAgent::OnOverlapEnd);



	//set static mesh
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CapsuleVisualAsset(TEXT("/Game/StarterContent/Shapes/Shape_NarrowCapsule.Shape_NarrowCapsule"));

	if (CapsuleVisualAsset.Succeeded())
	{
		VisibleComponent->SetStaticMesh(CapsuleVisualAsset.Object);
	}

	//scale up as big agent
	VisibleComponent->SetWorldScale3D(FVector(4, 4, 4));


	//set material
	static ConstructorHelpers::FObjectFinder<UMaterial>FoundMaterial(TEXT("/Game/myContent/glow.glow"));

	if (FoundMaterial.Succeeded())
	{
		StoredMaterial = FoundMaterial.Object;
	}
	DynamicMaterialInst = UMaterialInstanceDynamic::Create(StoredMaterial, VisibleComponent);

	VisibleComponent->SetMaterial(0, DynamicMaterialInst);

	//collision set up
	//CapsuleVisual->SetSimulatePhysics(true);
	VisibleComponent->SetNotifyRigidBodyCollision(true);
	VisibleComponent->BodyInstance.SetCollisionProfileName("BlockAllDynamic");
	VisibleComponent->OnComponentHit.AddDynamic(this, &AbigAgent::OnHit);


	//default variables
	TargetCheckpoint = 0;
	speed = 1.5;
	DefSpeed = speed;
	stunTime = 0.3;
	startStunSec = 10000000;
	MinDamage = 3;
	MaxDamage = 12;
	AttackRadius = 200;
	startRadialAttack = false;
	AttackFreq = 3;
	attackInterval = 1.0 / AttackFreq;
	lastAttackSec = 0;

}

// Called when the game starts or when spawned
void AbigAgent::BeginPlay()
{
	Super::BeginPlay();
	
	//reset hp
	HP = 80;
	defHP = HP;

}

// Called every frame
void AbigAgent::Tick(float DeltaTime)
{

	//update current second
	currentSecond = GetWorld()->UWorld::GetRealTimeSeconds();

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
	
	//stun effect over
	if (currentSecond - startStunSec > stunTime)
	{
		//resume speed
		speed = DefSpeed;

	}

	//radial attack
	if (startRadialAttack)
	{
		if (myPlayer)
		{
			if (currentSecond - lastAttackSec > attackInterval)
			{
				myDistance = FVector::Dist(GetActorLocation(), myPlayer->GetActorLocation());

				float myDamage = ((AttackRadius - myDistance) / AttackRadius) * MaxDamage;

				if (myDamage < MinDamage) { myDamage = MinDamage; }

				myPlayer->HP -= int(myDamage);

				lastAttackSec = currentSecond;
			}

		}

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
		
		//UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
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
		DynamicMaterialInst->SetScalarParameterValue(FName("AgentHP"), HPpercent);
	}
	else
	{
		/*
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("not char"));
		*/
	}


}

/*
void AbigAgent::ReceiveHit(UPrimitiveComponent* MyComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	GLog->Log(*OtherActor->GetName());
}
*/

void AbigAgent::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this))// && OtherComp)
	{
		if (Cast<AagentsMazeCharacter>(OtherActor))
		{
			if (GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("attack triggered"));

			myPlayer = Cast<AagentsMazeCharacter>(OtherActor);
		
			if (!startRadialAttack) 
			{ 
				startRadialAttack = true; 
				lastAttackSec = currentSecond;
			}
			
		}


		
	}
}

void AbigAgent::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && (OtherActor != this))// && OtherComp)
	{
		if (Cast<AagentsMazeCharacter>(OtherActor))
		{
			if (GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("stop attack"));

			startRadialAttack = false;
		}
	}
}