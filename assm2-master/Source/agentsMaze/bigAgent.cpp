// Fill out your copyright notice in the Description page of Project Settings.
#include "bigAgent.h"

// Sets default values
AbigAgent::AbigAgent()
{
	//default attack radius
	AttackRadius = 400;

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
	TriggerSphere->InitSphereRadius(AttackRadius);
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

	//sound effect

	static ConstructorHelpers::FObjectFinder<USoundBase> ExplosionSoundAsset(TEXT("/Game/myContent/Bang.Bang"));

	if (ExplosionSoundAsset.Succeeded())
	{
		BangEffect = ExplosionSoundAsset.Object;
	}

	//particle effect
	/*
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ExplosionPartAsset(TEXT("/Game/StarterContent/Particles/P_Explosion.P_Explosion"));

	if (ExplosionPartAsset.Succeeded())
	{
		ExplodeParticleSystem = ExplosionPartAsset.Object;
	}
	*/

	//default variables
	TargetCheckpoint = 0;
	speed = 1.5;
	DefSpeed = speed;
	
	stunTime = 0.3;
	startStunSec = 10000000;
	
	MinDamage = 3;
	MaxDamage = 12;
	AttackFreq = 3;

	startRadialAttack = false;
	attackInterval = 1.0 / AttackFreq;
	lastAttackSec = 0;

	startShooting = false;
	fireInterval = 1.0 / fireRate;

	GunOffset = FVector(50.0f, 0.0f, 10.0f);

	myValue = 15;
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
		MyChar->MyCoins += myValue;

		UGameplayStatics::PlaySoundAtLocation(this, BangEffect, GetActorLocation(), 1.0F, 1.0F, 0.0F, nullptr, nullptr);
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplodeParticleSystem, GetActorLocation(),FRotator::ZeroRotator, true);
		Destroy();
	}
	
	//stun effect over
	if (currentSecond - startStunSec > stunTime)
	{
		//resume speed
		speed = DefSpeed;

	}

	if (myPlayer)
	{
		myDistance = FVector::Dist(GetActorLocation(), myPlayer->GetActorLocation());
	}

	if (myGrenade)
	{

		if (myGrenade->isExplode)
		{
			UGameplayStatics::PlaySoundAtLocation(this, BangEffect, GetActorLocation(), 1.0F, 1.0F, 0.0F, nullptr, nullptr);
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplodeParticleSystem, GetActorLocation(), FRotator::ZeroRotator, true);
			Destroy();
		}

	}
	
	
	//attack by type
	if (AttackType)
	{
	
		if (startShooting)
		{
			//get rotation direction
			FVector TargetRotateDirection = myPlayer->GetActorLocation() - GetActorLocation();

			FVector TYawDir = TargetRotateDirection;
			TYawDir.Z = 0;
			TYawDir.Normalize();

			//get yaw degree
			float TDot = FVector::DotProduct(Forward, TYawDir);
			float TDet = Forward.X * TYawDir.Y + Forward.Y * TYawDir.X;
			float TRad = FMath::Atan2(TDet, TDot);
			float TDeg = FMath::RadiansToDegrees(TRad);

			//rotate
			FRotator TRotator(0, TDeg, 0);
			FQuat TRotationQuaternion = FQuat(TRotator);
			SetActorRotation(TRotationQuaternion);

			if (myPlayer)
			{
				if (currentSecond - lastAttackSec > fireInterval)
				{
					int myDamage = ((AttackRadius - myDistance) / AttackRadius) * MaxDamage;
					myDamage *= myPlayer->armorFactor;

					if (myDamage < MinDamage) { myDamage = MinDamage; }

					Shoot(myDamage);

					lastAttackSec = currentSecond;
				}

				if (myPlayer->HP <= 0)
				{
					startShooting = false;
				}
			}
		}
		
	}
	else
	{
		//radial attack
		if (startRadialAttack)
		{
			if (myPlayer)
			{
				if (currentSecond - lastAttackSec > attackInterval)
				{
					int myDamage = ((AttackRadius - myDistance) / AttackRadius) * MaxDamage;
					myDamage *= myPlayer->armorFactor;

					if (myDamage < MinDamage) { myDamage = MinDamage; }

					myPlayer->HP -= myDamage;

					lastAttackSec = currentSecond;
				}

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
			//if (GEngine)
				//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("attack triggered"));

			myPlayer = Cast<AagentsMazeCharacter>(OtherActor);
			
			if (AttackType)
			{
				if (!startShooting && myPlayer->HP > 0)
				{
					startShooting = true;
					lastAttackSec = currentSecond;
				}
			}
			else
			{
				if (!startRadialAttack)
				{
					startRadialAttack = true;
					lastAttackSec = currentSecond;
				}

			}
	
		}
		else if (Cast<AGrenade>(OtherActor))
		{
			
			myGrenade = Cast<AGrenade>(OtherActor);

		}


		
	}
}

void AbigAgent::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && (OtherActor != this))// && OtherComp)
	{
		if (Cast<AagentsMazeCharacter>(OtherActor))
		{
			//if (GEngine)
				//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("stop attack"));

			startRadialAttack = false;
			startShooting = false;
		}
	}
}

void AbigAgent::Shoot(int myDamage)
{

	// try and fire a projectile
	if (myProjectile != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			const FRotator SpawnRotation = GetActorRotation();

			const FVector SpawnLocation = GetActorLocation() + GunOffset;

			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

			// spawn the projectile at the muzzle
			AAgentBullet* tempBullet = World->SpawnActor<AAgentBullet>(myProjectile, SpawnLocation, SpawnRotation, ActorSpawnParams);
			tempBullet->Damage = myDamage;

			if (GEngine)
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("shooting"));
		}

	}

}