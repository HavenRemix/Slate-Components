// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Widgets/SCompoundWidget.h"

typedef TArray<TSharedPtr<class FCustomButton>> StoredMenu;
typedef TArray<TSharedPtr<class FCustomTab>> StoredTabs;

enum ECustomButtonType
{
	Standard,
	Multichoice,
	Display,
	Root,
};

struct FCustomButtonStyle
{
	/**The button's color when left alone*/
	FLinearColor DefaultColor;

	/**The button's color when hovered*/
	FLinearColor HoveredColor;

	/**The button's color when pressed*/
	FLinearColor PressedColor;

	/**The button's color when selected*/
	FLinearColor SelectedColor;

	/**The button's color when disabled*/
	FLinearColor DisabledColor;

	/**The button's image when left alone*/
	FSlateBrush* DefaultBrush;

	/**The button's image when hovered*/
	FSlateBrush* HoveredBrush;

	/**The button's image when pressed*/
	FSlateBrush* PressedBrush;

	/**The button's brush when selected*/
	FSlateBrush* SelectedBrush;

	/**The button's image when disabled*/
	FSlateBrush* DisabledBrush;

	/**The button's icon*/
	const FSlateBrush* Icon;

	/**The size of the button*/
	FVector2D Size;

	/**The size of the button icon*/
	FVector2D IconSize;

	/**The size of the arrows*/
	FVector2D ArrowSize;

	/**The horizontal alignment of the button*/
	EHorizontalAlignment HAlign;

	/**The vertical alignment of the button*/
	EVerticalAlignment VAlign;

	FCustomButtonStyle()
		: DefaultColor(FLinearColor::White)
		, HoveredColor(FLinearColor::White)
		, PressedColor(FLinearColor::White)
		, SelectedColor(FLinearColor::White)
		, DisabledColor(FLinearColor::White)
		, Icon(NULL)
		, Size(FVector2D(200.0f, 50.0f))
		, IconSize(FVector2D(40.0f, 40.0f))
		, ArrowSize(FVector2D(40.0f, 40.0f))
	{
	};
};

class FCustomTab : public TSharedFromThis<FCustomTab>
{
public:
	
	/**The menu which is stored in this tab*/
	StoredMenu MenuStored;

	/**Is this a root tab*/
	bool bIsRootTab;

	FCustomTab(const FText& _Text)
	{
		//MenuStored = _MenuStored;
		Text = _Text;
		bIsRootTab = true;
	}

	FCustomTab(FCustomButtonStyle _ButtonStyle, const FText& _Text, const FText& _HelpText = FText::GetEmpty())
	{
		ButtonStyle = _ButtonStyle;
		Text = _Text;
		HelpText = _HelpText;
		//MenuStored = _MenuStored;
		bIsRootTab = false;
	}

	TSharedPtr<class SCustomButton> Widget;

	const FText& GetText() const { return Text; };

	const FText& GetHelpText() const { return HelpText; };

	const FCustomButtonStyle& GetButtonStyle() const { return ButtonStyle; };

private:

	FCustomButtonStyle ButtonStyle;

	FText Text;
	FText HelpText;
};

class FCustomButton : public TSharedFromThis<FCustomButton>
{
public:

	DECLARE_DELEGATE(FOnCustomButtonClicked);

	DECLARE_DELEGATE_TwoParams(FOnCustomButtonOptionChanged, TSharedPtr<FCustomButton>, int32);

	DECLARE_DELEGATE(FOnCustomButtonControllerFacebuttonLeftPressed);

	DECLARE_DELEGATE(FOnCustomButtonControllerDownInputPressed);

	DECLARE_DELEGATE(FOnCustomButtonControllerUpInputPressed);

	DECLARE_DELEGATE(FOnCustomButtonControllerFacebuttonDownPressed);

	FOnCustomButtonControllerFacebuttonLeftPressed OnControllerFacebuttonLeftPressed;

	FOnCustomButtonControllerDownInputPressed OnControllerDownInputPressed;

	FOnCustomButtonControllerUpInputPressed OnControllerUpInputPressed;

	FOnCustomButtonControllerFacebuttonDownPressed OnControllerFacebuttonDownPressed;

	FOnCustomButtonClicked OnCustomButtonClicked;

	FOnCustomButtonOptionChanged OnCustomButtonOptionChanged;

	TArray<TSharedPtr<FCustomButton>> SubMenu;

	int32 SelectedMultiChoice;
	int32 MaxMultiChoiceIndex;
	int32 MinMultiChoiceIndex;

	FCustomButton(FCustomButtonStyle _ButtonStyle, const FText& _Text, const FText& _HelpText = FText::GetEmpty())
	{
		ButtonStyle = _ButtonStyle;
		Text = _Text;
		HelpText = _HelpText;
		ButtonType = ECustomButtonType::Standard;
	}

	FCustomButton(FCustomButtonStyle _ButtonStyle, const FText& _Text, const TArray<FText>& _OptionText, const FText& _HelpText = FText::GetEmpty())
	{
		ButtonStyle = _ButtonStyle;
		Text = _Text;
		OptionText = _OptionText;
		HelpText = _HelpText;
		MinMultiChoiceIndex = MaxMultiChoiceIndex = -1;
		SelectedMultiChoice = 0;
		ButtonType = ECustomButtonType::Multichoice;
	}

	static TSharedRef<FCustomButton> CreateRoot()
	{
		return MakeShareable(new FCustomButton());
	}

	TSharedPtr<class SCustomButton> Widget;

	const FText& GetText() const {return Text;};

	const FText& GetHelpText() const {return HelpText;};

	const FCustomButtonStyle& GetButtonStyle() const {return ButtonStyle;};

	TArray<FText> OptionText;

	ECustomButtonType ButtonType;

private:

	FCustomButton()
	{
		ButtonType = ECustomButtonType::Root;
	}

	FCustomButtonStyle ButtonStyle;

	FText Text;
	FText HelpText;
};

/**
 * Slate's Buttons are clickable Widgets that can contain arbitrary widgets as its Content().
 */
class SCustomButton : public SCompoundWidget
{
public:

	DECLARE_DELEGATE_TwoParams(FOnArrowPressed, int, int);

	SLATE_BEGIN_ARGS(SCustomButton)
	{}

	SLATE_ATTRIBUTE(FText, OptionText)

	SLATE_EVENT(FOnClicked, OnClicked)
	SLATE_EVENT(FOnArrowPressed, OnArrowPressed)

	SLATE_ARGUMENT(TWeakObjectPtr<ULocalPlayer>, PlayerOwner)
	SLATE_ARGUMENT(FText, Text)
	SLATE_ARGUMENT(FText, HelpText)
	SLATE_ARGUMENT(FCustomButtonStyle, ButtonStyle)
	SLATE_ARGUMENT(bool, bIsMultichoice)
	SLATE_ARGUMENT(bool, bIsTabButton)
	SLATE_ARGUMENT(int32, ButtonIndex)
	
	SLATE_END_ARGS()


public:

	/** Create the widget */
	void Construct(const FArguments& InArgs);

	/** Force a new text */
	void SetText(FText NewText) {Text = NewText;};

	/** Force a new help text */
	void SetHelpText(FText NewText) {HelpText = NewText;};
	
	/** Mouse click	*/
	FReply OnMenuButtonClicked();

	/**Sets this as the hovered menu button*/
	void SetHoveredMenuButton(bool bIsActiveMenuButton);

	/**Sets this as the selected menu button*/
	void SetSelectedMenuButton(bool bIsSelectedMenuButton);

	EVisibility LeftArrowVisible;
	EVisibility RightArrowVisible;

	/**Is the left arrow avaliable*/
	bool bLeftArrowAvaliable;

	/**Is the right arrow avaliable*/
	bool bRightArrowAvaliable;

private:

	FLinearColor CurrentButtonColor;

	FOnClicked OnClicked;
	FOnArrowPressed OnArrowPressed;

	const FSlateBrush* GetIconBrush() const;
	const FSlateBrush* GetMainBrush() const;

	EVisibility GetLeftArrowVisible() const;
	EVisibility GetRightArrowVisible() const;

	FSlateColor GetLeftArrowColor() const;
	FSlateColor GetRightArrowColor() const;

	FSlateColor GetMainColor() const;

	float GetUserSpecifiedScaleLeftArrow() const;
	float GetUserSpecifiedScaleRightArrow() const;

	FText GetMainText() const;

	EVisibility GetIconVisibility() const;

	/**Is this a multichoice button*/
	bool bIsMultichoice;

	/**Is this a tab button*/
	bool bIsTabButton;

	/**Is this menu button currently hovered*/
	bool bIsHoveredMenuButton;

	/**Is this menu button currently selected*/
	bool bIsSelectedMenuButton;

	/**Is the button currently pressed*/
	bool bIsPressed;

	/**Is the left arrow being hovered on*/
	bool bIsLeftArrowHovered;

	/**Is the right arrow being hovered on*/
	bool bIsRightArrowHovered;

	/**The current left arrow scale*/
	float CurrentLeftArrowUserSpecifiedScale;

	/**The current right arrow scale*/
	float CurrentRightArrowUserSpecifiedScale;

	/**The style for this button*/
	FCustomButtonStyle ButtonStyle;

	/**The owning player*/
	ULocalPlayer* PlayerOwner;

	/**The text for this button*/
	FText Text;

	/**The tooltip text for this button*/
	FText HelpText;

	/**The option text*/
	TAttribute<FText> OptionText;

	/**The index for this button*/
	int32 ButtonIndex;

	FReply OnMouseButtonDownBorder(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
	FReply OnMouseButtonUpBorder(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
	void OnMouseEnterBorder(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
	void OnMouseLeaveBorder(const FPointerEvent& MouseEvent);

	FReply OnMouseButtonDownLeftArrow(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
	FReply OnMouseButtonUpLeftArrow(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
	void OnMouseEnterLeftArrow(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
	void OnMouseLeaveLeftArrow(const FPointerEvent& MouseEvent);

	FReply OnMouseButtonDownRightArrow(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
	FReply OnMouseButtonUpRightArrow(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
	void OnMouseEnterRightArrow(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
	void OnMouseLeaveRightArrow(const FPointerEvent& MouseEvent);
};