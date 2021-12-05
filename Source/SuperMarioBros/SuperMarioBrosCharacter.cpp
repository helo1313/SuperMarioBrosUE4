// Copyright Epic Games, Inc. All Rights Reserved.

#include "SuperMarioBrosCharacter.h"
#include "PaperFlipbookComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Camera/CameraComponent.h"

DEFINE_LOG_CATEGORY_STATIC(SideScrollerCharacter, Log, All);

//////////////////////////////////////////////////////////////////////////
// ASuperMarioBrosCharacter

ASuperMarioBrosCharacter::ASuperMarioBrosCharacter()
{
	// Use only Yaw from the controller and ignore the rest of the rotation.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Set the size of our collision capsule.
	GetCapsuleComponent()->SetCapsuleHalfHeight(50.0f);
	GetCapsuleComponent()->SetCapsuleRadius(40.0f);

	// Create a camera boom attached to the root (capsule)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 500.0f;
	CameraBoom->SocketOffset = FVector(0.0f, 0.0f, 75.0f);
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	

	// Create an orthographic camera (no perspective) and attach it to the boom
	SideViewCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("SideViewCamera"));
	SideViewCameraComponent->ProjectionMode = ECameraProjectionMode::Orthographic;
	SideViewCameraComponent->OrthoWidth = 2048.0f;
	SideViewCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	// Prevent all automatic rotation behavior on the camera, character, and camera component
	CameraBoom->SetUsingAbsoluteRotation(true);
	SideViewCameraComponent->bUsePawnControlRotation = false;
	SideViewCameraComponent->bAutoActivate = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	// Configure character movement
	GetCharacterMovement()->GravityScale = 2.0f;
	GetCharacterMovement()->AirControl = 0.80f;
	GetCharacterMovement()->JumpZVelocity = 1000.f;
	GetCharacterMovement()->GroundFriction = 3.0f;
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	GetCharacterMovement()->MaxFlySpeed = 600.0f;

	// Lock character motion onto the XZ plane, so the character can't move in or out of the screen
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0.0f, -1.0f, 0.0f));

	// Behave like a traditional 2D platformer character, with a flat bottom instead of a curved capsule bottom
	// Note: This can cause a little floating when going up inclines; you can choose the tradeoff between better
	// behavior on the edge of a ledge versus inclines by setting this to true or false
	GetCharacterMovement()->bUseFlatBaseForFloorChecks = true;

	// Enable replication on the Sprite component so animations show up when networked
	GetSprite()->SetIsReplicated(true);
	bReplicates = true;
	
	//set up variables
	AllowAnimationUpdate = true;
	bIsAlive = true;
	Time = 399;
}

//////////////////////////////////////////////////////////////////////////
// Animation

void ASuperMarioBrosCharacter::UpdateAnimation()
{
	if(AllowAnimationUpdate)
	{
		const FVector PlayerVelocity = GetVelocity();
		const float PlayerSpeedSqr = PlayerVelocity.SizeSquared();

		UPaperFlipbook* DesiredAnimation = nullptr;
	
		if(bIsJumping) //AnimJump 
			{
			DesiredAnimation = JumpAnimation;
			}
		else if (PlayerSpeedSqr > 0)  //Anim Run
			{
			DesiredAnimation = RunningAnimation;
			}
		else	//Anim Idle
			{
			DesiredAnimation = IdleAnimation;
			}

		//Update animation
		if( GetSprite()->GetFlipbook() != DesiredAnimation 	)
		{
			GetSprite()->SetFlipbook(DesiredAnimation);
		}
	}
}

void ASuperMarioBrosCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	UpdateCharacter();

	UpdateTime(DeltaSeconds);
}


//////////////////////////////////////////////////////////////////////////
// Input

void ASuperMarioBrosCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Note: the 'Jump' action and the 'MoveRight' axis are bound to actual keys/buttons/sticks in DefaultInput.ini (editable from Project Settings..Input)
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASuperMarioBrosCharacter::MarioJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASuperMarioBrosCharacter::MoveRight);

	PlayerInputComponent->BindTouch(IE_Pressed, this, &ASuperMarioBrosCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ASuperMarioBrosCharacter::TouchStopped);
}

void ASuperMarioBrosCharacter::MoveRight(float Value)
{
	/*UpdateChar();*/

	// Apply the input to the character motion
	AddMovementInput(FVector(1.0f, 0.0f, 0.0f), Value);
}

void ASuperMarioBrosCharacter::TouchStarted(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	// Jump on any touch
	MarioJump();
}

void ASuperMarioBrosCharacter::TouchStopped(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	// Cease jumping once touch stopped
	StopJumping();
}

void ASuperMarioBrosCharacter::UpdateCharacter()
{
	// Update animation to match the motion
	UpdateAnimation();

	// Now setup the rotation of the controller based on the direction we are travelling
	const FVector PlayerVelocity = GetVelocity();	
	float TravelDirection = PlayerVelocity.X;
	// Set the rotation so that the character faces his direction of travel.
	if (Controller != nullptr)
	{
		if (TravelDirection < 0.0f)
		{
			Controller->SetControlRotation(FRotator(0.0, 180.0f, 0.0f));
		}
		else if (TravelDirection > 0.0f)
		{
			Controller->SetControlRotation(FRotator(0.0f, 0.0f, 0.0f));
		}
	}
}

/** Other functions */

void ASuperMarioBrosCharacter::Jump()
{
	FVector LaunchVelocity = FVector(0.f,0.f,1000.f);
	LaunchCharacter(LaunchVelocity, false , true);
}

void ASuperMarioBrosCharacter::Landed(const FHitResult& Hit)
{
	JumpsAmount = 0;
	bIsJumping = false;
}

void ASuperMarioBrosCharacter::MarioJump()
{
	if (JumpsAmount < 2)
	{
		Jump();
		JumpsAmount++;
		bIsJumping = true;
	}
	else
	{
		Death();
	}
}

//Handle Mario Death
void ASuperMarioBrosCharacter::Death()
{
	AllowAnimationUpdate = false;
	bIsAlive = false;
	GetSprite()-> SetFlipbook(DeathAnimation);
	
	//Launching Player On Death
	FVector LaunchVelocity = FVector(0.f,0.f,500.f);
	LaunchCharacter(LaunchVelocity, true , true);
	
	DisableInput(GetWorld()->GetFirstPlayerController());
}

//Update time left
void ASuperMarioBrosCharacter::UpdateTime(float DeltaTime) 
{
	Time = Time - DeltaTime;
}

void ASuperMarioBrosCharacter::AddCoin()
{
	Coins++;
}




