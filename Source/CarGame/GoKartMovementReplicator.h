// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GoKartMovementComponent.h"
#include "GoKartMovementReplicator.generated.h"

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

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CARGAME_API UGoKartMovementReplicator : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGoKartMovementReplicator();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	void ClearUnacknowledgedMoves(FGoKartMove LastMove);

	UPROPERTY(ReplicatedUsing = OnRep_ServerState)
	FGoKartState ServerState;

	UFUNCTION()
	void OnRep_ServerState();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendMove(FGoKartMove Move);

	TArray<FGoKartMove> UnacknowledgedMoves;

	UPROPERTY()
	UGoKartMovementComponent* MovementComponent = nullptr;
};
