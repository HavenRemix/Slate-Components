// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Misc/Attribute.h"
#include "Styling/SlateColor.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SWidget.h"
#include "Layout/Margin.h"
#include "Widgets/SCompoundWidget.h"
#include "Styling/CoreStyle.h"
#include "Styling/SlateTypes.h"

/**
 * 
 */
class SCustomBorder : public SCompoundWidget
{
public:
	
	SLATE_BEGIN_ARGS(SCustomBorder)
		: _Content()
		, _HAlign(HAlign_Fill)
		, _VAlign(VAlign_Fill)
		, _Padding(FMargin(2.0f))
		, _OnMouseButtonDown()
		, _OnMouseButtonUp()
		, _OnMouseEnter()
		, _OnMouseLeave()
		, _BorderImage(FCoreStyle::Get().GetBrush("Border"))
		, _ContentScale(FVector2D(1, 1))
		, _DesiredSizeScale(FVector2D(1, 1))
		, _ColorAndOpacity(FLinearColor(1, 1, 1, 1))
		, _BorderBackgroundColor(FLinearColor::White)
		, _ForegroundColor(FSlateColor::UseForeground())
		, _ShowEffectWhenDisabled(true)
		, _FlipForRightToLeftFlowDirection(false)
	{}

	SLATE_DEFAULT_SLOT(FArguments, Content)

		SLATE_ARGUMENT(EHorizontalAlignment, HAlign)
		SLATE_ARGUMENT(EVerticalAlignment, VAlign)
		SLATE_ATTRIBUTE(FMargin, Padding)

		SLATE_EVENT(FPointerEventHandler, OnMouseButtonDown)
		SLATE_EVENT(FPointerEventHandler, OnMouseButtonUp)
		SLATE_EVENT(FNoReplyPointerEventHandler, OnMouseEnter)
		SLATE_EVENT(FSimpleNoReplyPointerEventHandler, OnMouseLeave)

		SLATE_ATTRIBUTE(const FSlateBrush*, BorderImage)

		SLATE_ATTRIBUTE(FVector2D, ContentScale)

		SLATE_ATTRIBUTE(FVector2D, DesiredSizeScale)

		/** ColorAndOpacity is the color and opacity of content in the border */
		SLATE_ATTRIBUTE(FLinearColor, ColorAndOpacity)
		/** BorderBackgroundColor refers to the actual color and opacity of the supplied border image.*/
		SLATE_ATTRIBUTE(FSlateColor, BorderBackgroundColor)
		/** The foreground color of text and some glyphs that appear as the border's content. */
		SLATE_ATTRIBUTE(FSlateColor, ForegroundColor)
		/** Whether or not to show the disabled effect when this border is disabled */
		SLATE_ATTRIBUTE(bool, ShowEffectWhenDisabled)

		/** Flips the background image if the localization's flow direction is RightToLeft */
		SLATE_ARGUMENT(bool, FlipForRightToLeftFlowDirection)
		SLATE_END_ARGS()

		/**
		 * Default constructor.
		 */
		SCustomBorder();

	/**
	 * Construct this widget
	 *
	 * @param	InArgs	The declaration data for this widget
	 */
	void Construct(const FArguments& InArgs);

public:
	// SWidget interface
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	virtual bool ComputeVolatility() const override;
	// End of SWidget interface

protected:
	// Begin SWidget overrides.
	virtual FVector2D ComputeDesiredSize(float) const override;
	// End SWidget overrides.

protected:
	FInvalidatableBrushAttribute BorderImage;
	TAttribute<FSlateColor> BorderBackgroundColor;
	TAttribute<FVector2D> DesiredSizeScale;
	/** Whether or not to show the disabled effect when this border is disabled */
	TAttribute<bool> ShowDisabledEffect;

	/** Flips the image if the localization's flow direction is RightToLeft */
	bool bFlipForRightToLeftFlowDirection;
};
