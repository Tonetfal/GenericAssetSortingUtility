// Author: Antonio Sidenko (Tonetfal), May 2026

#pragma once

#include "AssetActionUtility.h"
#include "Kismet/KismetArrayLibrary.h"

#include "GenericAssetSortingUtility.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogGenericAssetSorting, All, All);

UCLASS(Abstract)
class ASSETSSORTING_API UGenericAssetSortingUtility
	: public UAssetActionUtility
{
	GENERATED_BODY()

public:
	UFUNCTION(CallInEditor, Category="Sorting")
	void PopulateDataTable(
		UPARAM(meta=(RequiredAssetDataTags="RowStructure=/Script/AssetsSorting.GenericSortableAssetDataTableEntry"))
		UDataTable* DataTable,
		EArraySortOrder SortOrder = EArraySortOrder::Ascending);

	UFUNCTION(CallInEditor, Category="Sorting")
	void SortOutAssets(
		UPARAM(meta=(RequiredAssetDataTags="RowStructure=/Script/AssetsSorting.GenericSortableAssetDataTableEntry"))
		UDataTable* DataTable);

protected:
	UFUNCTION(BlueprintNativeEvent, Category="Sorting")
	bool SupportsSpecificAssetType(const UObject* InAsset) const;

	UFUNCTION(BlueprintNativeEvent, Category="Sorting")
	bool CompareSpecificAssets(const UObject* Lhs, const UObject* Rhs, EArraySortOrder SortOrder) const;

	UFUNCTION(BlueprintNativeEvent, Category="Sorting")
	void SetAssetSortingPriority(UObject* InAsset, int32 InPriority) const;
};

UCLASS(Config="Editor", DefaultConfig)
class UGenericAssetSortingSettings
	: public UDeveloperSettings
{
	GENERATED_BODY()

public:
	static UDataTable* GetDefaultDataTable();

public:
	UPROPERTY(Config, EditAnywhere, Category="Asset Sorting",
		meta=(RequiredAssetDataTags="RowStructure=/Script/AssetsSorting.GenericSortableAssetDataTableEntry"))
	TSoftObjectPtr<UDataTable> DefaultDataTable = nullptr;
};

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
