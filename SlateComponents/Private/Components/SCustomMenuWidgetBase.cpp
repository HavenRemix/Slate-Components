// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Components/SCustomMenuWidgetBase.h"

#include "UI/Style/VILEStyle.h"

#include "Engine/Console.h"

#include "VILEGameInstance.h"

void SCustomMenuWidgetBase::Construct(const FArguments& InArgs)
{
	bControlsLocked = false;
	bConsoleVisible = false;
	bIsMenuRemovable = false;
	PlayerOwner = InArgs._PlayerOwner;
	SelectedIndex = 0;
	CurrentTab = 0;
	MenuWidgetStyle = InArgs._MenuWidgetStyle;
	MenuTitleText = InArgs._MenuTitle;

	Cast<UVILEGameInstance>(PlayerOwner->GetGameInstance())->OnInputDeviceChanged.AddRaw(this, &SCustomMenuWidgetBase::OnInputDeviceChanged);

	ChildSlot
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		.HAlign(InArgs._MenuWidgetStyle.BackgroundHAlign)
		.VAlign(InArgs._MenuWidgetStyle.BackgroundVAlign)
		.Padding(TAttribute<FMargin>(this, &SCustomMenuWidgetBase::GetMenuBackgroundPadding))
		[
			SNew(SImage)
			.Image(InArgs._MenuWidgetStyle.BackgroundBrush)
			.ColorAndOpacity(this, &SCustomMenuWidgetBase::GetMenuBackgroundColor)
		]

		+ SOverlay::Slot()
		.HAlign(InArgs._MenuWidgetStyle.HAlign)
		.VAlign(InArgs._MenuWidgetStyle.VAlign)
		.Padding(TAttribute<FMargin>(this, &SCustomMenuWidgetBase::GetMenuTitlePadding))
		[
			SNew(STextBlock)
			.TextStyle(FVILEStyle::Get(), "VILE.MenuTitleTextStyle")
			.Text(this, &SCustomMenuWidgetBase::GetMenuTitle)
		]

		+ SOverlay::Slot()
		.HAlign(InArgs._MenuWidgetStyle.TabsMenuHAlign)
		.VAlign(InArgs._MenuWidgetStyle.TabsMenuVAlign)
		.Padding(TAttribute<FMargin>(this, &SCustomMenuWidgetBase::GetTabsMenuHolderPadding))
		[
			SAssignNew(TabsMenuHolderBox, SHorizontalBox)
		]

		+ SOverlay::Slot()
		.HAlign(InArgs._MenuWidgetStyle.HAlign)
		.VAlign(InArgs._MenuWidgetStyle.VAlign)
		.Padding(TAttribute<FMargin>(this, &SCustomMenuWidgetBase::GetMenuHolderPadding))
		[
			SAssignNew(MenuHolderBox, SVerticalBox)
		]

		+ SOverlay::Slot()
		.HAlign(InArgs._MenuWidgetStyle.SecondaryMenuHAlign)
		.VAlign(InArgs._MenuWidgetStyle.SecondaryMenuVAlign)
		.Padding(TAttribute<FMargin>(this, &SCustomMenuWidgetBase::GetMenuHolderSecondaryPadding))
		[
			SAssignNew(MenuHolderBoxSecondary, SHorizontalBox)
		]
	];
}

void SCustomMenuWidgetBase::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	//Always tick the super
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	//ugly code seeing if the console is open
	UConsole* ViewportConsole = (GEngine != NULL && GEngine->GameViewport != NULL) ? GEngine->GameViewport->ViewportConsole : NULL;
	if (ViewportConsole != NULL && (ViewportConsole->ConsoleState == "Typing" || ViewportConsole->ConsoleState == "Open"))
	{
		if (!bConsoleVisible)
		{
			bConsoleVisible = true;
			FSlateApplication::Get().SetAllUserFocusToGameViewport();
		}
	}
	else
	{
		if (bConsoleVisible)
		{
			bConsoleVisible = false;
			FSlateApplication::Get().SetKeyboardFocus(SharedThis(this));
		}
	}

	if (GEngine && GEngine->GameViewport)
	{
		FVector2D Size = FVector2D::ZeroVector;
		GEngine->GameViewport->GetViewportSize(Size);
		if (!Size.IsZero())
		{
			ScreenRes = (Size / AllottedGeometry.Scale).IntPoint();
		}
	}
}

FReply SCustomMenuWidgetBase::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	return FReply::Handled()
		.SetUserFocus(SharedThis(this), EFocusCause::SetDirectly);
}

FReply SCustomMenuWidgetBase::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
{
	FReply Result = FReply::Unhandled();
	const int32 UserIndex = InKeyEvent.GetUserIndex();
	int32 GetOwnerUserIndex = PlayerOwner.IsValid() ? PlayerOwner->GetControllerId() : 0;
	bool bEventUserCanInteract = GetOwnerUserIndex == -1 || UserIndex == GetOwnerUserIndex;

	if (!bControlsLocked && bEventUserCanInteract)
	{
		const FKey Key = InKeyEvent.GetKey();
				
		//CHANGE OPTIONS
		if (Key == EKeys::Left || Key == EKeys::Gamepad_DPad_Left || Key == EKeys::Gamepad_LeftStick_Left)
		{
			ChangeOption(-1, SelectedIndex);

			Result = FReply::Handled();
		}
		else if (Key == EKeys::Right || Key == EKeys::Gamepad_DPad_Right || Key == EKeys::Gamepad_LeftStick_Right)
		{
			ChangeOption(1, SelectedIndex);

			Result = FReply::Handled();
		}

		//CHANGE INDEX
		else if (Key == EKeys::Up || Key == EKeys::Gamepad_DPad_Up || Key == EKeys::Gamepad_LeftStick_Up)
		{
			ControllerUpInputPressed();
			int32 NextValidIndex = GetNextValidIndex(-1);
			if (SelectedIndex != NextValidIndex)
			{
				ChangeActiveButton(NextValidIndex);
			}

			Result = FReply::Handled();
		}
		else if (Key == EKeys::Down || Key == EKeys::Gamepad_DPad_Down || Key == EKeys::Gamepad_LeftStick_Down)
		{
			ControllerDownInputPressed();
			int32 NextValidIndex = GetNextValidIndex(1);
			if (SelectedIndex != NextValidIndex)
			{
				ChangeActiveButton(NextValidIndex);
			}

			Result = FReply::Handled();
		}

		//CHANGE TAB
		else if (Key == EKeys::Gamepad_LeftShoulder)
		{
			ChangeActiveTab(GetNextValidTabIndex(-1));

			Result = FReply::Handled();
		}
		else if (Key == EKeys::Gamepad_RightShoulder)
		{
			ChangeActiveTab(GetNextValidTabIndex(1));

			Result = FReply::Handled();
		}

		//CONTROLLER FACE BUTTON
		else if (Key == EKeys::Gamepad_FaceButton_Left)
		{
			ControllerFacebuttonLeftPressed();

			if (OnControllerFacebuttonLeftPressed.IsBound())
			{
				OnControllerFacebuttonLeftPressed.Execute();
			}

			Result = FReply::Handled();
		}
		else if (Key == EKeys::Gamepad_FaceButton_Right)
		{
			if (OnControllerFacebuttonRightPressed.IsBound())
			{
				OnControllerFacebuttonRightPressed.Execute();
			}

			Result = FReply::Handled();
		}
		else if (Key == EKeys::Gamepad_FaceButton_Top)
		{
			if (OnControllerFacebuttonUpPressed.IsBound())
			{
				OnControllerFacebuttonUpPressed.Execute();
			}

			Result = FReply::Handled();
		}
		else if (Key == EKeys::Gamepad_FaceButton_Bottom)
		{
			ControllerFacebuttonDownPressed();

			if (OnControllerFacebuttonDownPressed.IsBound())
			{
				OnControllerFacebuttonDownPressed.Execute();
			}

			Result = FReply::Handled();
		}


		if ((Key == EKeys::Escape || Key == EKeys::Virtual_Back || Key == EKeys::Global_Back || Key == EKeys::Global_View) && !InKeyEvent.IsRepeat())
		{
			BackButtonClicked();

			Result = FReply::Handled();
		}
		if (Key == EKeys::Enter || Key == EKeys::Virtual_Accept)
		{
			//ControllerFacebuttonDownPressed();
			ButtonClicked(SelectedIndex);

			Result = FReply::Handled();
		}
	}

	return Result;
}

FReply SCustomMenuWidgetBase::OnFocusReceived(const FGeometry& MyGeometry, const FFocusEvent& InFocusEvent)
{
	return FReply::Handled().SetUserFocus(this->AsShared(), EFocusCause::SetDirectly);
}

void SCustomMenuWidgetBase::BuildAndShowMenu()
{
	if (TabsList.Num() > 1)
	{
		BuildTabs(TabsList);
	}

	BuildMenu(TabsList[CurrentTab]->MenuStored);
	BuildSecondaryMenu(SecondaryMenu);
}

void SCustomMenuWidgetBase::LockControls(bool bEnable)
{
	bControlsLocked = bEnable;
}

void SCustomMenuWidgetBase::ControllerFacebuttonLeftPressed()
{
	if (CurrentMenu[SelectedIndex]->OnControllerFacebuttonLeftPressed.IsBound())
	{
		CurrentMenu[SelectedIndex]->OnControllerFacebuttonLeftPressed.Execute();
	}
}

void SCustomMenuWidgetBase::ControllerUpInputPressed()
{
	if (CurrentMenu[SelectedIndex]->OnControllerUpInputPressed.IsBound())
	{
		CurrentMenu[SelectedIndex]->OnControllerUpInputPressed.Execute();
	}
}

void SCustomMenuWidgetBase::ControllerDownInputPressed()
{
	if (CurrentMenu[SelectedIndex]->OnControllerDownInputPressed.IsBound())
	{
		CurrentMenu[SelectedIndex]->OnControllerDownInputPressed.Execute();
	}
}

void SCustomMenuWidgetBase::ControllerFacebuttonDownPressed()
{
	if (CurrentMenu[SelectedIndex]->OnControllerFacebuttonDownPressed.IsBound())
	{
		CurrentMenu[SelectedIndex]->OnControllerFacebuttonDownPressed.Execute();
	}
}


void SCustomMenuWidgetBase::BuildMenu(StoredMenu MenuToBuild)
{
	if (MenuToBuild.Num() == 0)
	{
		return;
	}

	MenuHolderBox->ClearChildren();

	if (!bIsGoingBack)
	{
		MenuList.Add(MenuToBuild);
	}
	else
	{
		bIsGoingBack = false;
	}

	CurrentMenu = MenuToBuild;

	int32 MyPreviousIndex = -1;
	SelectedIndex = MyPreviousIndex;

	if (TabsList[CurrentTab])
	{
		MenuTitleText = TabsList[CurrentTab]->GetText();
	}

	for (int32 i = 0; i < MenuToBuild.Num(); ++i)
	{
		TSharedPtr<SWidget> TempWidget;

		TempWidget = SAssignNew(MenuToBuild[i]->Widget, SCustomButton)
		.PlayerOwner(PlayerOwner)
		.OnClicked(this, &SCustomMenuWidgetBase::ButtonClicked, i)
		.ButtonStyle(MenuToBuild[i]->GetButtonStyle())
		.Text(MenuToBuild[i]->GetText())
		.HelpText(MenuToBuild[i]->GetHelpText())
		.ButtonIndex(i)
		.bIsMultichoice(MenuToBuild[i]->ButtonType == ECustomButtonType::Multichoice)
		.OnArrowPressed(this, &SCustomMenuWidgetBase::ChangeOption)
		.OptionText(this, &SCustomMenuWidgetBase::GetOptionText, MenuToBuild[i]);

		if (MenuToBuild[i]->ButtonType == ECustomButtonType::Multichoice)
		{
			UpdateArrows(MenuToBuild[i]);
		}

		if (TempWidget.IsValid())
		{
			if (SelectedIndex == -1)
			{
				SelectedIndex = i;
			}

			MenuHolderBox->AddSlot().HAlign(HAlign_Left).AutoHeight()
			[
				TempWidget.ToSharedRef()
			];

			MenuHolderBox->AddSlot().HAlign(HAlign_Left).AutoHeight()
			[
				SNew(SSpacer)
				.Size(FVector2D(0.0f, 10.0f))
			];
		}
	}

	if (bIsUsingGamepad)
	{
		CurrentMenu[SelectedIndex]->Widget->SetHoveredMenuButton(true);
	}
}

void SCustomMenuWidgetBase::BuildTabs(StoredTabs TabsReference)
{
	if (TabsReference.Num() == 0)
	{
		return;
	}

	TabsMenuHolderBox->ClearChildren();

	CurrentTab = -1;

	for (int32 i = 0; i < TabsReference.Num(); ++i)
	{
		TSharedPtr<SWidget> TempWidget;

		TempWidget = SAssignNew(TabsReference[i]->Widget, SCustomButton)
		.PlayerOwner(PlayerOwner)
		.OnClicked(this, &SCustomMenuWidgetBase::TabClicked, i)
		.ButtonStyle(TabsReference[i]->GetButtonStyle())
		.Text(TabsReference[i]->GetText())
		.HelpText(TabsReference[i]->GetHelpText())
		.ButtonIndex(i)
		.bIsMultichoice(false)
		.bIsTabButton(true);

		if (TempWidget.IsValid())
		{
			if (CurrentTab == -1)
			{
				CurrentTab = i;
			}

			TabsMenuHolderBox->AddSlot().HAlign(HAlign_Left)
			[
				TempWidget.ToSharedRef()
			];
		}
	}

	TabsList[CurrentTab]->Widget->SetSelectedMenuButton(true);
}

void SCustomMenuWidgetBase::BuildSecondaryMenu(StoredMenu MenuToBuild)
{
	if (MenuToBuild.Num() == 0)
	{
		return;
	}

	MenuHolderBoxSecondary->ClearChildren();

	SelectedIndex = -1;

	if (TabsList[CurrentTab])
	{
		MenuTitleText = TabsList[CurrentTab]->GetText();
	}

	for (int32 i = 0; i < MenuToBuild.Num(); ++i)
	{
		TSharedPtr<SWidget> TempWidget;

		TempWidget = SAssignNew(MenuToBuild[i]->Widget, SCustomButton)
		.PlayerOwner(PlayerOwner)
		.OnClicked(this, &SCustomMenuWidgetBase::SecondaryMenuClicked, i)
		.ButtonStyle(MenuToBuild[i]->GetButtonStyle())
		.Text(MenuToBuild[i]->GetText())
		.HelpText(MenuToBuild[i]->GetHelpText())
		.ButtonIndex(i);
		//.bIsMultichoice(MenuToBuild[i]->ButtonType == ECustomButtonType::Multichoice)
		//.OnArrowPressed(this, &SCustomMenuWidgetBase::ChangeOption)
		//.OptionText(this, &SCustomMenuWidgetBase::GetOptionText, MenuToBuild[i]);

		if (TempWidget.IsValid())
		{
			if (SelectedIndex == -1)
			{
				SelectedIndex = i;
			}

			MenuHolderBoxSecondary->AddSlot().HAlign(MenuWidgetStyle.SecondaryMenuHAlign)
			[
				TempWidget.ToSharedRef()
			];
		}
	}
}


void SCustomMenuWidgetBase::SetKeyboardFocus(TSharedPtr<FCustomButton> FirstMenuItem)
{
	if (FirstMenuItem.IsValid())
	{
		FirstMenuItem->Widget->SetHoveredMenuButton(true);
		FSlateApplication::Get().SetKeyboardFocus(SharedThis(this));
	}
}

int32 SCustomMenuWidgetBase::GetNextValidIndex(int32 MoveBy)
{
	int32 Result = SelectedIndex;

	if (MoveBy != 0 && SelectedIndex + MoveBy > -1 && SelectedIndex + MoveBy < CurrentMenu.Num())
	{
		Result = SelectedIndex + MoveBy;

		while (!CurrentMenu[Result]->Widget.IsValid())
		{
			MoveBy > 0 ? Result++ : Result--;

			if (CurrentMenu.IsValidIndex(Result))
			{
				Result = SelectedIndex;
				break;
			}
		}
	}

	return Result;
}

int32 SCustomMenuWidgetBase::GetNextValidTabIndex(int32 MoveBy)
{
	int32 Result = CurrentTab;

	if (MoveBy != 0)
	{
		Result = CurrentTab + MoveBy;

		if (Result <= -1)
		{
			Result = TabsList.Find(TabsList.Last());
		}
		else if (Result >= TabsList.Num())
		{
			Result = 0;
		}
	}

	return Result;
}

FReply SCustomMenuWidgetBase::ButtonClicked(int32 ButtonIndex)
{
	if (bControlsLocked)
	{
		return FReply::Handled();
	}

	if (SelectedIndex != ButtonIndex)
	{
		SelectedIndex = ButtonIndex;
		TSharedPtr<SCustomButton> MenuButton = CurrentMenu[SelectedIndex]->Widget;
		NextMenu = CurrentMenu[SelectedIndex]->SubMenu;
		ConfirmMenuItem();
	}
	else if (SelectedIndex == ButtonIndex)
	{
		NextMenu = CurrentMenu[SelectedIndex]->SubMenu;
		ConfirmMenuItem();
	}

	return FReply::Handled().SetUserFocus(SharedThis(this), EFocusCause::SetDirectly);
}

FReply SCustomMenuWidgetBase::TabClicked(int32 TabIndex)
{
	if (bControlsLocked)
	{
		return FReply::Handled();
	}

	if (CurrentTab != TabIndex)
	{
		ChangeActiveTab(TabIndex);
	}

	return FReply::Handled().SetUserFocus(SharedThis(this), EFocusCause::SetDirectly);
}

FReply SCustomMenuWidgetBase::SecondaryMenuClicked(int32 Index)
{
	if (bControlsLocked)
	{
		return FReply::Handled();
	}

	if (SecondaryMenu[Index]->OnCustomButtonClicked.IsBound())
	{
		SecondaryMenu[Index]->OnCustomButtonClicked.Execute();
	}

	return FReply::Handled().SetUserFocus(SharedThis(this), EFocusCause::SetDirectly);
}

void SCustomMenuWidgetBase::ChangeActiveButton(int32 ButtonIndex)
{
	if (bControlsLocked)
	{
		return;
	}

	if (SelectedIndex != ButtonIndex)
	{
		CurrentMenu[SelectedIndex]->Widget->SetHoveredMenuButton(false);
		SelectedIndex = ButtonIndex;
		CurrentMenu[SelectedIndex]->Widget->SetHoveredMenuButton(true);
		NextMenu = CurrentMenu[SelectedIndex]->SubMenu;
	}

	return;
}

void SCustomMenuWidgetBase::ChangeActiveTab(int32 NewTabIndex)
{
	if (bControlsLocked)
	{
		return;
	}

	if (CurrentTab != NewTabIndex)
	{
		TabsList[CurrentTab]->Widget->SetSelectedMenuButton(false);
		CurrentTab = NewTabIndex;
		TabsList[CurrentTab]->Widget->SetSelectedMenuButton(true);

		BuildMenu(TabsList[CurrentTab]->MenuStored);
	}
}

FReply SCustomMenuWidgetBase::BackButtonClicked()
{
	if (MenuList.Num() == 1)
	{
		OnCloseMenu.ExecuteIfBound();
	}

	if (MenuList.Num() > 1)
	{
		bIsGoingBack = true;

		OnMenuGoBack.ExecuteIfBound();

		MenuList.RemoveAt(MenuList.Num() - 1);
		BuildMenu(MenuList.Last());
	}

	return FReply::Handled();
}


void SCustomMenuWidgetBase::ConfirmMenuItem()
{
	if (CurrentMenu[SelectedIndex]->OnCustomButtonClicked.IsBound())
	{
		CurrentMenu[SelectedIndex]->OnCustomButtonClicked.Execute();
	}
	if (CurrentMenu[SelectedIndex]->SubMenu.Num() > 0)
	{
		BuildMenu(NextMenu);
	}
}

void SCustomMenuWidgetBase::ChangeOption(int32 MoveBy, int32 ButtonIndex)
{
	if (SelectedIndex != ButtonIndex)
	{
		ChangeActiveButton(ButtonIndex);
	}

	TSharedPtr<FCustomButton> MenuButton = CurrentMenu[SelectedIndex];

	const int32 MinIndex = MenuButton->MinMultiChoiceIndex > -1 ? MenuButton->MinMultiChoiceIndex : 0;
	const int32 MaxIndex = MenuButton->MaxMultiChoiceIndex > -1 ? MenuButton->MaxMultiChoiceIndex : MenuButton->OptionText.Num() - 1;
	const int32 CurrentIndex = MenuButton->SelectedMultiChoice;

	if (MenuButton->ButtonType == ECustomButtonType::Multichoice)
	{
		if (CurrentIndex + MoveBy >= MinIndex && CurrentIndex + MoveBy <= MaxIndex)
		{
			MenuButton->SelectedMultiChoice += MoveBy;
			MenuButton->OnCustomButtonOptionChanged.ExecuteIfBound(MenuButton, MenuButton->SelectedMultiChoice);
		}
		UpdateArrows(MenuButton);
	}
}

void SCustomMenuWidgetBase::UpdateArrows(TSharedPtr<FCustomButton> MenuButton)
{
	const int32 MinIndex = MenuButton->MinMultiChoiceIndex > -1 ? MenuButton->MinMultiChoiceIndex : 0;
	const int32 MaxIndex = MenuButton->MaxMultiChoiceIndex > -1 ? MenuButton->MaxMultiChoiceIndex : MenuButton->OptionText.Num() - 1;
	const int32 CurrentIndex = MenuButton->SelectedMultiChoice;
	if (CurrentIndex > MinIndex)
	{
		//MenuButton->Widget->LeftArrowVisible = EVisibility::Visible;
		MenuButton->Widget->bLeftArrowAvaliable = true;
	}
	else {
		//MenuButton->Widget->LeftArrowVisible = EVisibility::Hidden;
		MenuButton->Widget->bLeftArrowAvaliable = false;
	}
	if (CurrentIndex < MaxIndex)
	{
		//MenuButton->Widget->RightArrowVisible = EVisibility::Visible;
		MenuButton->Widget->bRightArrowAvaliable = true;
	}
	else {
		//MenuButton->Widget->RightArrowVisible = EVisibility::Hidden;
		MenuButton->Widget->bRightArrowAvaliable = false;
	}
}

FText SCustomMenuWidgetBase::GetOptionText(TSharedPtr<FCustomButton> MenuButton) const
{
	FText Result = FText::GetEmpty();

	if (MenuButton->SelectedMultiChoice > -1 && MenuButton->SelectedMultiChoice < MenuButton->OptionText.Num())
	{
		Result = MenuButton->OptionText[MenuButton->SelectedMultiChoice];
	}

	return Result;
}


FMargin SCustomMenuWidgetBase::GetMenuHolderPadding() const
{
	return MenuWidgetStyle.MenuHolderPadding;
}

FMargin SCustomMenuWidgetBase::GetMenuHolderSecondaryPadding() const
{
	return MenuWidgetStyle.SecondaryMenuPadding;
}

FMargin SCustomMenuWidgetBase::GetMenuTitlePadding() const
{
	return MenuWidgetStyle.MenuTitlePadding;
}

FMargin SCustomMenuWidgetBase::GetTabsMenuHolderPadding() const
{
	return MenuWidgetStyle.TabMenuPadding;
}

FMargin SCustomMenuWidgetBase::GetMenuBackgroundPadding() const
{
	return MenuWidgetStyle.BackgroundPadding;
}


FSlateColor SCustomMenuWidgetBase::GetMenuBackgroundColor() const
{
	return MenuWidgetStyle.BackgroundColor;
}


void SCustomMenuWidgetBase::OnInputDeviceChanged(ECustomInputDevice NewInputDevice)
{
	if (NewInputDevice == ECustomInputDevice::XBoxController || NewInputDevice == ECustomInputDevice::PS4Controller)
	{
		bIsUsingGamepad = true;

		ChangeActiveButton(SelectedIndex);
	}
	else if (NewInputDevice == ECustomInputDevice::KeyboardAndMouse && CurrentMenu[SelectedIndex]->Widget)
	{
		bIsUsingGamepad = false;

		CurrentMenu[SelectedIndex]->Widget->SetHoveredMenuButton(false);
	}
}