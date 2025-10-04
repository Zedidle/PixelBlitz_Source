// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Item/BaseItem.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Utilitys/PXCustomStruct.h"
#include "Utilitys/SoundFuncLib.h"
#include "BaseWeapon.generated.h"

class ABasePXCharacter;


UCLASS(Abstract, Blueprintable)
class PIXEL2DKIT_API ABaseWeapon : public ABaseItem
{
	GENERATED_BODY()

	FWeaponData Data;

protected:
	virtual void BeginPlay() override;
	

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag WeaponTag;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetCharacterOwner(ABasePXCharacter* PxCharacter);
	
	void InitWeaponData();
	
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void LoadWeaponData(const FGameplayTag _WeaponTag);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int GetWeaponDamage() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	float GetRepelPower() const;
	

	virtual void Use_Implementation() override;
	
};

inline int ABaseWeapon::GetWeaponDamage() const
{
	return Data.Damage;
}

inline void ABaseWeapon::Use_Implementation()
{
	Super::Use_Implementation();
	if (USoundCue* SoundCue = Data.UseSound.LoadSynchronous())
	{
		USoundFuncLib::PlaySoundAtLocation(SoundCue, GetActorLocation());
	}

	if (Data.UseCameraShake)
	{
		UGameplayStatics::PlayWorldCameraShake(GetWorld(), Data.UseCameraShake,
			GetActorLocation(), 0.0f, 500.0f, 0.0f, true);
	}

	
}
