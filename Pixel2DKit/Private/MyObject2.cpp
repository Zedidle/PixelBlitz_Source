// Fill out your copyright notice in the Description page of Project Settings.


#include "MyObject2.h"
#include "MyObject.h"

float UMyObject2::GetHeight2()
{
	o1 = NewObject<UMyObject>(this);
	return o1->GetHeight();
	
}
