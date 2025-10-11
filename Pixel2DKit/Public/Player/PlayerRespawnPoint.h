// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerRespawnPoint.generated.h"

UCLASS()
class PIXEL2DKIT_API APlayerRespawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlayerRespawnPoint();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=GameplayFlow)
	FVector LocationOffset = FVector(0, 0, 100);
	
	void SetLocation(const FVector& L) { SetActorLocation(L + LocationOffset); }
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
