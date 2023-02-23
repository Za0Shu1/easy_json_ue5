// Copyright 2023, liangbochao. All Rights Reserved.

#include "EasyJsonEditor.h"

#include "EasyJsonCommand.h"
#include "LevelEditor.h"
#include "CreateAssets/CreateStructProxy.h"

#define LOCTEXT_NAMESPACE "FEasyJsonEditorModule"

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
	UCreateStructProxy* CreateStructProxy = NewObject<UCreateStructProxy>();
	CreateStructProxy->AddToRoot();
	CreateStructProxy->Init();
	CreateStructProxy->DoImport();
	CreateStructProxy->RemoveFromRoot();
}

void FEasyJsonEditorModule::CreateCustomStruct()
{
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FEasyJsonEditorModule, EasyJsonEditor)