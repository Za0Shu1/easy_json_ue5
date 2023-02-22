// Copyright 2023, liangbochao. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
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

UCLASS()
class EASYJSON_API UEasyJsonTypes : public UObject
{
public:
	GENERATED_BODY()
};
