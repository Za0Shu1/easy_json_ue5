// Copyright 2023, liangbochao. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EasyJsonTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EasyJsonEditorUtils.generated.h"

/**
 * 
 */
class UUserDefinedStruct;
DECLARE_LOG_CATEGORY_EXTERN(LogEasyJsonEditorUtils, Log, All);

UCLASS()
class EASYJSONEDITOR_API UEasyJsonEditorUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable,Category="EasyJson | Common")
	static class UObject* GetObjectFromPath(FName SoftPath);
	
	static bool GenerateStructPinType(FEdGraphPinType& OutPinType, EStructPinType PinType,FName SubStructPath = FName(),EPinContainerType ContainerType = EPinContainerType::None,FEdGraphTerminalType InPinValueType = FEdGraphTerminalType());
	static bool GenerateValuePinType(FEdGraphTerminalType& OutPinType, EStructPinType PinType,FName SubStructPath = FName());

	static TArray<FString> SaveFileDialog(const FString& DialogTitle, const FString& DefaultPath, const FString& DefaultFile, const FString& FileTypes, uint32 Flags);
	static TArray<FString> OpenFileDialog(const FString& DialogTitle, const FString& DefaultPath, const FString& DefaultFile, const FString& FileTypes, uint32 Flags);
	
private:
	static bool HasGetTypeHash(const FName& PinType,UObject* ScriptStruct = nullptr);
};
