// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputCoreTypes.h"
#include "Framework/Application/IInputProcessor.h"

UENUM()
enum ECustomInputDevice
{
	KeyboardAndMouse,
	XBoxController,
	PS4Controller,
	Unknown,
};

DECLARE_DELEGATE_OneParam(FOnInputDeviceChangedPreProcessorSignature, ECustomInputDevice);

/**
 * 
 */
class FCustomInputProcessor : public IInputProcessor, public TSharedFromThis<FCustomInputProcessor>
{
public:

	FOnInputDeviceChangedPreProcessorSignature OnInputDeviceChanged;

	ECustomInputDevice DefaultInputDevice = ECustomInputDevice::KeyboardAndMouse;

	float MouseMoveThreshold = 1.0f;
	float GamepadAxisThreshold = 1.0f;

	FCustomInputProcessor();

	virtual bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent);
	virtual bool HandleAnalogInputEvent(FSlateApplication& SlateApp, const FAnalogInputEvent& InAnalogInputEvent);
	virtual bool HandleMouseMoveEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent);
	virtual bool HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent);
	virtual bool HandleMouseWheelOrGestureEvent(FSlateApplication& SlateApp, const FPointerEvent& InWheelEvent, const FPointerEvent* InGestureEvent);

	void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef < ICursor > Cursor);

	ECustomInputDevice GetLastInputDevice();

protected:

	ECustomInputDevice LastUsedInputDevice;

	void ProcessKeyOrButton(FKey Key);
	void ProcessCorrectControllerType();
	void SetDevice(ECustomInputDevice NewInputDevice);
};
