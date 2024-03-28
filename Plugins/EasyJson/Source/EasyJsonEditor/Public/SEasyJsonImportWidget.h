// // Copyright 2023, liangbochao. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "IStructureDetailsView.h"
#include "Common/EasyJsonTypes.h"

/**
 * 
 */


class EASYJSONEDITOR_API SEasyJsonImportWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SEasyJsonImportWidget)
	{}
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
protected:
	void CreateImportView();
	FText GenerateImportToolTip() const;
	bool CanDoImport() const;
	FReply DoImport() const;
private:
	TSharedPtr<IStructureDetailsView> SettingsView;
	TSharedPtr<FEasyJsonImportConfig> ImportSettings;
	FString DefaultSavePath;
};
