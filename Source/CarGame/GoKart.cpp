// Fill out your copyright notice in the Description page of Project Settings.


#include "GoKart.h"
#include "Components/InputComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"


// Sets default values
AGoKart::AGoKart()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;

    MovementComponent = CreateDefaultSubobject<UGoKartMovementComponent>(TEXT("MovementComponent"));
    MovementReplicator = CreateDefaultSubobject<UGoKartMovementReplicator>(TEXT("MovementReplicator"));
}

// Called when the game starts or when spawned
void AGoKart::BeginPlay()
{
	Super::BeginPlay();
	
    if (HasAuthority())
    {
        //NetUpdateFrequency = 1;
    }
}

FString GetRoleEnumText(ENetRole Role)
{
    switch (Role)
    {
    case ROLE_None:
        return "ROLE_None";
    case ROLE_SimulatedProxy:
        return "ROLE_SimulatedProxy";;
    case ROLE_AutonomousProxy:
        return "ROLE_AutonomousProxy";;
    case ROLE_Authority:
        return "ROLE_Authority";;
    default:
        return "Error";
    }
}

// Called every frame
void AGoKart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    DrawDebugString(GetWorld(), FVector(0, 0, 100), GetRoleEnumText(GetLocalRole()), this, FColor::White, DeltaTime);
}

// Called to bind functionality to input
void AGoKart::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
    PlayerInputComponent->BindAxis("MoveForward", this, &AGoKart::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AGoKart::MoveRight);
}

void AGoKart::MoveForward(float Value)
{
    MovementComponent->SetThrottle(Value);
}

void AGoKart::MoveRight(float Value)
{
    MovementComponent->SetSteeringThrow(Value);
}