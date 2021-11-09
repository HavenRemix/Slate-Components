// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Components/SCustomButton.h"
#include "UI/Components/SCustomBorder.h"
#include "UI/Components/SCustomBoxPanel.h"

#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SScaleBox.h"

#include "UI/Style/VILEStyle.h"

#include "VILEGameInstance.h"

void SCustomButton::Construct(const FArguments& InArgs)
{
	// Setup
	bIsPressed = false;
	bIsHovered = false;

	// Arguments
	OnClicked = InArgs._OnClicked;
	OnArrowPressed = InArgs._OnArrowPressed;
	Text = InArgs._Text;
	HelpText = InArgs._HelpText;
	OptionText = InArgs._OptionText;
	bIsMultichoice = InArgs._bIsMultichoice;
	bIsTabButton = InArgs._bIsTabButton;
	ButtonStyle = InArgs._ButtonStyle;
	ButtonIndex = InArgs._ButtonIndex;
	PlayerOwner = InArgs._PlayerOwner.Get();

	LeftArrowVisible = InArgs._bIsMultichoice ? EVisibility::Visible : EVisibility::Hidden;
	RightArrowVisible = InArgs._bIsMultichoice ? EVisibility::Visible : EVisibility::Hidden;

	ChildSlot
	.VAlign(InArgs._ButtonStyle.VAlign)
	.HAlign(InArgs._ButtonStyle.HAlign)
	[
		SNew(SCustomBorder)
		.OnMouseButtonDown(this, &SCustomButton::OnMouseButtonDownBorder)
		.OnMouseButtonUp(this, &SCustomButton::OnMouseButtonUpBorder)
		.OnMouseEnter(this, &SCustomButton::OnMouseEnterBorder)
		.OnMouseLeave(this, &SCustomButton::OnMouseLeaveBorder)
		.BorderImage(FVILEStyle::Get().GetBrush("VILE.DefaultImageBrush"))
		.BorderBackgroundColor(this, &SCustomButton::GetMainColor)
		[
			SNew(SBox)
			.WidthOverride(InArgs._ButtonStyle.Size.X)
			.HeightOverride(InArgs._ButtonStyle.Size.Y)
			[
				SNew(SCustomHorizontalBox)
				+ SCustomHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				[
					SNew(SBox)
					.WidthOverride(ButtonStyle.Icon ? InArgs._ButtonStyle.IconSize.X : 0.0f)
					.HeightOverride(ButtonStyle.Icon ? InArgs._ButtonStyle.IconSize.Y : 0.0f)
					[
						SNew(SCustomBorder)
						.BorderImage(FCoreStyle::Get().GetBrush("NoBorder"))
						.Padding(FMargin(20.0f, 20.0f, 20.0f, 20.0f))
						.Visibility(this, &SCustomButton::GetIconVisibility)
						[
							SNew(SImage)
							.Image(this, &SCustomButton::GetIconBrush)
							.ColorAndOpacity(FLinearColor::White)
						]
					]
				]
				+ SCustomHorizontalBox::Slot()
				.HAlign(InArgs._ButtonStyle.HAlign)
				.VAlign(VAlign_Center)
				.Padding(FMargin(5.0f, 0.0f, 0.0f, 0.0f))
				[
					SNew(SScaleBox)
					.Stretch(EStretch::ScaleToFit)
					[
						SNew(STextBlock)
						.Text(this, &SCustomButton::GetMainText)
						.TextStyle(FVILEStyle::Get(), "VILE.DefaultTextStyle")
					]
				]
				+ SCustomHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(FMargin(0.0f, 0.0f, 10.0f, 0.0f))
				[
					SNew(SBox)
					.WidthOverride(InArgs._ButtonStyle.Size.X * 0.6)
					[
						SNew(SCustomBorder)
						.BorderImage(FCoreStyle::Get().GetBrush("NoBorder"))
						.Visibility(bIsMultichoice ? EVisibility::Visible : EVisibility::Collapsed)
						[
							SNew(SCustomHorizontalBox)
							+ SCustomHorizontalBox::Slot()
							.HAlign(HAlign_Left)
							.VAlign(VAlign_Center)
							[
								SNew(SBox)
								.WidthOverride(ButtonStyle.ArrowSize.X)
								.HeightOverride(ButtonStyle.ArrowSize.Y)
								[
									SNew(SScaleBox)
									.Stretch(EStretch::UserSpecified)
									.UserSpecifiedScale(this, &SCustomButton::GetUserSpecifiedScaleLeftArrow)
									[
										SNew(SCustomBorder)
										.OnMouseButtonDown(this, &SCustomButton::OnMouseButtonDownLeftArrow)
										.OnMouseButtonUp(this, &SCustomButton::OnMouseButtonUpLeftArrow)
										.OnMouseEnter(this, &SCustomButton::OnMouseEnterLeftArrow)
										.OnMouseLeave(this, &SCustomButton::OnMouseLeaveLeftArrow)
										.BorderImage(FCoreStyle::Get().GetBrush("NoBorder"))
										.Visibility(this, &SCustomButton::GetLeftArrowVisible)
										[
											SNew(SImage)
											.Image(FVILEStyle::Get().GetBrush("VILE.LeftArrowImageBrush"))
											.ColorAndOpacity(this, &SCustomButton::GetLeftArrowColor)
										]
									]
								]
							]
							+ SCustomHorizontalBox::Slot()
							.HAlign(HAlign_Fill)
							.AutoWidth()
							.VAlign(VAlign_Fill)
							[
								SNew(SBox)
								.WidthOverride((ButtonStyle.Size.X * 0.6) - (ButtonStyle.ArrowSize.X * 2.0f) - 50.0f)
								[
									SNew(SScaleBox)
									.Stretch(EStretch::ScaleToFit)
									[
										SNew(STextBlock)
										.Text(OptionText)
										.TextStyle(FVILEStyle::Get(), "VILE.DefaultTextStyle")
										.Justification(ETextJustify::Center)
										.Clipping(EWidgetClipping::Inherit)
									]
								]
							]
							+ SCustomHorizontalBox::Slot()
							.HAlign(HAlign_Right)
							.VAlign(VAlign_Center)
							[
								SNew(SBox)
								.WidthOverride(ButtonStyle.ArrowSize.X)
								.HeightOverride(ButtonStyle.ArrowSize.Y)
								[
									SNew(SScaleBox)
									.Stretch(EStretch::UserSpecified)
									.UserSpecifiedScale(this, &SCustomButton::GetUserSpecifiedScaleRightArrow)
									[
										SNew(SCustomBorder)
										.OnMouseButtonDown(this, &SCustomButton::OnMouseButtonDownRightArrow)
										.OnMouseButtonUp(this, &SCustomButton::OnMouseButtonUpRightArrow)
										.OnMouseEnter(this, &SCustomButton::OnMouseEnterRightArrow)
										.OnMouseLeave(this, &SCustomButton::OnMouseLeaveRightArrow)
										.BorderImage(FCoreStyle::Get().GetBrush("NoBorder"))
										.Visibility(this, &SCustomButton::GetRightArrowVisible)
										[
											SNew(SImage)
											.Image(FVILEStyle::Get().GetBrush("VILE.RightArrowImageBrush"))
											.ColorAndOpacity(this, &SCustomButton::GetRightArrowColor)
										]
									]
								]
							]
						]
					]
				]
			]
		]
	];
}


FReply SCustomButton::OnMouseButtonDownBorder(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	bIsPressed = true;

	SetSelectedMenuButton(true);

	if (OnClicked.IsBound())
	{
		return OnClicked.Execute();
	}

	return FReply::Handled();
}

FReply SCustomButton::OnMouseButtonUpBorder(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	bIsPressed = false;

	return FReply::Handled();
}

void SCustomButton::OnMouseEnterBorder(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	SWidget::OnMouseEnter(MyGeometry, MouseEvent);

	SetHoveredMenuButton(true);

	// InternalButton->SetButtonStyle(&FFlareStyleSet::Get().GetWidgetStyle<FButtonStyle>("Flare.ActiveCoreButton"));

	// AFlareMenuManager* MenuManager = AFlareMenuManager::GetSingleton();
	// if (MenuManager)
	// {
	// 	MenuManager->ShowTooltip(this, Text.Get(), HelpText.Get());
	// }
}

void SCustomButton::OnMouseLeaveBorder(const FPointerEvent& MouseEvent)
{
	SWidget::OnMouseLeave(MouseEvent);

	SetHoveredMenuButton(false);

	bIsPressed = false;

	// InternalButton->SetButtonStyle(&FFlareStyleSet::Get().GetWidgetStyle<FButtonStyle>("Flare.CoreButton"));

	// AFlareMenuManager* MenuManager = AFlareMenuManager::GetSingleton();
	// if (MenuManager)
	// {
	// 	MenuManager->HideTooltip(this);
	// }
}

const FSlateBrush* SCustomButton::GetIconBrush() const
{
    return ButtonStyle.Icon;
}

const FSlateBrush* SCustomButton::GetMainBrush() const
{
	return ButtonStyle.DefaultBrush ? ButtonStyle.DefaultBrush : FVILEStyle::Get().GetBrush("VILE.DefaultImageBrush");
}

FSlateColor SCustomButton::GetMainColor() const
{
	FLinearColor NeededColor = bIsPressed ? ButtonStyle.PressedColor : bIsHoveredMenuButton ? ButtonStyle.HoveredColor : bIsSelectedMenuButton ? ButtonStyle.SelectedColor : ButtonStyle.DefaultColor;

	float LerpTime = 0.0f;
	float LerpDuration = bIsPressed ? 0.05f : 0.10f;

	LerpTime += PlayerOwner->GetWorld()->GetDeltaSeconds();

	if (CurrentButtonColor != NeededColor)
	{
		SCustomButton* MutableThis = const_cast<SCustomButton*>(this);
		MutableThis->CurrentButtonColor = FMath::Lerp<FLinearColor>(CurrentButtonColor, NeededColor, LerpTime / LerpDuration);
	}	

	return CurrentButtonColor;
}

float SCustomButton::GetUserSpecifiedScaleLeftArrow() const
{
	float NeededScale = bIsLeftArrowHovered ? 0.65f : 0.6f;

	float LerpTime = 0.0f;
	float LerpDuration = 0.2f;

	LerpTime += PlayerOwner->GetWorld()->GetDeltaSeconds();

	if (CurrentLeftArrowUserSpecifiedScale != NeededScale)
	{
		SCustomButton* MutableThis = const_cast<SCustomButton*>(this);
		MutableThis->CurrentLeftArrowUserSpecifiedScale = FMath::Lerp(CurrentLeftArrowUserSpecifiedScale, NeededScale, LerpTime / LerpDuration);
	}

	return CurrentLeftArrowUserSpecifiedScale;
}

float SCustomButton::GetUserSpecifiedScaleRightArrow() const
{
	float NeededScale = bIsRightArrowHovered ? 0.65f : 0.6f;

	float LerpTime = 0.0f;
	float LerpDuration = 0.2f;

	LerpTime += PlayerOwner->GetWorld()->GetDeltaSeconds();

	if (CurrentRightArrowUserSpecifiedScale != NeededScale)
	{
		SCustomButton* MutableThis = const_cast<SCustomButton*>(this);
		MutableThis->CurrentRightArrowUserSpecifiedScale = FMath::Lerp(CurrentRightArrowUserSpecifiedScale, NeededScale, LerpTime / LerpDuration);
	}

	return CurrentRightArrowUserSpecifiedScale;
}

FText SCustomButton::GetMainText() const
{
	return Text;
}

EVisibility SCustomButton::GetIconVisibility() const
{
	return ButtonStyle.Icon ? EVisibility::Visible : EVisibility::Collapsed;
}

EVisibility SCustomButton::GetLeftArrowVisible() const
{
	return LeftArrowVisible;
}

EVisibility SCustomButton::GetRightArrowVisible() const
{
	return RightArrowVisible;
}

FSlateColor SCustomButton::GetLeftArrowColor() const
{
	return bLeftArrowAvaliable ? FLinearColor::White : FLinearColor::Gray;
}

FSlateColor SCustomButton::GetRightArrowColor() const
{
	return bRightArrowAvaliable ? FLinearColor::White : FLinearColor::Gray;
}

FReply SCustomButton::OnMenuButtonClicked()
{
	// bool WasDisabled = (IsDisabled.IsBound() || IsDisabled.IsSet()) && IsDisabled.Get();
	// if (!WasDisabled && IsHovered())
	// {
	// 	if (IsToggle)
	// 	{
	// 		IsPressed = !IsPressed;
	// 	}

	// 	AFlareMenuManager* MenuManager = AFlareMenuManager::GetSingleton();
	// 	if (MenuManager)
	// 	{
	// 		MenuManager->HideTooltip(this);
	// 	}

	// 	if (OnClicked.IsBound() == true)
	// 	{
	// 		OnClicked.Execute();
	// 	}
	// }

	bIsPressed = true;

	return FReply::Handled();
}

void SCustomButton::SetHoveredMenuButton(bool bIsMenuButtonActive)
{
	bIsHoveredMenuButton = bIsMenuButtonActive;
}

void SCustomButton::SetSelectedMenuButton(bool bIsMenuButtonSelected)
{
	bool bIsSelectable = bIsTabButton;

	if (bIsSelectable)
	{
		bIsSelectedMenuButton = bIsMenuButtonSelected;
	}
}


FReply SCustomButton::OnMouseButtonDownLeftArrow(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (OnArrowPressed.IsBound())
	{
		OnArrowPressed.Execute(-1, ButtonIndex);
	}

	return FReply::Handled();
}

FReply SCustomButton::OnMouseButtonUpLeftArrow(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	return FReply::Handled();
}

void SCustomButton::OnMouseEnterLeftArrow(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	bIsLeftArrowHovered = true;
}

void SCustomButton::OnMouseLeaveLeftArrow(const FPointerEvent& MouseEvent)
{
	bIsLeftArrowHovered = false;
}


FReply SCustomButton::OnMouseButtonDownRightArrow(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (OnArrowPressed.IsBound())
	{
		OnArrowPressed.Execute(1, ButtonIndex);
	}

	return FReply::Handled();
}

FReply SCustomButton::OnMouseButtonUpRightArrow(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	return FReply::Handled();
}

void SCustomButton::OnMouseEnterRightArrow(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	bIsRightArrowHovered = true;
}

void SCustomButton::OnMouseLeaveRightArrow(const FPointerEvent& MouseEvent)
{
	bIsRightArrowHovered = false;
}
