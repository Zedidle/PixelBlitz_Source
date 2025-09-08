// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Item/BaseItem.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Utilitys/CommonFuncLib.h"
#include "Utilitys/SoundFuncLib.h"
#include "BaseWeapon.generated.h"


USTRUCT(BlueprintType)
struct FWeaponData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Drop")
	FGameplayTag WeaponTag;
		
	UPROPERTY(BlueprintReadWrite,EditAnywhere,Category="Drop")
	TSubclassOf<ABaseWeapon> SpawnItemClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponData)
	TSoftObjectPtr<USoundCue> UseSound;

	UPROPERTY(Config, EditAnywhere, Category = "CameraShake")
	TSubclassOf<UCameraShakeBase> UseCameraShake;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponData)
	int Damage = 5.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponData)
	float RepelPower = 50.f;
};


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
		USoundFuncLib::PlaySoundAtLocation(SoundCue, GetActorLocation(), 1.0f);
	}

	if (Data.UseCameraShake)
	{
		UGameplayStatics::PlayWorldCameraShake(GetWorld(), Data.UseCameraShake,
			GetActorLocation(), 0.0f, 500.0f, 0.0f, true);
	}

	
}
