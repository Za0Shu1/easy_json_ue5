// // Copyright 2023, liangbochao. All Rights Reserved.


#include "CreateAssets/CreateStructProxy.h"
#include "Interfaces/IPluginManager.h"
#include "UObject/Class.h"

DEFINE_LOG_CATEGORY(LogEasyJsonCreateAssets);

void UCreateStructProxy::Init(FString JsonFilePath)
{
	// @TODO: Create a widget to located json file
	if(JsonFilePath.IsEmpty())
	{
		JsonFilePath = IPluginManager::Get().FindPlugin("EasyJson")->GetBaseDir() / TEXT("Json") / "Simple.json";
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

	UE_LOG(LogEasyJsonCreateAssets,Warning,TEXT("Do Import..."));

	const TSharedRef< TJsonReader<> >& Reader = TJsonReaderFactory<>::Create(OriginJsonString);
	TSharedPtr<FJsonObject> Object;
	if (!(FJsonSerializer::Deserialize(Reader, /*out*/ Object) && Object.IsValid()))
	{
		UE_LOG(LogEasyJsonCreateAssets, Error, TEXT("Input string can not parse to json object,with error code : %s"),*Reader->GetErrorMessage());
	}

	TMap<FString,TSharedPtr<FJsonValue>> JsonAttributes = Object.Get()->Values;
	for(auto attribute : JsonAttributes)
	{
		// @TODO: Get all infomations to generate UserDefineStruct
		FString VariableName = attribute.Key;

		FString Type;
		switch (attribute.Value->Type)
		{
		case EJson::Null:
			Type = "Null";
			break;
		case EJson::String :
			Type = "String";
			break;
		case EJson::Number:
			Type = "Number";
			break;
		case EJson::Boolean:
			Type = "Boolean";
			break;
		case EJson::Array:
			Type = "Array";
			break;
		case EJson::Object:
			Type = "Object";
			break;
		default:
			Type = "None";
		}
		UE_LOG(LogEasyJsonCreateAssets, Log, TEXT("Variable Name : %s === Type : %s "),*VariableName,*Type);
	}
}
