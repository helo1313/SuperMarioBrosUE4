// Fill out your copyright notice in the Description page of Project Settings.


#include "MarioCharacter.h"

// Sets default values
AMarioCharacter::AMarioCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	

}

// Called when the game starts or when spawned
void AMarioCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMarioCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMarioCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

