// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GoKart.generated.h"

USTRUCT()
struct FGoKartMove
{
    GENERATED_USTRUCT_BODY();

    UPROPERTY()
    float Throttle;

    UPROPERTY()
    float SteeringThrow;

    UPROPERTY()
    float DeltaTime;

    UPROPERTY()
    float Time;
};

USTRUCT()
struct FGoKartState
{
    GENERATED_USTRUCT_BODY();

    UPROPERTY()
    FTransform Transform;

    UPROPERTY()
    FVector Velocity;

    UPROPERTY()
    FGoKartMove LastMove;


};

UCLASS()
class CARGAME_API AGoKart : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AGoKart();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    
private:
    // The mass of the car
    UPROPERTY(EditAnywhere)
    float Mass = 1000.0f;
    
    // The force applied to car when throttle is fully down (N)
    UPROPERTY(EditAnywhere)
    float MaxDrivingForce = 10000.0f;

    // Minimum radius of the car turning circle at full lock (M)
    UPROPERTY(EditAnywhere)
    float MinTurningRadius = 10.0f;

    // Higher means more drag
    UPROPERTY(EditAnywhere)
    float DragCoefficient = 16.0f;

    // Higher means more rolling resistance
    UPROPERTY(EditAnywhere)
    float RollingResistanceCoefficient = 0.015f;
    
    UFUNCTION(Server, Reliable, WithValidation)
    void Server_SendMove(FGoKartMove Move);

    void ClearUnacknowledgedMoves(FGoKartMove LastMove);
    FGoKartMove CreateMove(float DeltaTime);
    void SimulateMove(FGoKartMove Move);

    void MoveForward(float Value);
    void MoveRight(float Value);

    void UpdateLocationFromVelocity(float DeltaTime);

    void ApplyRotation(float DeltaTime, float SteeringThrow);

    FVector GetAirResistance();
    FVector GetRollingResistance();

    float Throttle = 0.0f;
    
    float SteeringThrow = 0.0f;

    UPROPERTY(ReplicatedUsing=OnRep_ServerState)
    FGoKartState ServerState;

    UFUNCTION()
    void OnRep_ServerState();

    FVector Velocity;

    TArray<FGoKartMove> UnacknowledgedMoves;
};
