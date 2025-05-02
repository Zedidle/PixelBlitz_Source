// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/BasePixelAnimInstance.h"
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

void UBasePixelAnimInstance::SetActionField(const FGameplayTag& field)
{
	CurActionFiled = field;
}


void UBasePixelAnimInstance::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (IsValid(GetOwningActor()))
	{
		if (AAIController* AIController = UAIBlueprintHelperLibrary::GetAIController(GetOwningActor()))
		{

			eMoveStatus = AIController->GetMoveStatus();
			bMoving = eMoveStatus == 3; 
		}
		else
		{
			// 处理没有 AI 控制器的情况
			eMoveStatus = 0; // 或者其他默认值
		}
	}
	else
	{
		eMoveStatus = 0; // 或者其他默认值
	}
}

