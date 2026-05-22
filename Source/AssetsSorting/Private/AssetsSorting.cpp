// Author: Antonio Sidenko (Tonetfal), May 2026

#include "AssetsSorting.h"

#include "GenericSortableAsset.h"

IMPLEMENT_MODULE(FDefaultModuleImpl, AssetsSorting)

void UGenericAssetSortingUtilities::SortGenericAssetsArray(TArray<UObject*>& InOutSortableAssets,
	EArraySortOrder SortOrder)
{
	// Remove all unsupported assets
	InOutSortableAssets.RemoveAll([](const UObject* It)
	{
		return !IsValid(It) || !It->Implements<UGenericSortableAsset>();
	});

	// Define the sorting rules
	const auto SortingCallback = [SortOrder](int32 Lhs, int32 Rhs)
	{
		return SortOrder == EArraySortOrder::Ascending ? Lhs < Rhs : Lhs > Rhs;
	};

	// Sort the assets
	InOutSortableAssets.Sort([SortingCallback](const UObject& Lhs, const UObject& Rhs)
	{
		return SortingCallback(
			IGenericSortableAsset::Execute_GetSortingPriority(&Lhs),
			IGenericSortableAsset::Execute_GetSortingPriority(&Rhs));
	});
}
