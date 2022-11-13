// Fill out your copyright notice in the Description page of Project Settings.


#include "GoKart.h"
#include "Components/InputComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"

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

    if (GetLocalRole() == ROLE_AutonomousProxy)
    {
        FGoKartMove Move = CreateMove(DeltaTime);
        SimulateMove(Move);

        UnacknowledgedMoves.Add(Move);
        Server_SendMove(Move);
    }

    // We are in the server and in control of the pawn
    if (GetLocalRole() == ROLE_Authority && GetRemoteRole() == ROLE_SimulatedProxy)
    {
        FGoKartMove Move = CreateMove(DeltaTime);
        Server_SendMove(Move);
    }

    if (GetLocalRole() == ROLE_SimulatedProxy)
    {
        SimulateMove(ServerState.LastMove);
    }

    DrawDebugString(GetWorld(), FVector(0, 0, 100), GetRoleEnumText(GetLocalRole()), this, FColor::White, DeltaTime);
}

void AGoKart::ClearUnacknowledgedMoves(FGoKartMove LastMove)
{
    UE_LOG(LogTemp, Display, TEXT("ClearUnacknowledgedMoves, LastTime: %f, Throttle:%f"), LastMove.Time, LastMove.Throttle);

    TArray<FGoKartMove> NewMoves;
    
    for (const FGoKartMove& Move : UnacknowledgedMoves)
    {
        if (Move.Time > LastMove.Time)
        {
            NewMoves.Add(Move);
        }
    }

    UnacknowledgedMoves = NewMoves;
}

void AGoKart::ApplyRotation(float DeltaTime, float SteeringThr)
{
    float DeltaLocation = FVector::DotProduct(GetActorForwardVector(), Velocity) * DeltaTime;
    float RotationAngle = DeltaLocation / MinTurningRadius * SteeringThr;
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

void AGoKart::Server_SendMove_Implementation(FGoKartMove Move)
{
    SimulateMove(Move);
    ServerState.LastMove = Move;
    ServerState.Transform = GetActorTransform();
    ServerState.Velocity = Velocity;
}

bool AGoKart::Server_SendMove_Validate(FGoKartMove Move)
{
    return true; // TODO: Make better validation
}

void AGoKart::MoveForward(float Value)
{
    Throttle = Value;
}

void AGoKart::MoveRight(float Value)
{
    SteeringThrow = Value;
}

void AGoKart::OnRep_ServerState()
{

    SetActorTransform(ServerState.Transform);
    Velocity = ServerState.Velocity;
    ClearUnacknowledgedMoves(ServerState.LastMove);

    for (const FGoKartMove& Move : UnacknowledgedMoves)
    {
        SimulateMove(Move);
    }
}

void AGoKart::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AGoKart, ServerState);
}
