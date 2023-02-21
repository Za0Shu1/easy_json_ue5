// Copyright 2023, liangbochao. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"

/**
 * 
 */
class EASYJSONEDITOR_API FEasyJsonStyle
{
public:

 static void Initialize();

 static void Shutdown();

 static void ReloadTextures();

 static const ISlateStyle& Get();

 static FName GetStyleSetName();

private:

 static TSharedRef< class FSlateStyleSet > Create();

 static TSharedPtr< class FSlateStyleSet > StyleInstance;
};
