// Fill out your copyright notice in the Description page of Project Settings.


#include "GoKart.h"
#include "Components/InputComponent.h"
#include "Engine/World.h"

// Sets default values
AGoKart::AGoKart()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGoKart::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGoKart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    FVector Force = GetActorForwardVector() * MaxDrivingForce * Throttle;

    Force += GetAirResistance();
    Force += GetRollingResistance();

    FVector Acceleration = Force / Mass;
    
    Velocity = Velocity + Acceleration * DeltaTime;

    ApplyRotation(DeltaTime);
    
    UpdateLocationFromVelocity(DeltaTime);
}

void AGoKart::ApplyRotation(float DeltaTime)
{
    float RotationAngle = MaxDegreesPerSecond * DeltaTime * SteeringThrow;
    FQuat RotationDelta(GetActorUpVector(), FMath::DegreesToRadians(RotationAngle));

    Velocity = RotationDelta.RotateVector(Velocity);

    AddActorWorldRotation(RotationDelta);
}

FVector AGoKart::GetAirResistance()
{
    return - Velocity.GetSafeNormal() * Velocity.SizeSquared() * DragCoefficient;
}

FVector AGoKart::GetRollingResistance()
{
    float AccelerationDueToGravity = GetWorld()->GetGravityZ() / 100.0f;
    float NormalForce = Mass * AccelerationDueToGravity;
    return -Velocity.GetSafeNormal() * RollingResistanceCoefficient * NormalForce;
}

void AGoKart::UpdateLocationFromVelocity(float DeltaTime)
{
    FVector Translation = Velocity * DeltaTime * 100;

    FHitResult Hit;
    AddActorWorldOffset(Translation, true, &Hit);
    if (Hit.IsValidBlockingHit())
    {
        Velocity = FVector::ZeroVector;
    }
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
    Throttle = Value;
}

void AGoKart::MoveRight(float Value)
{
    SteeringThrow = Value;
}
