// Copyright 2023, liangbochao. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EasyJsonUtils.generated.h"

/**
 * 
 */
DECLARE_LOG_CATEGORY_EXTERN(LogEasyJsonUtils, Log, All);

UCLASS()
class EASYJSON_API UEasyJsonUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:	

	static bool GenericEasyDeserialize(const FString& InJsonString,FStructProperty* OutStruct,void* StructPtr);
	static bool GenericEasySerialize(const FStructProperty* InStruct,FString& OutJsonString,void* StructPtr);

	// this OutStruct is an Generics output param
	UFUNCTION(BlueprintCallable,CustomThunk,Category= "EasyJson | Common", meta=(CustomStructureParam = "OutStruct",BlueprintInternalUseOnly="false"))
	static bool EasyDeserialize(const FString& InJsonString,int32& OutStruct);

	// this InStruct is an Generics input param
	UFUNCTION(BlueprintCallable,CustomThunk,Category= "EasyJson | Common", meta=(CustomStructureParam = "InStruct",BlueprintInternalUseOnly="false"))
	static bool EasySerialize(const int32& InStruct,FString& OutJsonString);

	static TArray<FString> SaveFileDialog(const FString& DialogTitle, const FString& DefaultPath, const FString& DefaultFile, const FString& FileTypes, uint32 Flags);
	static TArray<FString> OpenFileDialog(const FString& DialogTitle, const FString& DefaultPath, const FString& DefaultFile, const FString& FileTypes, uint32 Flags);

private:
	DECLARE_FUNCTION(execEasyDeserialize);
	DECLARE_FUNCTION(execEasySerialize);
};
