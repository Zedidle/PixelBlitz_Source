// Fill out your copyright notice in the Description page of Project Settings.


#include "PXGameplayTags.h"
#include "Engine/EngineTypes.h"
#include "GameplayTagsManager.h"

namespace PXGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayFlow_OnLevelLoading, "GameplayFlow.OnLoadingLevel", "Level loading has started");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayFlow_OnLevelLoaded, "GameplayFlow.OnLevelLoaded", "Level loading has completed");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayFlow_OnLevelStarted, "GameplayFlow.OnStartLevelSuccess", "Level gameplay has started");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(GameplayFlow_OnEnemyDie, "GameplayFlow.OnEnemyDie", "Enemy died");

	FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString)
	{
		const UGameplayTagsManager& Manager = UGameplayTagsManager::Get();
		FGameplayTag Tag = Manager.RequestGameplayTag(FName(*TagString), false);

		if (!Tag.IsValid() && bMatchPartialString)
		{
			FGameplayTagContainer AllTags;
			Manager.RequestAllGameplayTags(AllTags, true);

			for (const FGameplayTag& TestTag : AllTags)
			{
				if (TestTag.ToString().Contains(TagString))
				{
					UE_LOG(LogTemp, Display, TEXT("Could not find exact match for tag [%s] but found partial match on tag [%s]."), *TagString, *TestTag.ToString());
					Tag = TestTag;
					break;
				}
			}
		}

		return Tag;
	}
}
