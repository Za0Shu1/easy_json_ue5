// Copyright 2023, liangbochao. All Rights Reserved.


#include "Common/EasyJsonUtils.h"

#include "DesktopPlatformModule.h"
#include "IDesktopPlatform.h"
#include "JsonObjectConverter.h"
#include "UObject/UnrealTypePrivate.h"

DEFINE_LOG_CATEGORY(LogEasyJsonUtils);



bool UEasyJsonUtils::GenericEasyDeserialize(const FString& InJsonString, FStructProperty* OutStruct, void* StructPtr)
{
	// pin check
	if(!(OutStruct && StructPtr))
	{
		UE_LOG(LogEasyJsonUtils, Error, TEXT("Output Pin is not a struct."));
		return false;
	}

	// json string deserialize
	const TSharedRef< TJsonReader<> >& Reader = TJsonReaderFactory<>::Create(InJsonString);
	TSharedPtr<FJsonObject> Object;
	if (!(FJsonSerializer::Deserialize(Reader, /*out*/ Object) && Object.IsValid()))
	{
		UE_LOG(LogEasyJsonUtils, Error, TEXT("Input string can not parse to json object,with error code : %s"),*Reader->GetErrorMessage());
		return false;
	}

	// convert json object to struct
	return FJsonObjectConverter::JsonObjectToUStruct(Object.ToSharedRef(),OutStruct->Struct,StructPtr);
}

bool UEasyJsonUtils::GenericEasySerialize(const FStructProperty* InStruct, FString& OutJsonString, void* StructPtr)
{
	// pin check 
	if(!(InStruct && StructPtr))
	{
		UE_LOG(LogEasyJsonUtils, Error, TEXT("Input Pin is not a struct."));
		return false;
	}

	// convert struct to json string
	//@ TODO: Using this method will cause a problem that those variable name first char will force changed to lowercase,you can redefine this method specific your own variable name.
	return FJsonObjectConverter::UStructToJsonObjectString(InStruct->Struct,StructPtr,OutJsonString);
}

bool UEasyJsonUtils::EasyDeserialize(const FString& InJsonString,int32& OutStruct)
{
	// We should never hit this!  stubs to avoid NoExport on the class.
	check(0);
	return false;
}

bool UEasyJsonUtils::EasySerialize(const int32& InStruct,FString& OutJsonString)
{
	// We should never hit this!  stubs to avoid NoExport on the class.
	check(0);
	return false;
}

TArray<FString> UEasyJsonUtils::SaveFileDialog(const FString& DialogTitle, const FString& DefaultPath,
	const FString& DefaultFile, const FString& FileTypes, uint32 Flags)
{
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();

	TArray<FString> SaveFilenames;
	if (DesktopPlatform)
	{
		const bool bOpened = DesktopPlatform->SaveFileDialog(
			nullptr,
			DialogTitle,
			DefaultPath,
			DefaultFile,
			FileTypes,
			Flags,
			SaveFilenames
		);
	}
	return SaveFilenames;
}

TArray<FString> UEasyJsonUtils::OpenFileDialog(const FString& DialogTitle, const FString& DefaultPath,
	const FString& DefaultFile, const FString& FileTypes, uint32 Flags)
{
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	TArray<FString> SelectedFiles;
	
	if (DesktopPlatform)
	{
		const bool bOpened = DesktopPlatform->OpenFileDialog(
			nullptr,
			DialogTitle,
			DefaultPath,
			DefaultFile,
			FileTypes,
			Flags,
			SelectedFiles
		);
	}
	return SelectedFiles;
}


DEFINE_FUNCTION(UEasyJsonUtils::execEasyDeserialize)
{
	// Get input variable
	P_GET_PROPERTY_REF(FStrProperty,InJsonString);
	// Get output variable
	Stack.StepCompiledIn<FStructProperty>(NULL);
	void* StructPtr = Stack.MostRecentPropertyAddress;
	FStructProperty* OutStruct = CastField<FStructProperty>(Stack.MostRecentProperty);
	P_FINISH;
	
	bool bSuccess = false;
	P_NATIVE_BEGIN;
	bSuccess = GenericEasyDeserialize(InJsonString,OutStruct,StructPtr);
	P_NATIVE_END;
	*(bool*)RESULT_PARAM = bSuccess;
}

DEFINE_FUNCTION(UEasyJsonUtils::execEasySerialize)
{
	// Get input variable
	Stack.StepCompiledIn<FStructProperty>(NULL);
	void* StructPtr = Stack.MostRecentPropertyAddress;
	FStructProperty* InStruct = CastField<FStructProperty>(Stack.MostRecentProperty);
	// Get output variable
	P_GET_PROPERTY_REF(FStrProperty,OutJsonString);
	P_FINISH;

	bool bSuccess = false;
	P_NATIVE_BEGIN;
	bSuccess = GenericEasySerialize(InStruct,OutJsonString,StructPtr);
	P_NATIVE_END;
	*(bool*)RESULT_PARAM = bSuccess;
}






