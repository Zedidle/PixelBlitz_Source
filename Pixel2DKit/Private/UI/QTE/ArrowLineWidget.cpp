// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/QTE/ArrowLineWidget.h"

#include "SkeletalMeshAttributes.h"
#include "Interfaces/Fight_Interface.h"
#include "Kismet/KismetMathLibrary.h"
#include "Pixel2DKit/Pixel2DKit.h"
#include "Subsystems/TimerManagerFuncLib.h"
#include "UI/QTE/ArrowLineBodyWidget.h"
#include "UI/QTE/ArrowLineHeadAntiWidget.h"
#include "UI/QTE/ArrowLineHeadWidget.h"
#include "Utilitys/SpaceFuncLib.h"

void UArrowLineWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (LifeSpan > 0)
	{
		UTimerManagerFuncLib::SetDelay(GetWorld(),[WeakThis = TWeakObjectPtr(this)]
		{
			if (WeakThis.IsValid())
			{
				WeakThis->RemoveArrowLine();
			}
		}, LifeSpan);
	}

	if (UArrowLineHeadWidget* Widget = CreateWidget<UArrowLineHeadWidget>(this, ArrowHeadWidgetClass))
	{
		Widget->Init(Color, HeadSize);
		ArrowParts.Add(Widget);
		Widget->AddToViewport(100);
	}

	for (int i = 0; i<MaxArrowBodyNum; i++)
	{
		if (UArrowLineBodyWidget* Widget = CreateWidget<UArrowLineBodyWidget>(this, ArrowBodyWidgetClass))
		{
			Widget->Init(Color, BodySize);
			ArrowParts.Add(Widget);
			Widget->AddToViewport(100);
		}
	}
	if (TwoWay)
	{
		if (UArrowLineHeadAntiWidget* Widget = CreateWidget<UArrowLineHeadAntiWidget>(this, ArrowHeadAntiWidgetClass))
		{
			ArrowHeadAntiWidget = Widget;
			ArrowHeadAntiWidget->Init(Color, HeadSize);
			ArrowHeadAntiWidget->AddToViewport(100);
		}
	}
	Tick_Update();
}

void UArrowLineWidget::NativeDestruct()
{
	Super::NativeDestruct();

}

void UArrowLineWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	Tick_Update();
}

void UArrowLineWidget::RemoveArrowLine()
{
	if (ArrowHeadAntiWidget)
	{
		ArrowHeadAntiWidget->SetVisibility(ESlateVisibility::Collapsed);
		ArrowHeadAntiWidget->RemoveFromParent();
	}
	
	if (ArrowParts.IsEmpty())
	{
		RemoveFromParent();
		return;
	}

	for (UUserWidget* Widget : ArrowParts)
	{
		if (Widget)
		{
			Widget->SetVisibility(ESlateVisibility::Collapsed);
			Widget->RemoveFromParent();
		}
	}
	ArrowParts.Empty();

	if (GetParent())
	{
		SetVisibility(ESlateVisibility::Collapsed);
		RemoveFromParent();
	}
}


void UArrowLineWidget::SetHide(bool bHide)
{
	ESlateVisibility Visibele = bHide ? ESlateVisibility::Collapsed : ESlateVisibility::HitTestInvisible;
	
	if (!ArrowParts.IsEmpty())
	{
		for (UUserWidget* Widget : ArrowParts)
		{
			Widget->SetVisibility(Visibele);
		}
	}
	if (ArrowHeadAntiWidget)
	{
		ArrowHeadAntiWidget->SetVisibility(Visibele);
	}
}

void UArrowLineWidget::Tick_Update()
{
	UWorld* World = GetWorld();
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(World)

	if (ArrowParts.IsEmpty()) return;

	if (!StartActor || !EndActor)
	{
		SetHide(true);
		return;
	}
	
	// 如果开始是活的，后来没活
	if (StartActor->Implements<UFight_Interface>() && !IFight_Interface::Execute_IsAlive(StartActor))
	{
		SetHide(true);
		return;
	}
	if (EndActor->Implements<UFight_Interface>() && !IFight_Interface::Execute_IsAlive(EndActor))
	{
		SetHide(true);
		return;
	}

	if (!USpaceFuncLib::IsActorInScreen(World, StartActor, -0.02f, StartOffset) || !USpaceFuncLib::IsActorInScreen(World, EndActor, -0.02f))
	{
		SetHide(true);
		return;
	}

	float Distance = FVector::Distance(StartActor->GetActorLocation() + StartOffset, EndActor->GetActorLocation());
	float BodyNumFactor = FMath::Min(1.0f, Distance / MaxDistance);
	TmpMaxBodyNum = FMath::Max(3,  BodyNumFactor* MaxArrowBodyNum);

	for (int i = 0 ; i < ArrowParts.Num(); i++)
	{
		if (!ArrowParts[i]) continue;
		
		if (i < TmpMaxBodyNum)
		{
			TmpIndex = i;
			UpdateWidget(ArrowParts[i]);
		}
		else
		{
			ArrowParts[i]->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	if (TwoWay)
	{
		if (ArrowParts[TmpIndex])
		{
			ArrowParts[TmpIndex]->SetVisibility(ESlateVisibility::Collapsed);
		}

		TmpIndex = TmpMaxBodyNum - 1;
		UpdateWidget(ArrowHeadAntiWidget);
	}
	
}

void UArrowLineWidget::UpdateWidget(UUserWidget* Widget)
{
	CHECK_RAW_POINTER_IS_VALID_OR_RETURN(Widget);
	
	Widget->SetVisibility(ESlateVisibility::HitTestInvisible);
	if (Widget->Implements<UUI_Interface>())
	{
		FVector2D Distance2D = USpaceFuncLib::GetDistance2D_InScreen(EndActor, StartActor, 0,  FVector::ZeroVector, StartOffset);
		
		bool StartActorAtRight = Distance2D.X < 0;
		float Angle = (StartActorAtRight ? -1 : 1) * USpaceFuncLib::CalAngle2D(Distance2D, FVector2D(0, -1));
		
		IUI_Interface::Execute_SetAngle(Widget, Angle);

		FVector2D StartPosition = USpaceFuncLib::GetActorPositionInScreen(GetWorld(), StartActor, StartOffset);
		FVector2D WidgetPosition = StartPosition + Distance2D * FVector2D(float(TmpMaxBodyNum - TmpIndex) / float(TmpMaxBodyNum + 1) );
		
		Widget->SetPositionInViewport(WidgetPosition);
	}	
}

void UArrowLineWidget::SetStartActor(AActor* Actor)
{
	StartActor = Actor;
}

void UArrowLineWidget::SetEndActor(AActor* Actor)
{
	EndActor = Actor;
}

void UArrowLineWidget::InitializeData(AActor* _StartActor, AActor* _EndActor, bool _TwoWay, int _MaxArrowBodyNum,
                                      float _MaxDistance, FLinearColor _Color, float _LifeSpan, FVector2D _BodySize, FVector2D _HeadSize, FVector _StartOffset)
{
	StartActor = _StartActor;
	EndActor = _EndActor;
	TwoWay = _TwoWay;
	MaxArrowBodyNum = _MaxArrowBodyNum;
	MaxDistance = _MaxDistance;
	Color = _Color;
	LifeSpan = _LifeSpan;
	BodySize = _BodySize;
	HeadSize = _HeadSize;
	StartOffset = _StartOffset;
}
