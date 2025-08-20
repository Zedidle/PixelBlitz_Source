// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/QTE/ArrowLineWidget.h"

#include "SkeletalMeshAttributes.h"

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
