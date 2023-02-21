// // Copyright 2023, liangbochao. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CreateStructProxy.generated.h"

/**
 * 
 */

DECLARE_LOG_CATEGORY_EXTERN(LogEasyJsonCreateAssets, Log, All);

UCLASS()
class EASYJSONEDITOR_API UCreateStructProxy : public UObject
{
	GENERATED_BODY()

public:
	void Init(FString JsonFilePath = "");
	void DoImport();

private:
	FString OriginJsonString = "";
};
