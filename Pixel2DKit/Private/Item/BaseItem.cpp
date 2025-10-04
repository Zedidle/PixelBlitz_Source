// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/BaseItem.h"

#include "Sound/SoundCue.h"
#include "Utilitys/SoundFuncLib.h"

// Sets default values
ABaseItem::ABaseItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABaseItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseItem::SetData(const FItemData& ItemData)
{
	Data = ItemData;
}

void ABaseItem::Use_Implementation()
{
	if (USoundCue* SoundCue = Data.UseSound.LoadSynchronous())
	{
		USoundFuncLib::PlaySoundAtLocation(SoundCue, GetActorLocation());
	}
}

void ABaseItem::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	
}

void ABaseItem::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);
}

