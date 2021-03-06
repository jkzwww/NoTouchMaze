// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup.h"

// Sets default values
APickup::APickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	VisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Visible Component"));
	VisibleComponent->SetupAttachment(RootComponent);

	//set mesh
	ConstructorHelpers::FObjectFinder<UStaticMesh> PipeMeshObject(TEXT("/Game/StarterContent/Shapes/Shape_Pipe_180.Shape_Pipe_180"));
	if (PipeMeshObject.Succeeded())
	{
		VisibleComponent->SetStaticMesh(PipeMeshObject.Object);
	}

	//set materials
	SpeedMaterial = CreateDefaultSubobject<UMaterial>(TEXT("Speed Material"));
	HealthMaterial = CreateDefaultSubobject<UMaterial>(TEXT("Health Material"));
	ShieldMaterial = CreateDefaultSubobject <UMaterial>(TEXT("Shield Material"));
	CoinMaterial = CreateDefaultSubobject <UMaterial>(TEXT("Coin Material"));

	ConstructorHelpers::FObjectFinder<UMaterial> CoinMaterialObject(TEXT("/Game/StarterContent/Materials/M_Metal_Gold.M_Metal_Gold"));
	ConstructorHelpers::FObjectFinder<UMaterial> HealthMaterialObject(TEXT("/Game/StarterContent/Materials/M_Ground_Moss.M_Ground_Moss"));
	ConstructorHelpers::FObjectFinder<UMaterial> ShieldMaterialObject(TEXT("/Game/StarterContent/Materials/M_Water_Lake.M_Water_Lake"));
	ConstructorHelpers::FObjectFinder<UMaterial> SpeedMaterialObject(TEXT("/Game/StarterContent/Materials/M_ColorGrid_LowSpec.M_ColorGrid_LowSpec"));

	if (SpeedMaterialObject.Succeeded())
	{
		SpeedMaterial = SpeedMaterialObject.Object;
	}

	if (HealthMaterialObject.Succeeded())
	{
		HealthMaterial = HealthMaterialObject.Object;
	}


	if (ShieldMaterialObject.Succeeded())
	{
		ShieldMaterial = ShieldMaterialObject.Object;
	}

	if (CoinMaterialObject.Succeeded())
	{
		CoinMaterial = CoinMaterialObject.Object;
	}

	//set actor rotation (aesthetic purpose)
	SetActorRotation(FRotator(90, 0, 0));

	//collision set up
	VisibleComponent->SetNotifyRigidBodyCollision(true);
	VisibleComponent->BodyInstance.SetCollisionProfileName("BlockAllDynamic");
	VisibleComponent->OnComponentHit.AddDynamic(this, &APickup::OnHit);
	
	//sound effect
	static ConstructorHelpers::FObjectFinder<USoundBase> PickupSoundAsset(TEXT("/Game/myContent/pickup.pickup"));

	if (PickupSoundAsset.Succeeded())
	{
		CollectSound = PickupSoundAsset.Object;
	}

	//default value
	myValue = 5;
}

// Called when the game starts or when spawned
void APickup::BeginPlay()
{
	Super::BeginPlay();
	
	switch (PickupType)
	{
	case 0:
		VisibleComponent->SetMaterial(0, SpeedMaterial);
		break;

	case 1:
		VisibleComponent->SetMaterial(0, HealthMaterial);
		break;

	case 2:
		VisibleComponent->SetMaterial(0, ShieldMaterial);
		break;

	default:
		VisibleComponent->SetMaterial(0, CoinMaterial);
		break;
	}
}

// Called every frame
void APickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//update current second
	currentSecond = GetWorld()->UWorld::GetRealTimeSeconds();

	if (Target)
	{
		//speed effect is over
		if (currentSecond - startSpeedSec > EffectTime)
		{
			//resume speed
			Target->GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
			//destroy itself as picked up
			Destroy();
		}
	
		//shield effect is over
		if (currentSecond - startShieldSec > EffectTime)
		{
			//resume damage
			Target->armorFactor += 0.3;
			//destroy itself as picked up
			Destroy();
		}
		
	
	}

}

void APickup::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (Cast<AagentsMazeCharacter>(OtherActor)) // the cast returns nullptr when failed
	{
		// AActor can be casted into UObject therefor AActor is child of UObject
		if (GEngine)
		{
			GEngine->ClearOnScreenDebugMessages();
		}

		UGameplayStatics::PlaySoundAtLocation(this, CollectSound, GetActorLocation(), 1.0F, 1.0F, 0.0F, nullptr, nullptr);

		//UGameplayStatics::SpawnEmitterAttached(PickedUpParticle, Target->GetRootComponent(), NAME_None, Target->GetActorLocation(), FRotator::ZeroRotator, EAttachLocation::SnapToTarget, false);
		
		//offset
		FVector offset = FVector(0, 0, 30);
		
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PickedUpParticle, GetActorLocation() + offset, FRotator::ZeroRotator, true);


		Target = Cast<AagentsMazeCharacter>(OtherActor);

		//get character movement
		WalkSpeed = Target->GetCharacterMovement()->MaxWalkSpeed;

		switch (PickupType)
		{
		case 0: //increase speed


			if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Character speed increased (5 secs) !!!")); }

			startSpeedSec = currentSecond;

			//double speed
			Target->GetCharacterMovement()->MaxWalkSpeed = WalkSpeed * 2;
			break;

		case 1: //increase health if not full

			if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Extra health +20HP !!!")); }

			if (Target->HP < 100)
			{
				Target->HP += 20; 
				if (Target->HP > 100)
				{
					Target->HP = 100;
				}
			}
			//destroy itself as picked up
			Destroy();
			break;

		case 2: //activate shield

			if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("Shield collected !\nDamage received decreased for 5s !!!")); }

			startShieldSec = currentSecond;
			Target->armorFactor -= 0.3;

			break;

		default: //coin pickup
			if (GEngine) { GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, TEXT("Extra 5 coins collected!!!")); }

			Target->MyCoins += myValue;

			Destroy();
			break;
		}

		SetActorHiddenInGame(true);
	

	}
	else
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("Not target"));
	}

}