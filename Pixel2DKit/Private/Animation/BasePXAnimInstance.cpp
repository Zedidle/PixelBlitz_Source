// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/BasePXAnimInstance.h"
#include "AIController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"


// void UBasePixelAnimInstance::PostInitProperties()
// {
// 	Super::PostInitProperties();
// 	UClass* Class = GetClass();
// 	for (TFieldIterator<FProperty> It(Class); It; ++It) {
// 		FProperty* Prop = *It;
// 		CachedProperties.Add(Prop->GetFName(), *Prop);
// 	}
// }

void UBasePXAnimInstance::SetActionField(const FGameplayTag& field)
{
	CurActionFiled = field;
}


void UBasePXAnimInstance::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (IsValid(GetOwningActor()))
	{
		if (AAIController* AIController = UAIBlueprintHelperLibrary::GetAIController(GetOwningActor()))
		{

			eMoveStatus = AIController->GetMoveStatus();
			bMoving = eMoveStatus == EPathFollowingStatus::Moving; 
		}
		else
		{
			// 处理没有 AI 控制器的情况
			eMoveStatus = EPathFollowingStatus::Paused; // 或者其他默认值
		}
	}
	else
	{
		eMoveStatus = EPathFollowingStatus::Paused; // 或者其他默认值
	}
}

