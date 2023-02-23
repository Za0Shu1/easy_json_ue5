// // Copyright 2023, liangbochao. All Rights Reserved.


#include "CreateAssets/CreateStructProxy.h"

// ENGINE HEADER
//editor
#include "Editor.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "Interfaces/IPluginManager.h"
#include "Subsystems/EditorAssetSubsystem.h"

// asset tool
#include "AssetDeleteModel.h"
#include "ObjectTools.h"

// struct
#include "Engine/UserDefinedStruct.h"
#include "Kismet2/StructureEditorUtils.h"
#include "UserDefinedStructure/UserDefinedStructEditorData.h"

// misc
#include "Misc/FileHelper.h"
#include "Misc/MessageDialog.h"

// json
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

// PLUGIN HEADER
#include "Common/EasyJsonTypes.h"
#include "Common/EasyJsonUtils.h"
#include "JsonUtils/JsonPointer.h"

DEFINE_LOG_CATEGORY(LogEasyJsonCreateAssets);

#define LOCTEXT_NAMESPACE "FEasyJsonModule"

void UCreateStructProxy::Init(FString JsonFilePath)
{
	// @TODO: Create a widget to located json file
	if(JsonFilePath.IsEmpty())
	{
		JsonFilePath = IPluginManager::Get().FindPlugin("EasyJson")->GetBaseDir() / TEXT("Json") / JsonFileName + TEXT(".json");
	}
	
	UE_LOG(LogEasyJsonCreateAssets,Log,TEXT("Select json file at : %s"),*JsonFilePath);
	
	if(!FPaths::FileExists(JsonFilePath))
	{
		UE_LOG(LogEasyJsonCreateAssets,Warning,TEXT("Can not find json file"));
	}

	if(!FFileHelper::LoadFileToString(OriginJsonString,*JsonFilePath))
	{
		UE_LOG(LogEasyJsonCreateAssets,Error,TEXT("Can not load json file to string"));
	}

	UE_LOG(LogEasyJsonCreateAssets,Log,TEXT(" \r\n%s"),*OriginJsonString);
}

void UCreateStructProxy::DoImport()
{
	if(OriginJsonString.IsEmpty())
	{
		UE_LOG(LogEasyJsonCreateAssets,Warning,TEXT("Json string is empty,please check your input."));
		return;
	}

	// test code
	if (false)
	{
		FStructDescription InStruct;

		const FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
		IContentBrowserSingleton& ContentBrowserSingleton = ContentBrowserModule.Get();
		const FString SavePath = ContentBrowserSingleton.GetCurrentPath().GetInternalPathString();

		InStruct.AssetName = FName("Simple");
		InStruct.PackageName = SavePath / InStruct.AssetName.ToString();
	
		FEdGraphPinType PinType;
		FEdGraphTerminalType ValuePinType;
		bool bValueType = UEasyJsonUtils::GenerateValuePinType(ValuePinType,EStructPinType::PC_Struct,FName(SavePath + "/TestStruct1.TestStruct1"));
		if(!bValueType)
		{
			UE_LOG(LogTemp, Error, TEXT("Invalid ValuePinType."));
			return;
		}
			
		bool bResult = UEasyJsonUtils::GenerateStructPinType(PinType,EStructPinType::PC_String,FName(),EPinContainerType::Map,ValuePinType);
		if(!bResult)
		{
			UE_LOG(LogTemp, Error, TEXT("Invalid PinType."));
			return;
		}
	
		InStruct.VariableInfo.Add("V1",PinType);
	
		FSoftObjectPath OutputFile;
		GenerateStructFromDescription(InStruct,OutputFile);
		return;
	}
	const TSharedRef< TJsonReader<> >& Reader = TJsonReaderFactory<>::Create(OriginJsonString);
	TSharedPtr<FJsonObject> Object;
	if (!(FJsonSerializer::Deserialize(Reader, /*out*/ Object) && Object.IsValid()))
	{
		UE_LOG(LogEasyJsonCreateAssets, Error, TEXT("Input string can not parse to json object,with error code : %s"),*Reader->GetErrorMessage());
	}

	FSoftObjectPath StructSoftPath;
	if(!CreateStructFromJsonObject(JsonFileName,Object,StructSoftPath))
	{
		UE_LOG(LogEasyJsonCreateAssets, Error, TEXT("Can not create structs asset from json object."));
	}
}

// this is a recurrence method, we can generate struct recursively
bool UCreateStructProxy::CreateStructFromJsonObject(FString StructName, const TSharedPtr<FJsonObject>& InObject, FSoftObjectPath& OutStructSoftPath)
{
	FStructDescription StructInfo;

	// get struct save path
	const FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	IContentBrowserSingleton& ContentBrowserSingleton = ContentBrowserModule.Get();
	const FString SavePath = ContentBrowserSingleton.GetCurrentPath().GetInternalPathString();

	StructInfo.AssetName = FName(StructName);
	StructInfo.PackageName = SavePath / StructName;


	TMap<FString,TSharedPtr<FJsonValue>> JsonAttributes = InObject.Get()->Values;
	for(auto attribute : JsonAttributes)
	{
		FString VariableName = attribute.Key;
		UE_LOG(LogEasyJsonCreateAssets, Log, TEXT("Struct{%s}, Variable{%s}, Type{%s}."),*StructName,*VariableName,*GetJsonTypeNameString(attribute.Value->Type));

		// calc pin type
		FEdGraphPinType Pin;
		EStructPinType PinMainType = EStructPinType::PC_String;
		FName SubStructPath = FName();
		EPinContainerType ContainerType = EPinContainerType::None;
		FEdGraphTerminalType TerminalType = FEdGraphTerminalType();
		
		if(!GeneratePinFromJsonValueType(attribute,PinMainType,ContainerType,SubStructPath))
		{
			return false;
		}
		else
		{
			if(UEasyJsonUtils::GenerateStructPinType(Pin,PinMainType,SubStructPath,ContainerType,TerminalType))
			{
				StructInfo.VariableInfo.Add(VariableName,Pin);
			}
		}
	}
	return GenerateStructFromDescription(StructInfo,OutStructSoftPath);
}

bool UCreateStructProxy::GeneratePinFromJsonValueType(TTuple<FString, TSharedPtr<FJsonValue>> InAttribute,
	EStructPinType& OutPinType, EPinContainerType& OutContainerType, FName& OutSubStructPath)
{
	OutPinType = EStructPinType::PC_String;
	OutSubStructPath = FName();
	OutContainerType = EPinContainerType::None;
	
	switch (InAttribute.Value->Type)
	{
		// just using default pin
	case EJson::None:
	case EJson::Null:
	case EJson::String:
		break;
	case EJson::Number:
		OutPinType = EStructPinType::PC_Float;
		break;
	case EJson::Boolean:
		OutPinType = EStructPinType::PC_Boolean;
		break;
	case EJson::Array:
		{
			OutContainerType = EPinContainerType::Array;
			TArray<TSharedPtr<FJsonValue>> JsonValues =  InAttribute.Value->AsArray();
			if(JsonValues.Num() <= 0)
			{
				UE_LOG(LogEasyJsonCreateAssets, Warning, TEXT("There is a json value type is Array, but don not have any variable,can not decide to use which pintype."));
			}
			TSharedPtr<FJsonValue> TempValue = JsonValues[0];

			// check if values is a Multidimensional array
			if(TempValue->Type == EJson::Array)
			{
				const FString DisplayLog = "If you want use multidimensional array json, you can not use the default method FJsonObjectConverter::JsonObjectToUStruct, \n"\
				"because this method JsonValueToFPropertyWithContainer in JsonObjectConverter will check the ArrayDim value. If ArrayDim > 1, values will be ignored. \n"\
				"There are some solutions for this situation: \n"\
				"\t1. Reconstruct your json structure, avoid using Multidimensional array; \n"\
				"\t2. Define a custom structrue,and Define a method same like FJsonObjectConverter::JsonObjectToUStruct, deserialize json by yourself.";
				
				UE_LOG(LogEasyJsonCreateAssets, Warning, TEXT("Value type is multidimensional array,do not support yet. Simply using string pin."));
				UE_LOG(LogEasyJsonCreateAssets, Warning, TEXT("%s"),*DisplayLog);
				OutPinType = EStructPinType::PC_String;
				OutContainerType = EPinContainerType::None;
			}
			else
			{
				FString TempKey = InAttribute.Key + "_Item";
				TTuple<FString, TSharedPtr<FJsonValue>> TempAttribute(TempKey,TempValue);

				EStructPinType TempPinType;
				FName TempSubStructPath;
				EPinContainerType TempContainerType;
				if(GeneratePinFromJsonValueType(TempAttribute,TempPinType,TempContainerType,TempSubStructPath))
				{
					OutPinType = TempPinType;
					OutSubStructPath = TempSubStructPath;
				}
				else
				{
					UE_LOG(LogEasyJsonCreateAssets, Warning, TEXT("Can not generate correct pintype for %s . "),*InAttribute.Key);
				}
			}
		}
		break;
	case EJson::Object:
		{
			TSharedPtr<FJsonObject> ChildObj =  InAttribute.Value->AsObject();
			FSoftObjectPath SubStructSoftPath;
			if(CreateStructFromJsonObject(InAttribute.Key,ChildObj,SubStructSoftPath))
			{
				OutPinType = EStructPinType::PC_Struct;
				OutSubStructPath = *SubStructSoftPath.ToString();
			}
			else
			{
				return false;
			}
		}
		break;
	}
	return true;
}


bool UCreateStructProxy::GenerateStructFromDescription(FStructDescription& InDescription, FSoftObjectPath& CreatedStructSoftPath)
{
	/**
	 *Step 1 : Delete asset if already exists
	**/
	if (DeleteAssetByPackageName(InDescription.PackageName))
	{
		UPackage* StructPackage = CreatePackage(*InDescription.PackageName);
		if (!StructPackage)
		{
			UE_LOG(LogEasyJsonCreateAssets, Error, TEXT("Can not create package"));
			return  false;
		}

		/**
		 *Step 2 : Create Asset
		**/
		UUserDefinedStruct* OutStruct = FStructureEditorUtils::CreateUserDefinedStruct(StructPackage, InDescription.AssetName, RF_Public | RF_Standalone | RF_Transactional);
		if (!OutStruct)
		{
			UE_LOG(LogEasyJsonCreateAssets, Error, TEXT("Can not create UserDefinedStruct"));
			return false;
		}

		/**
		 *Step 3 : Register Asset
		**/
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
		// register new asset,or we can not see in content browser
		AssetRegistryModule.Get().AssetCreated(OutStruct);

		for(auto variable : InDescription.VariableInfo)
		{

			/**
			 *Step 4 : Add Variables
			**/
			if(FStructureEditorUtils::AddVariable(OutStruct, variable.Value))
			{
				/**
				 *Step 5 : Rename Variables
				**/
				TArray<FStructVariableDescription> VarDescArray = FStructureEditorUtils::GetVarDesc(OutStruct);
				if(!FStructureEditorUtils::RenameVariable(OutStruct, VarDescArray[VarDescArray.Num() - 1].VarGuid, variable.Key))
				{
					UE_LOG(LogEasyJsonCreateAssets, Error, TEXT("Rename Variable Failed."));
				}
			}
			else
			{
				UE_LOG(LogEasyJsonCreateAssets, Error, TEXT("Can not add variable to struct : %s."), *InDescription.AssetName.ToString());
			}
		}

		/**
		 *Step 6 : Remove Default Variable
		**/

		TArray<FStructVariableDescription> VarDescArray = FStructureEditorUtils::GetVarDesc(OutStruct);
		if (VarDescArray.Num() > 0)
		{
			if (!FStructureEditorUtils::RemoveVariable(OutStruct, VarDescArray[0].VarGuid))
			{
				UE_LOG(LogTemp, Warning, TEXT("Member variable cannot be removed. User Defined Structure cannot be empty"));
			}
		}

		/**
		 *Step 7 : Save Asset
		**/
		UEditorAssetSubsystem* EditorAssetSubsystem = GEditor->GetEditorSubsystem<UEditorAssetSubsystem>();
		EditorAssetSubsystem->SaveAsset(InDescription.PackageName, true);
		
		InDescription.bAlreadyCreated = true;
		CreatedStructSoftPath = FSoftObjectPath(OutStruct);
		UE_LOG(LogEasyJsonCreateAssets, Warning, TEXT("Struct{%s} created at : %s."),*InDescription.AssetName.ToString(),*CreatedStructSoftPath.ToString());
		return true;
	}
	return false;
}


FString UCreateStructProxy::GetJsonTypeNameString(EJson JsonType)
{
	FString Result;
	switch (JsonType)
	{
	case EJson::Null:
		Result = "Null";
		break;
	case EJson::String :
		Result = "String";
		break;
	case EJson::Number:
		Result = "Number";
		break;
	case EJson::Boolean:
		Result = "Boolean";
		break;
	case EJson::Array:
		Result = "Array";
		break;
	case EJson::Object:
		Result = "Object";
		break;
	default:
		Result = "None";
	}
	return Result;
}

bool UCreateStructProxy::DeleteAssetByPackageName(FString PackageName) const
{
	// @ TODO: check refrences for package, if there is some refrence to this package,we can't delete it
	if (UObject* DeleteObj = FindPackage(nullptr, *PackageName))
	{
		// FAssetDeleteModel DeleteTemp({DeleteObj});
		// DeleteTemp.DoDelete();

		TArray<UObject*> AssetsToDelete;
		AssetsToDelete.Add(DeleteObj);
		if(ObjectTools::ForceDeleteObjects(AssetsToDelete,false) != AssetsToDelete.Num())
		{
			FText DialogText = FText::Format(LOCTEXT("CannotDeleteAsset","Can not Delete Asset : {0} "),FText::FromString(PackageName));
			FMessageDialog::Open(EAppMsgType::Ok,DialogText);
			return false;
		}
	}
	return true;
}


#undef LOCTEXT_NAMESPACE
