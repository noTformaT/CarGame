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

    void ClearUnacknowledgedMoves(FGoKartMove LastMove);
    
    void MoveForward(float Value);
    
    void MoveRight(float Value);

    UPROPERTY(ReplicatedUsing=OnRep_ServerState)
    FGoKartState ServerState;

    UFUNCTION()
    void OnRep_ServerState();

    UFUNCTION(Server, Reliable, WithValidation)
    void Server_SendMove(FGoKartMove Move);

    TArray<FGoKartMove> UnacknowledgedMoves;
};
