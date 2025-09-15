// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Item/Weapon/BaseWeapon.h"
#include "Utilitys/PXCustomStruct.h"
#include "DataTableSettings.generated.h"


UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "DataTable Settings"))
class PIXEL2DKIT_API UDataTableSettings : public UDeveloperSettings
{
	GENERATED_BODY()


	TObjectPtr<UDataTable> GetData(const FString& Path) const;
	
	mutable TMap<FName, TObjectPtr<UDataTable>> DataTables;

	UPROPERTY(Config, EditAnywhere, Category = "Character")
	TSoftObjectPtr<UDataTable> CharacterDataPath;

	UPROPERTY(Config, EditAnywhere, Category = "ItemData")
	TSoftObjectPtr<UDataTable> ItemDataPath;

	UPROPERTY(Config, EditAnywhere, Category = "Buff")
	TSoftObjectPtr<UDataTable> BuffOnWidgetDataPath;

	UPROPERTY(Config, EditAnywhere, Category = "Ability")
	TArray<TSoftObjectPtr<UDataTable>> AbilityDataPaths;
	mutable TMap<FGameplayTag, FAbility> AbilityData;
	
	UPROPERTY(Config, EditAnywhere, Category = "Talent")
	TSoftObjectPtr<UDataTable> TalentDataPath;
	mutable TMap<FGameplayTag, FTalent> TalentData;
	
	UPROPERTY(Config, EditAnywhere, Category = "Achievement")
	TSoftObjectPtr<UDataTable> AchievementDataPath;

	UPROPERTY(Config, EditAnywhere, Category = "Weather")
	TSoftObjectPtr<UDataTable> LevelWeatherRateDataPath;
	
	UPROPERTY(Config, EditAnywhere, Category = "Weather")
	TSoftObjectPtr<UDataTable> WeatherDataPath;

	UPROPERTY(Config, EditAnywhere, Category = "Weapon")
	TSoftObjectPtr<UDataTable> WeaponDataPath;
	mutable TMap<FGameplayTag, FWeaponData> WeaponData;
	
public:

	virtual void PostInitProperties() override;

	
	static const UDataTableSettings& Get()
	{
		return *GetDefault<UDataTableSettings>();
	}


	mutable TMap<FString, TObjectPtr<UDataTable>> LocalizedDataTables;
	UDataTable* GetLocalizedDataTable(const FLocalizedTableData& Data) const;


	TObjectPtr<UDataTable> GetCharacterData() const { return GetData(CharacterDataPath.ToString()); }
	TObjectPtr<UDataTable> GetItemData() const { return GetData(ItemDataPath.ToString()); }
	TObjectPtr<UDataTable> GetBuffOnWidgetData() const { return GetData(BuffOnWidgetDataPath.ToString()); }

	TArray<TObjectPtr<UDataTable>> GetAbilityData() const
	{
		TArray<TObjectPtr<UDataTable>> Result;
		for (const auto& Path : AbilityDataPaths)
		{
			Result.Add(GetData(Path.ToString()));
		}
		return Result;
	}
	void LoadAbilityData() const;
	const FAbility* GetAbilityDataByTag(const FGameplayTag& AbilityTag) const;
	const TMap<FGameplayTag, FAbility>& GetAllAbilities() const;
	
	TObjectPtr<UDataTable> GetTalentData() const { return GetData(TalentDataPath.ToString()); }
	void LoadTalentData() const;
	const FTalent* GetTalentDataByTag(const FGameplayTag& TalentTag) const;
	
	TObjectPtr<UDataTable> GetAchievementData() const { return GetData(AchievementDataPath.ToString()); }
	TObjectPtr<UDataTable> GetLevelWeatherRateData() const { return GetData(LevelWeatherRateDataPath.ToString()); }
	TObjectPtr<UDataTable> GetWeatherData() const { return GetData(WeatherDataPath.ToString()); }
	
	TObjectPtr<UDataTable> GetWeaponData() const { return GetData(WeaponDataPath.ToString()); }
	void LoadWeaponData() const;
	const FWeaponData* GetWeaponDataByTag(const FGameplayTag& WeaponTag) const;
	








	
	


	
};
