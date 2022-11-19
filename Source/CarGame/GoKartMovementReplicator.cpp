// Fill out your copyright notice in the Description page of Project Settings.


#include "GoKartMovementReplicator.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UGoKartMovementReplicator::UGoKartMovementReplicator()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
}


// Called when the game starts
void UGoKartMovementReplicator::BeginPlay()
{
	Super::BeginPlay();

    MovementComponent = GetOwner()->FindComponentByClass<UGoKartMovementComponent>();
}


// Called every frame
void UGoKartMovementReplicator::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (MovementComponent == nullptr)
    {
        return;
    }

    FGoKartMove LastMove = MovementComponent->GetLastMove();

    if (GetOwner()->GetLocalRole() == ROLE_AutonomousProxy)
    {
        UnacknowledgedMoves.Add(LastMove);
        Server_SendMove(LastMove);
    }

    // We are in the server and in control of the pawn
    if (GetOwner()->GetRemoteRole() == ROLE_SimulatedProxy)
    {
        UpdateServerState(LastMove);
    }

    if (GetOwner()->GetLocalRole() == ROLE_SimulatedProxy)
    {
        MovementComponent->SimulateMove(ServerState.LastMove);
    }

    
}

void UGoKartMovementReplicator::UpdateServerState(const FGoKartMove& Move)
{
    ServerState.LastMove = Move;
    ServerState.Transform = GetOwner()->GetActorTransform();
    ServerState.Velocity = MovementComponent->GetVelocity();
}

void UGoKartMovementReplicator::ClearUnacknowledgedMoves(FGoKartMove LastMove)
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

void UGoKartMovementReplicator::Server_SendMove_Implementation(FGoKartMove Move)
{
    if (MovementComponent == nullptr)
    {
        return;
    }
    MovementComponent->SimulateMove(Move);

    UpdateServerState(Move);
}

bool UGoKartMovementReplicator::Server_SendMove_Validate(FGoKartMove Move)
{
    return true; // TODO: Make better validation
}

void UGoKartMovementReplicator::OnRep_ServerState()
{
    if (MovementComponent == nullptr)
    {
        return;
    }

    GetOwner()->SetActorTransform(ServerState.Transform);
    MovementComponent->SetVelocity(ServerState.Velocity);
    ClearUnacknowledgedMoves(ServerState.LastMove);

    for (const FGoKartMove& Move : UnacknowledgedMoves)
    {
        MovementComponent->SimulateMove(Move);
    }
}

void UGoKartMovementReplicator::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UGoKartMovementReplicator, ServerState);
}