// Copyright 2023, liangbochao. All Rights Reserved.

#include "EasyJsonStyle.h"
#include "EasyJsonEditor.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/SlateStyleRegistry.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"

TSharedPtr< FSlateStyleSet > FEasyJsonStyle::StyleInstance = NULL;

void FEasyJsonStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FEasyJsonStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FEasyJsonStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("EasyJsonStyle"));
	return StyleSetName;
}

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BORDER_BRUSH( RelativePath, ... ) FSlateBorderBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define TTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".ttf") ), __VA_ARGS__ )
#define OTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".otf") ), __VA_ARGS__ )

const FVector2D Icon40x40(40.0f, 40.0f);
const FVector2D Icon128x128(128.0f, 128.0f);

TSharedRef< FSlateStyleSet > FEasyJsonStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("EasyJsonStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("EasyJson")->GetBaseDir() / TEXT("Resources"));

	Style->Set("EasyJson.Import", new IMAGE_BRUSH(TEXT("Icon128"), Icon128x128));


	return Style;
}

#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH
#undef TTF_FONT
#undef OTF_FONT

void FEasyJsonStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FEasyJsonStyle::Get()
{
	return *StyleInstance;
}

