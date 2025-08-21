// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Components/TalentComponent.h"

#include "Core/PXSaveGameSubsystem.h"
#include "Settings/DataTableSettings.h"

// Sets default values for this component's properties
UTalentComponent::UTalentComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UTalentComponent::BeginPlay()
{
	Super::BeginPlay();

	PXCharacter = Cast<ABasePXCharacter>(GetOwner());
	if (PXCharacter)
	{
		OwnerPreLocation = PXCharacter->GetActorLocation();
		if (UPXSaveGameSubsystem* SaveGameSubsystem = PXCharacter->GetGameInstance()->GetSubsystem<UPXSaveGameSubsystem>())
		{
			UDataTable* TalentData = UDataTableSettings::Get().GetTalentData();
			if (SaveGameSubsystem->BasicBuildSaveGame && TalentData)
			{
				TArray<FName> ChoicedTalentIndexes = SaveGameSubsystem->BasicBuildSaveGame->ChoicedTalentIndexes;
				for (FName Index: ChoicedTalentIndexes)
				{
					// 获取对应的Talent结构
					FTalent* Data = TalentData->FindRow<FTalent>(Index, "Find Talent Data");
					if (!Data) continue;

					TArray<FGameplayTag> Tags;
					Data->Effect_GameplayTag.GetKeys(Tags);
					for (auto& Tag: Tags)
					{
						if (Data->Effect_GameplayTag.Contains(Tag))
						{
							
							if (Effect_GameplayTag.Contains(Tag))
							{
								Effect_GameplayTag[Tag] = Effect_GameplayTag[Tag] + Data->Effect_GameplayTag[Tag];
							}
							else
							{
								Effect_GameplayTag.Add(Tag, Data->Effect_GameplayTag[Tag]);
							}
						}
					}
					
				}
			}
		}

		// 对 OnPlayerAttack 的绑定，在玩家发起攻击时的事件
		PXCharacter->OnPlayerAttackStart.AddDynamic(this, &UTalentComponent::Event_OnPlayerAttackStart);
	}
}

void UTalentComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	
	PXCharacter->OnPlayerAttackStart.RemoveDynamic(this, &UTalentComponent::Event_OnPlayerAttackStart);
}


// Called every frame
void UTalentComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UTalentComponent::Event_OnPlayerAttackStart()
{
	// Make Swing Fist Power ？
}

