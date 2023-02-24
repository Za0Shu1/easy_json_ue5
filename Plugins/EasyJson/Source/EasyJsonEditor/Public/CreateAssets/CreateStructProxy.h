// // Copyright 2023, liangbochao. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "Common/EasyJsonTypes.h"
#include "Serialization/JsonTypes.h"
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
	virtual bool Init(TSharedPtr<FEasyJsonImportConfig> ImportSettings);
	virtual void DoImport();
	bool CreateStructFromJsonObject(FString StructName, const TSharedPtr<FJsonObject>& InObject, FSoftObjectPath& OutStructSoftPath);
	bool GeneratePinFromJsonValueType(TTuple<FString,TSharedPtr<FJsonValue>> InAttribute,EStructPinType& OutPinType,EPinContainerType& OutContainerType,FName& OutSubStructPath);
	bool GenerateStructFromDescription(FStructDescription& InDescription, FSoftObjectPath& CreatedStructSoftPath);

private:
	FString OriginJsonString = "";
	FString JsonFileName = "Default";
	
	static FString GetJsonTypeNameString(EJson JsonType);
	bool DeleteAssetByPackageName(FString PackageName) const;
	FString SavePath = "";
};
