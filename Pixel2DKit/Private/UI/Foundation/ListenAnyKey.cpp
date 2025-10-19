// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Foundation/ListenAnyKey.h"
#include "Framework/Application/IInputProcessor.h"
#include "Framework/Application/IInputProcessor.h"


class FListenAnyKeyInputPreProcessor : public IInputProcessor
{
public:
	FListenAnyKeyInputPreProcessor()
	{

	}

	virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override { }

	virtual bool HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override
	{
		HandleKey(InKeyEvent.GetKey());
		return true;
	}

	virtual bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override
	{
		return true;
	}

	virtual bool HandleMouseButtonDoubleClickEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override
	{
		HandleKey(MouseEvent.GetEffectingButton());
		return true;
	}

	virtual bool HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override
	{
		return true;
	}

	virtual bool HandleMouseButtonUpEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override
	{
		HandleKey(MouseEvent.GetEffectingButton());
		return true;
	}

	virtual bool HandleMouseWheelOrGestureEvent(FSlateApplication& SlateApp, const FPointerEvent& InWheelEvent, const FPointerEvent* InGestureEvent) override
	{
		if (InWheelEvent.GetWheelDelta() != 0)
		{
			const FKey Key = InWheelEvent.GetWheelDelta() < 0 ? EKeys::MouseScrollDown : EKeys::MouseScrollUp;
			HandleKey(Key);
		}
		return true;
	}

	DECLARE_MULTICAST_DELEGATE(FListenAnyKeyInputPreProcessorCanceled);
	FListenAnyKeyInputPreProcessorCanceled OnKeyExit;
	
	DECLARE_MULTICAST_DELEGATE_OneParam(FListenAnyKeyInputPreProcessorKeySelected, FKey);
	FListenAnyKeyInputPreProcessorKeySelected OnKeyPressed;
	
private:
	void HandleKey(const FKey& Key)
	{
		// Cancel this process if it's Escape, Touch, or a gamepad key.
		if (Key == EKeys::LeftCommand || Key == EKeys::RightCommand)
		{
			// Ignore
		}
		else if (Key == EKeys::Escape)
		{
			OnKeyExit.Broadcast();
		}
		else
		{
			OnKeyPressed.Broadcast(Key);
		}
	}
};


UListenAnyKey::UListenAnyKey(const FObjectInitializer& Initializer)
{
}

void UListenAnyKey::NativeOnActivated()
{
	Super::NativeOnActivated();

	bKeySelected = false;

	InputProcessor = MakeShared<FListenAnyKeyInputPreProcessor>();
	InputProcessor->OnKeyPressed.AddUObject(this, &ThisClass::HandleKeyPressed);
	InputProcessor->OnKeyExit.AddUObject(this, &ThisClass::HandleKeyExit);
	FSlateApplication::Get().RegisterInputPreProcessor(InputProcessor, 0);
}

void UListenAnyKey::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();

	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().UnregisterInputPreProcessor(InputProcessor);
	}
}

void UListenAnyKey::HandleKeyPressed(FKey InKey)
{
	OnKeyPressed.Broadcast(InKey);
}

void UListenAnyKey::HandleKeyExit()
{
	FSlateApplication::Get().UnregisterInputPreProcessor(InputProcessor);
	Close();
	OnKeySelectionCanceled.Broadcast();
}
