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
	mutable TMap<FString, TObjectPtr<UDataTable>> LocalizedDataTables;

public:

	virtual void PostInitProperties() override;

	static const UDataTableSettings& Get()
	{
		return *GetDefault<UDataTableSettings>();
	}
	
	UDataTable* GetLocalizedDataTable(const FLocalizedTableData& Data) const;
	
	UPROPERTY(Config, EditAnywhere, Category = "Character")
	TSoftObjectPtr<UDataTable> UIDataPath;
	TObjectPtr<UDataTable> GetUIData() const { return GetData(UIDataPath.ToString()); }
	
	
	UPROPERTY(Config, EditAnywhere, Category = "Character")
	TSoftObjectPtr<UDataTable> CharacterDataPath;
	TObjectPtr<UDataTable> GetCharacterData() const { return GetData(CharacterDataPath.ToString()); }
	
	UPROPERTY(Config, EditAnywhere, Category = "ItemData")
	TSoftObjectPtr<UDataTable> ItemDataPath;
	TObjectPtr<UDataTable> GetItemData() const { return GetData(ItemDataPath.ToString()); }
	
	UPROPERTY(Config, EditAnywhere, Category = "Buff")
	TSoftObjectPtr<UDataTable> BuffOnWidgetDataPath;
	TObjectPtr<UDataTable> GetBuffOnWidgetData() const { return GetData(BuffOnWidgetDataPath.ToString()); }

	
	UPROPERTY(Config, EditAnywhere, Category = "Ability")
	TArray<TSoftObjectPtr<UDataTable>> AbilityDataPaths;
	mutable TMap<FGameplayTag, FAbility> AbilityData;
	void LoadAbilityData() const;
	const FAbility* GetAbilityDataByTag(const FGameplayTag& AbilityTag) const;
	const TMap<FGameplayTag, FAbility>& GetAllAbilities() const;
	TArray<TObjectPtr<UDataTable>> GetAbilityData() const
	{
		TArray<TObjectPtr<UDataTable>> Result;
		for (const auto& Path : AbilityDataPaths)
		{
			Result.Add(GetData(Path.ToString()));
		}
		return Result;
	}

	
	UPROPERTY(Config, EditAnywhere, Category = "Talent")
	TSoftObjectPtr<UDataTable> TalentDataPath;
	mutable TMap<FGameplayTag, FTalent> TalentData;
	TObjectPtr<UDataTable> GetTalentData() const { return GetData(TalentDataPath.ToString()); }
	void LoadTalentData() const;
	const FTalent* GetTalentDataByTag(const FGameplayTag& TalentTag) const;


	
	UPROPERTY(Config, EditAnywhere, Category = "Achievement")
	TSoftObjectPtr<UDataTable> AchievementDataPath;
	TObjectPtr<UDataTable> GetAchievementData() const { return GetData(AchievementDataPath.ToString()); }


	
	
	UPROPERTY(Config, EditAnywhere, Category = "Weather")
	TSoftObjectPtr<UDataTable> LevelWeatherRateDataPath;
	TObjectPtr<UDataTable> GetLevelWeatherRateData() const { return GetData(LevelWeatherRateDataPath.ToString()); }
	
	UPROPERTY(Config, EditAnywhere, Category = "Weather")
	TSoftObjectPtr<UDataTable> WeatherDataPath;
	TObjectPtr<UDataTable> GetWeatherData() const { return GetData(WeatherDataPath.ToString()); }

	
	
	UPROPERTY(Config, EditAnywhere, Category = "Weapon")
	TSoftObjectPtr<UDataTable> WeaponDataPath;
	mutable TMap<FGameplayTag, FWeaponData> WeaponData;
	void LoadWeaponData() const;
	TObjectPtr<UDataTable> GetWeaponData() const { return GetData(WeaponDataPath.ToString()); }
	const FWeaponData* GetWeaponDataByTag(const FGameplayTag& WeaponTag) const;










	

	
	

	








	
	


	
};
