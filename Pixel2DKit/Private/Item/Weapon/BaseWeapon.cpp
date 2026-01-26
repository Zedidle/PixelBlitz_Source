// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/BaseWeapon.h"
#include "Core/PXGameInstance.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "Subsystems/DataTableSubsystem.h"
#include "Character/BasePXCharacter.h"

class UDataTableSubsystem;

void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	InitWeaponData();
}

void ABaseWeapon::SetCharacterOwner_Implementation(ABasePXCharacter* PxCharacter)
{
	if (AActor* Actor = Cast<AActor>(PxCharacter))
	{
		SetOwner(Actor);
	}
}

void ABaseWeapon::InitWeaponData()
{
	LoadWeaponData(WeaponTag);
}

void ABaseWeapon::LoadWeaponData(const FGameplayTag _WeaponTag)
{
	if (!_WeaponTag.IsValid()) return;
	if (!_WeaponTag.MatchesTag(FGameplayTag::RequestGameplayTag("Weapon"))) return;

	UPXGameInstance* GameInstance = GetGameInstance<UPXGameInstance>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(GameInstance)

	UDataTableSubsystem* DataTableSubsystem = GameInstance->GetSubsystem<UDataTableSubsystem>();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(DataTableSubsystem)

	if (const FWeaponData* WeaponDataPtr = DataTableSubsystem->GetWeaponDataByTag(_WeaponTag))
	{
		Data = *WeaponDataPtr;
		WeaponTag = _WeaponTag;
	}
}


float ABaseWeapon::GetRepelPower() const
{
	return Data.RepelPower;
}
