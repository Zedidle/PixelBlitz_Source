// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/BaseWeapon.h"

#include "Pixel2DKit/Pixel2DKit.h"
#include "Settings/DataTableSettings.h"

void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	InitWeaponData();
}

void ABaseWeapon::InitWeaponData()
{
	LoadWeaponData(WeaponTag);
}

void ABaseWeapon::LoadWeaponData(const FGameplayTag _WeaponTag)
{
	if (!_WeaponTag.IsValid()) return;
	if (!_WeaponTag.MatchesTag(FGameplayTag::RequestGameplayTag("Weapon"))) return;

	
	const UDataTableSettings* DataTableSettings = GetDefault<UDataTableSettings>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(DataTableSettings);

	if (const FWeaponData* WeaponDataPtr = DataTableSettings->GetWeaponDataByTag(_WeaponTag))
	{
		Data = *WeaponDataPtr;
		WeaponTag = _WeaponTag;
	}
}


float ABaseWeapon::GetRepelPower() const
{
	return Data.RepelPower;
}
