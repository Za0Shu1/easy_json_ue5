// Copyright 2023, liangbochao. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IPluginManager.h"
#include "EdGraph/EdGraphPin.h"
#include "EasyJsonTypes.generated.h"

/**
 * 
 */
UENUM()
enum class EStructPinType : uint8
{
	PC_Boolean		UMETA(DisplayName="Boolean"),
	PC_Byte			UMETA(DisplayName="Byte"),
	PC_Int			UMETA(DisplayName="Int"),
	PC_Int64		UMETA(DisplayName="Int64"),
	PC_Float		UMETA(DisplayName="Float"),
	PC_Double		UMETA(DisplayName="Double"),
	PC_Name			UMETA(DisplayName="Name"),
	PC_String		UMETA(DisplayName="String"),
	PC_Text			UMETA(DisplayName="Text"),
	PC_Vector		UMETA(DisplayName="Vector"),
	PC_Rotator		UMETA(DisplayName="Rotator"),
	PC_Transform	UMETA(DisplayName="Transform"),
	PC_Struct		UMETA(DisplayName="Struct"),
	PC_Enum			UMETA(DisplayName="Enum")
};

USTRUCT()
struct FStructDescription
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
		FName AssetName;

	UPROPERTY()
		FString PackageName;

	UPROPERTY()
		TMap<FString,FEdGraphPinType> VariableInfo;
	
	UPROPERTY()
		bool bAlreadyCreated;

	FStructDescription() :
		AssetName(FName("")),
		PackageName(""),
		VariableInfo({}),
		bAlreadyCreated(false)
	{
	}
};

UENUM(BlueprintType)
enum class EAutoGenerateFolderType : uint8
{
	E_GUID			UMETA(DisplayName="GUID"),
	E_Timestamp		UMETA(DisplayName="Timestamp"),
	E_UserDefine	UMETA(DisplayName="UserDefine")
};


USTRUCT(BlueprintType)
struct FEasyJsonImportConfig
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Import From",meta = (RelativeToGameContentDir))
	FFilePath JsonFile;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Save To" ,meta = (RelativeToGameContentDir, LongPackageName))
	FDirectoryPath StructSavePath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Save To")
	EAutoGenerateFolderType AutoGenerateSubFolderName = EAutoGenerateFolderType::E_Timestamp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Save To" ,meta=(EditCondition="AutoGenerateSubFolderName==EAutoGenerateFolderType::E_UserDefine"))
	FString CustomSubFolder = "";

	FEasyJsonImportConfig()
	{
		
	}
	
	FEasyJsonImportConfig(FString DefaultSavePath)
	{
		JsonFile.FilePath = IPluginManager::Get().FindPlugin("EasyJson")->GetBaseDir() / TEXT("Json/Simple.json");
		StructSavePath.Path = DefaultSavePath;
	}
};

UCLASS()
class EASYJSON_API UEasyJsonTypes : public UObject
{
public:
	GENERATED_BODY()
};
