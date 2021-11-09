// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UI/Components/SCustomButton.h"
#include "UI/Components/CustomInputProcessor.h"

typedef TArray<TSharedPtr<class FCustomButton>> StoredMenu;
typedef TArray<TSharedPtr<class FCustomTab>> StoredTabs;

struct FCustomMenuWidgetStyle
{
	/**The horizontal alignment of the widget*/
	EHorizontalAlignment HAlign;

	/**The vertical alignment of the widget*/
	EVerticalAlignment VAlign;

	/**The horizontal alignment of the secondary menu*/
	EHorizontalAlignment SecondaryMenuHAlign;

	/**The vertical alignment of the secondary menu*/
	EVerticalAlignment SecondaryMenuVAlign;

	/**The horizontal alignment of the tabs menu*/
	EHorizontalAlignment TabsMenuHAlign;

	/**The vertical alignment of the tabs menu*/
	EVerticalAlignment TabsMenuVAlign;

	/**The horizontal alignment of the background*/
	EHorizontalAlignment BackgroundHAlign;

	/**The vertical alignment of the background*/
	EVerticalAlignment BackgroundVAlign;

	/**The padding for the menu*/
	FMargin MenuHolderPadding;

	/**The padding for the menu title*/
	FMargin MenuTitlePadding;

	/**The padding for the secondary menu*/
	FMargin SecondaryMenuPadding;

	/**The padding for the tabs menu*/
	FMargin TabMenuPadding;

	/**The padding for the background*/
	FMargin BackgroundPadding;

	/**The color of the background*/
	FLinearColor BackgroundColor;

	/**The brush of the background*/
	FSlateBrush* BackgroundBrush;

	FCustomMenuWidgetStyle()
		: MenuHolderPadding(FMargin(0.0f, 0.0f, 0.0f, 0.0f))
		, MenuTitlePadding(FMargin(0.0f, 0.0f, 0.0f, 0.0f))
		, SecondaryMenuPadding(FMargin(0.0f, 0.0f, 0.0f, 0.0f))
		, TabMenuPadding(FMargin(0.0f, 0.0f, 0.0f, 0.0f))
		, BackgroundPadding(FMargin(0.0f, 0.0f, 0.0f, 0.0f))
		, BackgroundColor(FLinearColor::Black)
		, HAlign(HAlign_Center)
		, VAlign(VAlign_Top)
		, SecondaryMenuHAlign(HAlign_Right)
		, SecondaryMenuVAlign(VAlign_Bottom)
		, TabsMenuHAlign(HAlign_Left)
		, TabsMenuVAlign(VAlign_Top)
		, BackgroundHAlign(HAlign_Fill)
		, BackgroundVAlign(VAlign_Fill)
	{
	};
};

/**
 * 
 */
class SCustomMenuWidgetBase : public SCompoundWidget
{
public:

	DECLARE_DELEGATE(FOnMenuGoBack);

	DECLARE_DELEGATE(FOnCloseMenu);

	DECLARE_DELEGATE(FOnControllerFacebuttonLeftPressed);

	DECLARE_DELEGATE(FOnControllerFacebuttonRightPressed);

	DECLARE_DELEGATE(FOnControllerFacebuttonUpPressed);

	DECLARE_DELEGATE(FOnControllerFacebuttonDownPressed);

	FOnControllerFacebuttonLeftPressed OnControllerFacebuttonLeftPressed;

	FOnControllerFacebuttonRightPressed OnControllerFacebuttonRightPressed;

	FOnControllerFacebuttonUpPressed OnControllerFacebuttonUpPressed;

	FOnControllerFacebuttonDownPressed OnControllerFacebuttonDownPressed;

	SLATE_BEGIN_ARGS(SCustomMenuWidgetBase)
		: _PlayerOwner()
	{
	}

	/** weak pointer to the parent HUD base */
	SLATE_ARGUMENT(TWeakObjectPtr<ULocalPlayer>, PlayerOwner)

	/**The menu widget style*/
	SLATE_ARGUMENT(FCustomMenuWidgetStyle, MenuWidgetStyle)

	/**The title of the menu*/
	SLATE_ARGUMENT(FText, MenuTitle)

	/** always goes here */
	SLATE_END_ARGS()

	/** every widget needs a construction function */
	virtual void Construct(const FArguments& InArgs);

	/** update function. Kind of a hack. Allows us to only start fading in once we are done loading. */
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

	/** to have the mouse cursor show up at all times, we need the widget to handle all mouse events */
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	/** key down handler */
	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;

	/** says that we can support keyboard focus */
	virtual bool SupportsKeyboardFocus() const override { return true; }

	/** The menu sets up the appropriate mouse settings upon focus */
	virtual FReply OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent) override;

	void LockControls(bool bEnable);

	/** views a friend's profile in the current user's in-game menu friend list */
	void ControllerFacebuttonLeftPressed();

	/** decrement the index of the friend that the user is currently selecting while in the in-game menu friend list */
	void ControllerUpInputPressed();

	/** increment the index of the friend that the user is currently selecting while in the in-game menu friend list */
	void ControllerDownInputPressed();

	/** Sends a friend invite to a friend in the current user's in-game menu friend list */
	void ControllerFacebuttonDownPressed();

	void BuildAndShowMenu();

	StoredMenu CurrentMenu;
	StoredMenu NextMenu;
	StoredMenu SecondaryMenu;
	TArray<StoredMenu> MenuList;

	StoredTabs TabsList;

	FCustomMenuWidgetStyle MenuWidgetStyle;

	FOnMenuGoBack OnMenuGoBack;

	FOnCloseMenu OnCloseMenu;

protected:

	/** weak pointer to our parent PC */
	TWeakObjectPtr<class ULocalPlayer> PlayerOwner;

	/**Are we using a gamepad*/
	bool bIsUsingGamepad;

	/** if moving around menu is currently locked */
	bool bControlsLocked;

	/** if console is currently opened */
	bool bConsoleVisible;

	/**If the menu can be removed*/
	bool bIsMenuRemovable;

	bool bIsGoingBack;

	/** selected index of current menu */
	int32 SelectedIndex;

	/** Index that was previously selected*/
	int32 PreviousIndex;

	/**Current tab*/
	int32 CurrentTab;

	FText MenuTitleText;

	/**The widget switcher that holds the menus*/
	TSharedPtr<SVerticalBox> MenuHolderBox;

	/**The widget that holds the secondary widgets*/
	TSharedPtr<SHorizontalBox> MenuHolderBoxSecondary;

	/**The horizontal box for the tabs menu*/
	TSharedPtr<SHorizontalBox> TabsMenuHolderBox;

	/**The text for the title of this menu*/
	FText GetMenuTitle() const {return MenuTitleText;};

	void BuildMenu(StoredMenu MenuToBuild);
	void BuildTabs(StoredTabs TabsReference);
	void BuildSecondaryMenu(StoredMenu MenuToBuild);

	/**Panel to set focus to*/
	void SetKeyboardFocus(TSharedPtr<FCustomButton> FirstMenuItem);

	int32 GetNextValidIndex(int32 MoveBy);
	int32 GetNextValidTabIndex(int32 MoveBy);
	void ChangeActiveButton(int32 ButtonIndex);
	void ChangeActiveTab(int32 NewTabIndex);

	FReply ButtonClicked(int32 ButtonIndex);
	FReply BackButtonClicked();

	FReply TabClicked(int32 TabIndex);
	FReply SecondaryMenuClicked(int32 Index);

	void ConfirmMenuItem();
	void ChangeOption(int32 MoveBy, int32 ButtonIndex);

	void UpdateArrows(TSharedPtr<FCustomButton> MenuButton);

	FText GetOptionText(TSharedPtr<FCustomButton> MenuButton) const;

	FMargin GetMenuHolderPadding() const;
	FMargin GetMenuHolderSecondaryPadding() const;
	FMargin GetMenuTitlePadding() const;
	FMargin GetTabsMenuHolderPadding() const;
	FMargin GetMenuBackgroundPadding() const;

	FSlateColor GetMenuBackgroundColor() const;

	/**Called when the input device is changed*/
	void OnInputDeviceChanged(ECustomInputDevice NewInputDevice);

	/** screen resolution */
	FIntPoint ScreenRes;
};

namespace CustomMenuHelper
{
	FORCEINLINE void EnsureValid(TSharedPtr<FCustomButton>& CustomItem)
	{
		if (!CustomItem.IsValid())
		{
			CustomItem = FCustomButton::CreateRoot();
		}
	}

	FORCEINLINE void InitializeTab(TArray<TSharedPtr<class FCustomTab>>& TabsList, TSharedPtr<class FCustomButton>& Menu, const FCustomButtonStyle ButtonStyle, const FText& Text, const FText& HelpText)
	{
		EnsureValid(Menu);
		TSharedPtr<FCustomTab> Item = MakeShareable(new FCustomTab(ButtonStyle, Text, HelpText));
		Item->MenuStored = Menu->SubMenu;
		TabsList.Add(Item);
	}

	FORCEINLINE void InitializeRootTab(TArray<TSharedPtr<class FCustomTab>>& TabsList, TSharedPtr<class FCustomButton>& Menu, const FText& Text)
	{
		EnsureValid(Menu);
		TSharedPtr<FCustomTab> Item = MakeShareable(new FCustomTab(Text));
		Item->MenuStored = Menu->SubMenu;
		TabsList.Add(Item);
	}

	template< class UserClass >
	FORCEINLINE TSharedRef<FCustomButton> InitializeSecondaryMenu(const FCustomButtonStyle ButtonStyle, const FText& Text, const FText& HelpText, UserClass* inObj, typename FCustomButton::FOnCustomButtonClicked::TSPMethodDelegate< UserClass >::FMethodPtr inMethod)
	{
		TSharedPtr<FCustomButton> Item = MakeShareable(new FCustomButton(ButtonStyle, Text, HelpText));
		Item->OnCustomButtonClicked.BindSP(inObj, inMethod);
		return Item.ToSharedRef();
	}

	template< class UserClass >
	FORCEINLINE TSharedRef<FCustomButton> AddStandardMenuButton(TSharedPtr<FCustomButton>& CustomItem, const FCustomButtonStyle ButtonStyle, const FText& Text, const FText& HelpText, UserClass* inObj, typename FCustomButton::FOnCustomButtonClicked::TSPMethodDelegate< UserClass >::FMethodPtr inMethod)
	{
		EnsureValid(CustomItem);
		TSharedPtr<FCustomButton> Item = MakeShareable(new FCustomButton(ButtonStyle, Text, HelpText));
		Item->OnCustomButtonClicked.BindSP(inObj, inMethod);
		CustomItem->SubMenu.Add(Item);
		return Item.ToSharedRef();
	}

	template< class UserClass >
	FORCEINLINE TSharedRef<FCustomButton> AddMultichoiceMenuButton(TSharedPtr<FCustomButton>& CustomItem, const FCustomButtonStyle ButtonStyle, const FText& Text, const TArray<FText> OptionText, const FText& HelpText, UserClass* inObj, typename FCustomButton::FOnCustomButtonOptionChanged::TSPMethodDelegate< UserClass >::FMethodPtr inMethod)
	{
		EnsureValid(CustomItem);
		TSharedPtr<FCustomButton> Item = MakeShareable(new FCustomButton(ButtonStyle, Text, OptionText, HelpText));
		Item->OnCustomButtonOptionChanged.BindSP(inObj, inMethod);
		CustomItem->SubMenu.Add(Item);
		return Item.ToSharedRef();
	}

	FORCEINLINE TSharedRef<FCustomButton> AddMenuButton(TSharedPtr<FCustomButton>& CustomItem, const FCustomButtonStyle ButtonStyle, const FText& Text, const FText& HelpText)
	{
		EnsureValid(CustomItem);
		TSharedPtr<FCustomButton> Item = MakeShareable(new FCustomButton(ButtonStyle, Text, HelpText));
		CustomItem->SubMenu.Add(Item);
		return Item.ToSharedRef();
	}

	FORCEINLINE void ClearSubMenu(TSharedPtr<FCustomButton>& CustomItem)
	{
		EnsureValid(CustomItem);
		CustomItem->SubMenu.Empty();
	}
}