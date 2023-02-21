// Copyright 2023, liangbochao. All Rights Reserved.


#include "EasyJsonCommand.h"
#define LOCTEXT_NAMESPACE "FEasyJsonRuntimeModule"

void FEasyJsonCommand::RegisterCommands()
{
	UI_COMMAND(Import, "Import", "Import struct from json string.", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
