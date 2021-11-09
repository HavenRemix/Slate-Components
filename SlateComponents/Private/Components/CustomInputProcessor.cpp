// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Components/CustomInputProcessor.h"

FCustomInputProcessor::FCustomInputProcessor()
{

}

bool FCustomInputProcessor::HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
	ProcessKeyOrButton(InKeyEvent.GetKey());

	return false;
}

bool FCustomInputProcessor::HandleAnalogInputEvent(FSlateApplication& SlateApp, const FAnalogInputEvent& InAnalogInputEvent)
{
	if (InAnalogInputEvent.GetAnalogValue() > GamepadAxisThreshold)
	{
		ProcessCorrectControllerType();
	}

	return false;
}

bool FCustomInputProcessor::HandleMouseMoveEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent)
{
	SetDevice(ECustomInputDevice::KeyboardAndMouse);

	return false;
}

bool FCustomInputProcessor::HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent)
{
	SetDevice(ECustomInputDevice::KeyboardAndMouse);

	return false;
}

bool FCustomInputProcessor::HandleMouseWheelOrGestureEvent(FSlateApplication& SlateApp, const FPointerEvent& InWheelEvent, const FPointerEvent* InGestureEvent)
{
	SetDevice(ECustomInputDevice::KeyboardAndMouse);

	return false;
}

void FCustomInputProcessor::Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef < ICursor > Cursor)
{

}

ECustomInputDevice FCustomInputProcessor::GetLastInputDevice()
{
	return LastUsedInputDevice;
}

void FCustomInputProcessor::ProcessKeyOrButton(FKey Key)
{
	if (Key.IsGamepadKey())
	{
		ProcessCorrectControllerType();
	}
	else if (Key.IsMouseButton())
	{
		SetDevice(ECustomInputDevice::KeyboardAndMouse);
	}
	else
	{
		SetDevice(ECustomInputDevice::KeyboardAndMouse);
	}
}

void FCustomInputProcessor::ProcessCorrectControllerType()
{
#if PLATFORM_XBOXONE
	SetDevice(ECustomInputDevice::XBoxController);
#elif PLATFORM_PS4
	SetDevice(ECustomInputDevice::PS4Controller);
#elif PLATFORM_WINDOWS
	SetDevice(ECustomInputDevice::XBoxController);
#endif
}

void FCustomInputProcessor::SetDevice(ECustomInputDevice NewInputDevice)
{
	if (NewInputDevice != ECustomInputDevice::Unknown && NewInputDevice != GetLastInputDevice())
	{
		LastUsedInputDevice = NewInputDevice;
		OnInputDeviceChanged.ExecuteIfBound(NewInputDevice);
	}
}

