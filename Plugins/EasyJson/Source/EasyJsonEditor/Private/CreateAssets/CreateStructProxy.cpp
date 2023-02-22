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
	else
	{
		UE_LOG(LogEasyJsonCreateAssets, Log, TEXT("Struct created at : %s."),*StructSoftPath.ToString());
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

		switch (attribute.Value->Type)
		{
		// simply generate FString Pin for None and Null Type
		case EJson::None :
		case EJson::Null :
		case EJson::String :
			{
				FEdGraphPinType Pin;
				if(UEasyJsonUtils::GenerateStructPinType(Pin,EStructPinType::PC_String,FName(),EPinContainerType::None,FEdGraphTerminalType()))
				{
					StructInfo.VariableInfo.Add(attribute.Key,Pin);
				}
			}
			break;
		case EJson::Number :
			{
				FEdGraphPinType Pin;
				if(UEasyJsonUtils::GenerateStructPinType(Pin,EStructPinType::PC_Float,FName(),EPinContainerType::None,FEdGraphTerminalType()))
				{
					StructInfo.VariableInfo.Add(attribute.Key,Pin);
				}
			}
			break;
		
		case EJson::Boolean:
			{
				FEdGraphPinType Pin;
				if(UEasyJsonUtils::GenerateStructPinType(Pin,EStructPinType::PC_Boolean,FName(),EPinContainerType::None,FEdGraphTerminalType()))
				{
					StructInfo.VariableInfo.Add(attribute.Key,Pin);
				}
			}
			break;
		case EJson::Array:
			{
				// @ TODO: Generate variables or structs for json array
			}
			break;
		case EJson::Object :
			{
				// if attribute is a json object,we should generate another struct for it
				TSharedPtr<FJsonObject> ChildObj =  attribute.Value->AsObject();
				FSoftObjectPath SubStructSoftPath;
				if(CreateStructFromJsonObject(attribute.Key,ChildObj,SubStructSoftPath))
				{
					FEdGraphPinType Pin;
					if(UEasyJsonUtils::GenerateStructPinType(Pin,EStructPinType::PC_Struct,SubStructSoftPath.ToFName(),EPinContainerType::None,FEdGraphTerminalType()))
					{
						StructInfo.VariableInfo.Add(attribute.Key,Pin);
					}
				}
				else
				{
					return false;
				}
			}
			break;
		default:
			break;
		}
	}

	return GenerateStructFromDescription(StructInfo,OutStructSoftPath);
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
