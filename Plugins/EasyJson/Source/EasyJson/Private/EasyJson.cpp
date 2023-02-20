// Copyright 2023, liangbochao. All Rights Reserved.

#include "EasyJson.h"

#define LOCTEXT_NAMESPACE "FEasyJsonModule"

void FEasyJsonModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FEasyJsonModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FEasyJsonModule, EasyJson)