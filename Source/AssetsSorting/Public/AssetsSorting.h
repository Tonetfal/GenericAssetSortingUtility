// Author: Antonio Sidenko (Tonetfal), May 2026

#pragma once

#include "Kismet/KismetArrayLibrary.h"

#include "AssetsSorting.generated.h"

UCLASS()
class UGenericAssetSortingUtilities
	: public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Sorting")
	static void SortGenericAssetsArray(UPARAM(Ref) TArray<UObject*>& InOutSortableAssets,
		EArraySortOrder SortOrder = EArraySortOrder::Ascending);
};
