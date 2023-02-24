// Copyright 2023, liangbochao. All Rights Reserved.


#include "Common/EasyJsonUtils.h"

#include "DesktopPlatformModule.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/UserDefinedStruct.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "EdGraphSchema_K2.h"
#include "IDesktopPlatform.h"
#include "JsonObjectConverter.h"
#include "UObject/UnrealTypePrivate.h"

DEFINE_LOG_CATEGORY(LogEasyJsonUtils);

bool UEasyJsonUtils::GenerateStructPinType( FEdGraphPinType& OutPinType, EStructPinType PinType, FName SubStructPath,
	EPinContainerType ContainerType,FEdGraphTerminalType InPinValueType)
{
	FName PinCategory = NAME_None;
	FName PinSubCategory = NAME_None;
	UObject* PinSubCategoryObject = nullptr;
	FEdGraphTerminalType PinValueType = FEdGraphTerminalType();
	switch (PinType)
	{
	case EStructPinType::PC_Boolean:
		PinCategory = FName("bool");
		break;
	case EStructPinType::PC_Byte:
		PinCategory = FName("byte");
		break;
	case EStructPinType::PC_Int:
		PinCategory = FName("int");
		break;
	case EStructPinType::PC_Int64:
		PinCategory = FName("int64");
		break;
	case EStructPinType::PC_Float:
		PinCategory = FName("real");
		PinSubCategory = FName("float");
		break;
	case EStructPinType::PC_Double:
		PinCategory = FName("real");
		PinSubCategory = FName("double");
		break;
	case EStructPinType::PC_Name:
		PinCategory = FName("name");
		break;
	case EStructPinType::PC_String:
		PinCategory = FName("string");
		break;
	case EStructPinType::PC_Text:
		PinCategory = FName("text");
		break;
	case EStructPinType::PC_Vector:
		PinCategory = FName("struct");
		PinSubCategoryObject = TBaseStructure<FVector>::Get();
		break;
	case EStructPinType::PC_Rotator:
		PinCategory = FName("struct");
		PinSubCategoryObject = TBaseStructure<FRotator>::Get();
		break;
	case EStructPinType::PC_Transform:
		PinCategory = FName("struct");
		PinSubCategoryObject = TBaseStructure<FTransform>::Get();
		break;
	case EStructPinType::PC_Struct:
		PinCategory = FName("struct");
		PinSubCategoryObject = GetObjectFromPath(SubStructPath);
		if(!PinSubCategoryObject)
		{
			UE_LOG(LogEasyJsonUtils, Error, TEXT("Can not construct struct pin without PinSubCategoryObject"));
			return false;
		}
		break;
	case EStructPinType::PC_Enum:
		PinCategory = FName("byte");
		PinSubCategoryObject = GetObjectFromPath(SubStructPath);
		if(!PinSubCategoryObject)
		{
			UE_LOG(LogEasyJsonUtils, Error, TEXT("Can not construct struct pin without PinSubCategoryObject"));
			return false;
		}
		break;
	}
	
	if(ContainerType == EPinContainerType::Map || ContainerType == EPinContainerType::Set)
	{
		if(!HasGetTypeHash(PinCategory,PinSubCategoryObject))
		{
			UE_LOG(LogEasyJsonUtils, Warning, TEXT("This PinCategory {%s} does not support map or set container type, change to none, please check your code!"), *PinCategory.ToString());
			ContainerType = EPinContainerType::None;
		}
		if(ContainerType == EPinContainerType::Map)
		{
			PinValueType = InPinValueType;
		}
	}
	OutPinType = FEdGraphPinType(PinCategory, PinSubCategory, PinSubCategoryObject, ContainerType, false, PinValueType);
	return true;
}

bool UEasyJsonUtils::GenerateValuePinType(FEdGraphTerminalType& OutPinType, EStructPinType PinType, FName SubStructPath)
{
	FName PinCategory = NAME_None;
	FName PinSubCategory = NAME_None;
	UObject* PinSubCategoryObject = nullptr;
	switch (PinType)
	{
	case EStructPinType::PC_Boolean:
		PinCategory = FName("bool");
		break;
	case EStructPinType::PC_Byte:
		PinCategory = FName("byte");
		break;
	case EStructPinType::PC_Int:
		PinCategory = FName("int");
		break;
	case EStructPinType::PC_Int64:
		PinCategory = FName("int64");
		break;
	case EStructPinType::PC_Float:
		PinCategory = FName("real");
		PinSubCategory = FName("float");
		break;
	case EStructPinType::PC_Double:
		PinCategory = FName("real");
		PinSubCategory = FName("double");
		break;
	case EStructPinType::PC_Name:
		PinCategory = FName("name");
		break;
	case EStructPinType::PC_String:
		PinCategory = FName("string");
		break;
	case EStructPinType::PC_Text:
		PinCategory = FName("text");
		break;
	case EStructPinType::PC_Vector:
		PinCategory = FName("struct");
		PinSubCategoryObject = TBaseStructure<FVector>::Get();
		break;
	case EStructPinType::PC_Rotator:
		PinCategory = FName("struct");
		PinSubCategoryObject = TBaseStructure<FRotator>::Get();
		break;
	case EStructPinType::PC_Transform:
		PinCategory = FName("struct");
		PinSubCategoryObject = TBaseStructure<FTransform>::Get();
		break;
	case EStructPinType::PC_Struct:
	case EStructPinType::PC_Enum:
		PinCategory = FName("byte");
		PinSubCategoryObject = GetObjectFromPath(SubStructPath);
		if(!PinSubCategoryObject)
		{
			UE_LOG(LogEasyJsonUtils, Error, TEXT("Can not construct struct pin without PinSubCategoryObject"));
			return false;
		}
		break;
	}
	OutPinType = FEdGraphTerminalType::FromPinType(FEdGraphPinType(PinCategory, PinSubCategory, PinSubCategoryObject,EPinContainerType::None, false, FEdGraphTerminalType()));
	return true;
}

UObject* UEasyJsonUtils::GetObjectFromPath(FName SoftPath)
{
	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	const FAssetData AssetData = AssetRegistryModule.Get().GetAssetByObjectPath(FSoftObjectPath(SoftPath));
	if (AssetData.IsValid())
	{
		if(UObject* Result = AssetData.FastGetAsset(true))
		{
			return Result;
		}
	}
	UE_LOG(LogEasyJsonUtils, Error, TEXT("Can not load object from path : %s."), *SoftPath.ToString());
	return nullptr;
}

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

bool UEasyJsonUtils::HasGetTypeHash(const FName& PinType,UObject* ScriptStruct)
{
	if(PinType == UEdGraphSchema_K2::PC_Boolean)
	{
		return false;
	}

	if (PinType == UEdGraphSchema_K2::PC_Text)
	{
		return false;
	}

	if (PinType != UEdGraphSchema_K2::PC_Struct)
	{
		// even object or class types can be hashed, no reason to investigate further
		return true;
	}

	const UScriptStruct* StructType = Cast<const UScriptStruct>(ScriptStruct);
	if( StructType )
	{
		return FBlueprintEditorUtils::StructHasGetTypeHash(StructType);
	}
	return false;
}




