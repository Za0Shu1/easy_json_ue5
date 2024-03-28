// // Copyright 2023, liangbochao. All Rights Reserved.


#include "SEasyJsonImportWidget.h"

#include "ContentBrowserModule.h"
#include "EditorStyleSet.h"
#include "IContentBrowserSingleton.h"
#include "IStructureDetailsView.h"
#include "SlateOptMacros.h"
#include "Common/EasyJsonTypes.h"
#include "CreateAssets/CreateStructProxy.h"
#include "Kismet/KismetTextLibrary.h"
#include "Widgets/Input/SButton.h"
#include "PropertyEditorModule.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
#define LOCTEXT_NAMESPACE "SEasyJsonImport"

void SEasyJsonImportWidget::Construct(const FArguments& InArgs)
{
	// get struct save path
	const FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	IContentBrowserSingleton& ContentBrowserSingleton = ContentBrowserModule.Get();
	DefaultSavePath = ContentBrowserSingleton.GetCurrentPath().GetInternalPathString() ;
	ImportSettings = MakeShareable(new FEasyJsonImportConfig(DefaultSavePath));

	// construct import structure view widget
	CreateImportView();
	ChildSlot
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(FEditorStyle::GetMargin("StandardDialog.ContentPadding"))
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.VAlign(VAlign_Center)
				[
					SettingsView->GetWidget()->AsShared()
				]
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Right)
			.Padding(4, 4, 10, 4)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Right)
				.AutoWidth()
				.Padding(0, 0, 4, 0)
				[
					SNew(SButton)
					.Text(LOCTEXT("Import", "Import"))
					.IsEnabled(this, &SEasyJsonImportWidget::CanDoImport)
					.OnClicked(this, &SEasyJsonImportWidget::DoImport)
					.ToolTipText(this, &SEasyJsonImportWidget::GenerateImportToolTip)
				]
			]
		];
}

void SEasyJsonImportWidget::CreateImportView()
{
	// Create a property view
	FPropertyEditorModule& EditModule = FModuleManager::Get().GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

	FDetailsViewArgs DetailsViewArgs;
	{
		DetailsViewArgs.bAllowSearch = true;
		DetailsViewArgs.bHideSelectionTip = true;
		DetailsViewArgs.bLockable = false;
		DetailsViewArgs.bSearchInitialKeyFocus = true;
		DetailsViewArgs.bUpdatesFromSelection = false;
		DetailsViewArgs.NotifyHook = nullptr;
		DetailsViewArgs.bShowOptions = true;
		DetailsViewArgs.bShowModifiedPropertiesOption = false;
		DetailsViewArgs.bShowScrollBar = false;
		DetailsViewArgs.bShowOptions = true;
	}

	FStructureDetailsViewArgs StructureViewArgs;
	{
		StructureViewArgs.bShowObjects = true;
		StructureViewArgs.bShowAssets = true;
		StructureViewArgs.bShowClasses = true;
		StructureViewArgs.bShowInterfaces = true;
	}
	
	SettingsView = EditModule.CreateStructureDetailView(DetailsViewArgs, StructureViewArgs, nullptr);
	FStructOnScope* Struct = new FStructOnScope(FEasyJsonImportConfig::StaticStruct(), (uint8*)ImportSettings.Get());
	SettingsView->SetStructureData(MakeShareable(Struct));
}

FText SEasyJsonImportWidget::GenerateImportToolTip() const
{
	const FString Result =  CanDoImport() ? "Execute" : "Json File : Invalid Json file.";
	return UKismetTextLibrary::Conv_StringToText(Result);
}

bool SEasyJsonImportWidget::CanDoImport() const
{
	if(!ImportSettings.Get()->JsonFile.FilePath.EndsWith(".json"))
	{
		return false;
	}
	if(ImportSettings.Get()->StructSavePath.Path.IsEmpty())
	{
		ImportSettings.Get()->StructSavePath.Path = DefaultSavePath;
	}
	return true;
}

FReply SEasyJsonImportWidget::DoImport() const
{
	UCreateStructProxy* CreateStructProxy = NewObject<UCreateStructProxy>();
	if(CreateStructProxy->Init(ImportSettings))
	{
		CreateStructProxy->AddToRoot();
		CreateStructProxy->DoImport();
		CreateStructProxy->RemoveFromRoot();
	}
	return FReply::Handled();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
