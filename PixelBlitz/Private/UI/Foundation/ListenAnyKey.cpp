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
		KeyReleased(InKeyEvent.GetKey());
		return true;
	}

	virtual bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override
	{
		KeyPressed(InKeyEvent.GetKey());
		return true;
	}

	virtual bool HandleMouseButtonDoubleClickEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override
	{
		KeyPressed(MouseEvent.GetEffectingButton());
		return true;
	}

	virtual bool HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override
	{
		return true;
	}

	virtual bool HandleMouseButtonUpEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override
	{
		KeyPressed(MouseEvent.GetEffectingButton());
		return true;
	}

	virtual bool HandleMouseWheelOrGestureEvent(FSlateApplication& SlateApp, const FPointerEvent& InWheelEvent, const FPointerEvent* InGestureEvent) override
	{
		if (InWheelEvent.GetWheelDelta() != 0)
		{
			const FKey Key = InWheelEvent.GetWheelDelta() < 0 ? EKeys::MouseScrollDown : EKeys::MouseScrollUp;
			KeyPressed(Key);
		}
		return true;
	}

	DECLARE_MULTICAST_DELEGATE(FListenAnyKeyInput_Exit);
	FListenAnyKeyInput_Exit OnKeyExit;
	
	DECLARE_MULTICAST_DELEGATE_OneParam(FListenAnyKeyInput_Pressed, FKey);
	FListenAnyKeyInput_Pressed OnKeyPressed;

	DECLARE_MULTICAST_DELEGATE_OneParam(FListenAnyKeyInput_Released, FKey);
	FListenAnyKeyInput_Released OnKeyReleased;
	
private:
	void KeyPressed(const FKey& Key)
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

	void KeyReleased(const FKey& Key)
	{
		OnKeyReleased.Broadcast(Key);
	}
};


UListenAnyKey::UListenAnyKey(const FObjectInitializer& Initializer)
{
}

void UListenAnyKey::NativeOnActivated()
{
	Super::NativeOnActivated();

	InputProcessor = MakeShared<FListenAnyKeyInputPreProcessor>();
	InputProcessor->OnKeyPressed.AddUObject(this, &ThisClass::HandleKeyPressed);
	InputProcessor->OnKeyReleased.AddUObject(this, &ThisClass::HandleKeyReleased);
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

void UListenAnyKey::HandleKeyReleased(FKey InKey)
{
	OnKeyReleased.Broadcast(InKey);
}

void UListenAnyKey::HandleKeyExit()
{
	FSlateApplication::Get().UnregisterInputPreProcessor(InputProcessor);
	Close();
	OnKeySelectionCanceled.Broadcast();
}
