// Copyright 2023, liangbochao. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FEasyJsonEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void AddToolbarExtension(FToolBarBuilder& Builder);
	void OnImportButtonClick();


	void CreateCustomStruct();
private:
	TSharedPtr<class FUICommandList> PluginCommands;
};
