// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Components/SCustomBorder.h"
#include "Rendering/DrawElements.h"

static FName SCustomBorderTypeName("SCustomBorder");

SCustomBorder::SCustomBorder()
	: BorderImage(FCoreStyle::Get().GetBrush("Border"))
	, BorderBackgroundColor(FLinearColor::White)
	, DesiredSizeScale(FVector2D(1, 1))
{
}

void SCustomBorder::Construct(const SCustomBorder::FArguments& InArgs)
{
	// Only do this if we're exactly an SCustomBorder
	if (GetType() == SCustomBorderTypeName)
	{
		SetCanTick(false);
		bCanSupportFocus = false;
	}

	ContentScale = InArgs._ContentScale;
	ColorAndOpacity = InArgs._ColorAndOpacity;
	DesiredSizeScale = InArgs._DesiredSizeScale;

	ShowDisabledEffect = InArgs._ShowEffectWhenDisabled;

	bFlipForRightToLeftFlowDirection = InArgs._FlipForRightToLeftFlowDirection;

	BorderImage = InArgs._BorderImage;
	BorderBackgroundColor = InArgs._BorderBackgroundColor;
	ForegroundColor = InArgs._ForegroundColor;

	if (InArgs._OnMouseButtonDown.IsBound())
	{
		SetOnMouseButtonDown(InArgs._OnMouseButtonDown);
	}

	if (InArgs._OnMouseButtonUp.IsBound())
	{
		SetOnMouseButtonUp(InArgs._OnMouseButtonUp);
	}

	if (InArgs._OnMouseEnter.IsBound())
	{
		SetOnMouseEnter(InArgs._OnMouseEnter);
	}

	if (InArgs._OnMouseLeave.IsBound())
	{
		SetOnMouseLeave(InArgs._OnMouseLeave);
	}

	ChildSlot
	.HAlign(InArgs._HAlign)
	.VAlign(InArgs._VAlign)
	.Padding(InArgs._Padding)
	[
		InArgs._Content.Widget
	];
}

int32 SCustomBorder::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	const FSlateBrush* BrushResource = BorderImage.Get();

	const bool bEnabled = ShouldBeEnabled(bParentEnabled);

	if (BrushResource && BrushResource->DrawAs != ESlateBrushDrawType::NoDrawType)
	{
		const bool bShowDisabledEffect = ShowDisabledEffect.Get();
		const ESlateDrawEffect DrawEffects = (bShowDisabledEffect && !bEnabled) ? ESlateDrawEffect::DisabledEffect : ESlateDrawEffect::None;

		if (bFlipForRightToLeftFlowDirection && GSlateFlowDirection == EFlowDirection::RightToLeft)
		{
			const FGeometry FlippedGeometry = AllottedGeometry.MakeChild(FSlateRenderTransform(FScale2D(-1, 1)));
			FSlateDrawElement::MakeBox(
				OutDrawElements,
				LayerId,
				FlippedGeometry.ToPaintGeometry(),
				BrushResource,
				DrawEffects,
				BrushResource->GetTint(InWidgetStyle) * InWidgetStyle.GetColorAndOpacityTint() * BorderBackgroundColor.Get().GetColor(InWidgetStyle)
			);
		}
		else
		{
			FSlateDrawElement::MakeBox(
				OutDrawElements,
				LayerId,
				AllottedGeometry.ToPaintGeometry(),
				BrushResource,
				DrawEffects,
				BrushResource->GetTint(InWidgetStyle) * InWidgetStyle.GetColorAndOpacityTint() * BorderBackgroundColor.Get().GetColor(InWidgetStyle)
			);
		}
	}

	return SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bEnabled);
}

bool SCustomBorder::ComputeVolatility() const
{
	return BorderImage.IsBound() || BorderBackgroundColor.IsBound() || DesiredSizeScale.IsBound() || ShowDisabledEffect.IsBound();
}

FVector2D SCustomBorder::ComputeDesiredSize(float LayoutScaleMultiplier) const
{
	return DesiredSizeScale.Get() * SCompoundWidget::ComputeDesiredSize(LayoutScaleMultiplier);
}