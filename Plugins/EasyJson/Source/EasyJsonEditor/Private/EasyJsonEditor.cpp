// Copyright 2023, liangbochao. All Rights Reserved.

#include "EasyJsonEditor.h"

#include "EasyJsonCommand.h"
#include "LevelEditor.h"
#include "SEasyJsonImportWidget.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Widgets/Docking/SDockTab.h"
#include "CreateAssets/CreateStructProxy.h"

#define LOCTEXT_NAMESPACE "FEasyJsonEditorModule"

static const FName EasyJsonTableName("EasyJson");

void FEasyJsonEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	FEasyJsonStyle::Initialize();
	FEasyJsonStyle::ReloadTextures();
	
	FEasyJsonCommand::Register();

	PluginCommands = MakeShareable(new FUICommandList);
	PluginCommands->MapAction(
		FEasyJsonCommand::Get().Import,
		FExecuteAction::CreateRaw(this, &FEasyJsonEditorModule::OnImportButtonClick),
		FCanExecuteAction());

	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

	// settings
	#if ENGINE_MAJOR_VERSION > 4
	FName ExtensionHook = "Play";
#else
	FName ExtensionHook = "Settings";
#endif
	TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
	ToolbarExtender->AddToolBarExtension(ExtensionHook, EExtensionHook::After, PluginCommands, FToolBarExtensionDelegate::CreateRaw(this, &FEasyJsonEditorModule::AddToolbarExtension));

	LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
}

void FEasyJsonEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FEasyJsonCommand::Unregister();
	FEasyJsonStyle::Shutdown();
}

void FEasyJsonEditorModule::AddToolbarExtension(FToolBarBuilder& Builder)
{
	Builder.AddToolBarButton(FEasyJsonCommand::Get().Import);
}

void FEasyJsonEditorModule::OnImportButtonClick()
{
	if(!DockTab.IsValid())
	{
		FGlobalTabmanager::Get()->RegisterNomadTabSpawner(EasyJsonTableName, FOnSpawnTab::CreateRaw(this, &FEasyJsonEditorModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FEasyJsonTabTitle", "EasyJson"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
	}
	FGlobalTabmanager::Get()->TryInvokeTab(EasyJsonTableName);
}

void FEasyJsonEditorModule::OnTabClosed(TSharedRef<SDockTab> InTab)
{
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(EasyJsonTableName);
	DockTab.Reset();
}

TSharedRef<class SDockTab> FEasyJsonEditorModule::OnSpawnPluginTab(const class FSpawnTabArgs& InSpawnTabArgs)
{
	return SAssignNew(DockTab,SDockTab)
		.TabRole(ETabRole::NomadTab)
		.Label(LOCTEXT("EasyJsonTab", "Easy Json"))
		.ToolTipText(LOCTEXT("EasyJsonTabTextToolTip", "Easy Json"))
		.OnTabClosed(SDockTab::FOnTabClosedCallback::CreateRaw(this,&FEasyJsonEditorModule::OnTabClosed))
		.Clipping(EWidgetClipping::ClipToBounds)
		[
			SNew(SEasyJsonImportWidget)
		];
	;
}
#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FEasyJsonEditorModule, EasyJsonEditor)