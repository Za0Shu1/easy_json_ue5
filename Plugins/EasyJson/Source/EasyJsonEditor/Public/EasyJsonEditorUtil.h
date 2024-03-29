// Copyright © 2023 liangbochao, All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Common/EasyJsonTypes.h"

#include "EasyJsonEditorUtil.generated.h"

/**
 * 
 */
DECLARE_LOG_CATEGORY_EXTERN(LogEasyJsonEditorUtils, Log, All);

class UUserDefinedStruct;

UCLASS()
class EASYJSONEDITOR_API UEasyJsonEditorUtil : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "EasyJson | Common")
	static class UObject* GetObjectFromPath(FName SoftPath);

	static bool GenerateStructPinType(FEdGraphPinType& OutPinType, EStructPinType PinType, FName SubStructPath = FName(), EPinContainerType ContainerType = EPinContainerType::None, FEdGraphTerminalType InPinValueType = FEdGraphTerminalType());
	static bool GenerateValuePinType(FEdGraphTerminalType& OutPinType, EStructPinType PinType, FName SubStructPath = FName());

private:
	static bool HasGetTypeHash(const FName& PinType, UObject* ScriptStruct = nullptr);

};
