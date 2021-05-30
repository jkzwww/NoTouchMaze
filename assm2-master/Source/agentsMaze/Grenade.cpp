// Fill out your copyright notice in the Description page of Project Settings.


#include "Grenade.h"

// Sets default values
AGrenade::AGrenade()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	VisibleComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualRepresentation"));
	VisibleComponent->SetupAttachment(RootComponent);

	//sphere trigger component
	TriggerSphere = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerSphere"));
	TriggerSphere->InitSphereRadius(300);
	TriggerSphere->SetCollisionProfileName(TEXT("Trigger"));
	TriggerSphere->SetupAttachment(RootComponent);

	TriggerSphere->OnComponentBeginOverlap.AddDynamic(this, &AGrenade::OnOverlapBegin);

	//set static mesh
	static ConstructorHelpers::FObjectFinder<UStaticMesh>VisualAsset(TEXT("/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere"));

	if (VisualAsset.Succeeded())
	{
		VisibleComponent->SetStaticMesh(VisualAsset.Object);
	}

	//set material
	static ConstructorHelpers::FObjectFinder<UMaterial>FoundMaterial(TEXT("/Game/StarterContent/Materials/M_Metal_Steel.M_Metal_Steel"));

	if (FoundMaterial.Succeeded())
	{
		StoredMaterial = FoundMaterial.Object;
	}
	DynamicMaterialInst = UMaterialInstanceDynamic::Create(StoredMaterial, VisibleComponent);

	VisibleComponent->SetMaterial(0, DynamicMaterialInst);
	
	//collision set up
	VisibleComponent->SetSimulatePhysics(true);

	//sound effect

	static ConstructorHelpers::FObjectFinder<USoundBase> ExplosionSoundAsset(TEXT("/Game/StarterContent/Audio/Explosion02.Explosion02"));

	if (ExplosionSoundAsset.Succeeded())
	{
		BombEffect = ExplosionSoundAsset.Object;
	}

	//default damage
	damage = 60;

	bombDelay = 0.8;

	startSecond = 0;

	isExplode = false;
}

// Called when the game starts or when spawned
void AGrenade::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGrenade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//update current second
	currentSecond = GetWorld()->UWorld::GetRealTimeSeconds();

	if (!startSecond)
	{
		startSecond = currentSecond;
	}

	if (currentSecond - startSecond > bombDelay)
	{

		UGameplayStatics::PlaySoundAtLocation(this, BombEffect, GetActorLocation(), 1.0F, 1.0F, 0.0F, nullptr, nullptr);
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplodeParticleSystem, GetActorLocation(), FRotator::ZeroRotator, true);
		isExplode = true;
		Destroy();
	}
	

}

void AGrenade::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this))// && OtherComp)
	{
		//if (Cast<AagentsMazeCharacter>(OtherActor))
		
	}
}