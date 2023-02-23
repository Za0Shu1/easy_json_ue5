// Copyright 2023, liangbochao. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "EasyJsonStyle.h"

/**
 * 
 */
class EASYJSONEDITOR_API FEasyJsonCommand : public TCommands<FEasyJsonCommand>
{
public:
 FEasyJsonCommand()
  // bug (UE-170850), this toolbar label will not appear under the toolbar icon
  : TCommands<FEasyJsonCommand>(TEXT("EasyJson"), NSLOCTEXT("Contexts", "JsonConverter", "Convert between json string and ustruct."), NAME_None, FEasyJsonStyle::GetStyleSetName())
 {
 } 
 virtual void RegisterCommands() override;

public:
 TSharedPtr< FUICommandInfo > Import;
};
