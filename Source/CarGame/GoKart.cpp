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

}

// Called when the game starts or when spawned
void AGoKart::BeginPlay()
{
	Super::BeginPlay();
	
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
    FVector Force = GetActorForwardVector() * MaxDrivingForce * Throttle;

    Force += GetAirResistance();
    Force += GetRollingResistance();

    FVector Acceleration = Force / Mass;
    
    Velocity = Velocity + Acceleration * DeltaTime;

    ApplyRotation(DeltaTime);
    
    UpdateLocationFromVelocity(DeltaTime);

    UE_LOG(LogTemp, Display, TEXT("Throttle: %f, Force (x: %f, y: %f, z: %f), "), Throttle, Force.X, Force.Y, Force.Z);
    
    DrawDebugString(GetWorld(), FVector(0, 0, 100), GetRoleEnumText(GetLocalRole()), this, FColor::White, DeltaTime);
}

void AGoKart::ApplyRotation(float DeltaTime)
{
    float DeltaLocation = FVector::DotProduct(GetActorForwardVector(), Velocity) * DeltaTime;
    float RotationAngle = DeltaLocation / MinTurningRadius * SteeringThrow;
    FQuat RotationDelta(GetActorUpVector(), RotationAngle);

    Velocity = RotationDelta.RotateVector(Velocity);

    AddActorWorldRotation(RotationDelta);
}

FVector AGoKart::GetAirResistance()
{
    return - Velocity.GetSafeNormal() * Velocity.SizeSquared() * DragCoefficient;
}

FVector AGoKart::GetRollingResistance()
{
    float AccelerationDueToGravity = -GetWorld()->GetGravityZ() / 100.0f;
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

void AGoKart::Server_MoveForward_Implementation(float Value)
{
    Throttle = Value;
}

bool AGoKart::Server_MoveForward_Validate(float Value)
{
    return FMath::Abs(Value) <= 1.0f;
}

void AGoKart::Server_MoveRight_Implementation(float Value)
{
    SteeringThrow = Value;
}

bool AGoKart::Server_MoveRight_Validate(float Value)
{
    return FMath::Abs(Value) <= 1.0f;
}

void AGoKart::MoveForward(float Value)
{
    Throttle = Value;
    Server_MoveForward(Value);
}

void AGoKart::MoveRight(float Value)
{
    SteeringThrow = Value;
    Server_MoveRight(Value);
}
