// Fill out your copyright notice in the Description page of Project Settings.


#include "Anim/BasePixelAnimInstance.h"
#include "AIController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"


void UBasePixelAnimInstance::SetDead(bool V)
{
	bDead = V;
}

void UBasePixelAnimInstance::SetMoving(bool V)
{
	bMoving = V;
}

void UBasePixelAnimInstance::SetJumping(bool V)
{
	bJumping = V;
}

void UBasePixelAnimInstance::SetFalling(bool V)
{
	bFalling = V;
}

void UBasePixelAnimInstance::SetLanding(bool V)
{
	bLanding = V;
}

void UBasePixelAnimInstance::SetInAttackState(bool V)
{
	bInAttackState = V;
}

void UBasePixelAnimInstance::SetAttackAnimToggle(bool V)
{
	bAttackAnimToggle = V;
}

void UBasePixelAnimInstance::SetInAttackEffect(bool V)
{
	bInAttackEffect = V;
}

void UBasePixelAnimInstance::SetAttackHolding(bool V)
{
	bAttackHolding = V;
}

void UBasePixelAnimInstance::SetAttackFire(bool V)
{
	bAttackFire = V;
}

void UBasePixelAnimInstance::SetDashing(bool V)
{
	bDashing = V;
}

void UBasePixelAnimInstance::SetHurt(bool V)
{
	bHurt = V;
}


void UBasePixelAnimInstance::SetActionField(const EActionField field)
{
	CurActionFiled = field;
}


void UBasePixelAnimInstance::SetDefendState(const bool V)
{
	bInDefendState = V;
}

void UBasePixelAnimInstance::SetDefendStart(const bool V)
{
	bDefendStart = V;
}

void UBasePixelAnimInstance::SetDefendHurt(const bool V)
{
	bDefendHurt = V;
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

