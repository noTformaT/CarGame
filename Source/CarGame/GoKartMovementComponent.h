// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GoKartMovementComponent.generated.h"

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


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CARGAME_API UGoKartMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGoKartMovementComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    void SimulateMove(const FGoKartMove& Move);

    FGoKartMove CreateMove(float DeltaTime);

    FVector GetVelocity() { return Velocity; };
    void SetVelocity(FVector Val) { Velocity = Val; };

    void SetThrottle(float Val) { Throttle = Val; };
    void SetSteeringThrow(float Val) { SteeringThrow = Val; };

private:

	void UpdateLocationFromVelocity(float DeltaTime);

	void ApplyRotation(float DeltaTime, float SteeringThrow);

	FVector GetAirResistance();

	FVector GetRollingResistance();

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

    float Throttle = 0.0f;

    float SteeringThrow = 0.0f;

    FVector Velocity;
};
