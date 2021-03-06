// Fill out your copyright notice in the Description page of Project Settings.


#include "Coin.h"
#include "SuperMarioBrosCharacter.h"
#include "Components/BoxComponent.h"

// Sets default values
ACoin::ACoin()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CoinRootComponent = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = CoinRootComponent;
	
	CoinFlipBook = CreateDefaultSubobject<UPaperFlipbookComponent>("Flipbook");
	CoinFlipBook->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	CoinFlipBook->AttachToComponent(CoinRootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	CoinBoxCollision = CreateDefaultSubobject<UBoxComponent>("Collision Box");
	CoinBoxCollision->SetRelativeScale3D(FVector(1.0f,1.0f,1.0f));
	CoinBoxCollision->AttachToComponent(CoinRootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	CoinBoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ACoin::OnCoinPickedUp);

	
	
}

// Called when the game starts or when spawned
void ACoin::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACoin::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//Handle Begin overlap / Collect coin
void ACoin::OnCoinPickedUp(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ASuperMarioBrosCharacter* Player = Cast<ASuperMarioBrosCharacter>(OtherActor);

	if(Player)
	{
		Player->AddCoin();
		this->Destroy();
	}
	
}

