// Author: Antonio Sidenko (Tonetfal), May 2026

#include "GenericAssetSortingUtility.h"

#include "EditorAssetLibrary.h"
#include "EditorUtilityLibrary.h"
#include "Framework/Notifications/NotificationManager.h"
#include "GenericSortableAsset.h"
#include "Logging/StructuredLog.h"
#include "Subsystems/EditorAssetSubsystem.h"
#include "Widgets/Notifications/SNotificationList.h"

DEFINE_LOG_CATEGORY(LogGenericAssetSorting);

namespace
{
	void ShowWarningNotification(const FText& Message)
	{
		FNotificationInfo Info(Message);

		Info.ExpireDuration = 5.0f;
		Info.bUseLargeFont = false;

		Info.Image = FCoreStyle::Get().GetBrush("MessageLog.Warning");

		TSharedPtr<SNotificationItem> Notification =
			FSlateNotificationManager::Get().AddNotification(Info);

		if (Notification.IsValid())
		{
			Notification->SetCompletionState(SNotificationItem::CS_Fail);
			Notification->ExpireAndFadeout();
		}
	}
}

void UGenericAssetSortingUtility::PopulateDataTable(UDataTable* DataTable, EArraySortOrder SortOrder)
{
#if WITH_EDITOR
	if (!IsValid(DataTable))
	{
		DataTable = UGenericAssetSortingSettings::GetDefaultDataTable();
		if (!IsValid(DataTable))
		{
			ShowWarningNotification(INVTEXT("Data Table is invalid"));
			return;
		}
	}

	// Check out the data table
	if (!UEditorAssetLibrary::CheckoutLoadedAsset(DataTable))
	{
		ShowWarningNotification(INVTEXT("Failed to checkout data table"));
		return;
	}

	// Leave only assets that are sortable
	TArray<UObject*> AssetsToSort = UEditorUtilityLibrary::GetSelectedAssets();
	AssetsToSort.RemoveAll([this](const UObject* It)
	{
		if (!IsValid(It))
		{
			return true;
		}

		if (SupportsSpecificAssetType(It))
		{
			return false;
		}

		return true;
	});

	// Sort the assets
	AssetsToSort.Sort([&](const UObject& Lhs, const UObject& Rhs)
	{
		return CompareSpecificAssets(&Lhs, &Rhs, SortOrder);
	});

	// Clear out the map
	const TArray<FName> RowNames = DataTable->GetRowNames();
	for (const FName& RowName : RowNames)
	{
		DataTable->RemoveRow(RowName);
	}

	// Add all assets to the data table
	for (UObject* Asset : AssetsToSort)
	{
		const FName RowName = FName(Asset->GetName());
		const FGenericSortableAssetDataTableEntry Entry(Asset);
		DataTable->AddRow(RowName, Entry);
	}

	// Save the data table
	auto* EditorAssetSubsystem = GEditor->GetEditorSubsystem<UEditorAssetSubsystem>();
	DataTable->MarkPackageDirty();
	EditorAssetSubsystem->SaveAsset(DataTable->GetName(), false);

	UE_LOGFMT(LogGenericAssetSorting, Log, "Sorted {0} assets", AssetsToSort.Num());
#endif
}

void UGenericAssetSortingUtility::SortOutAssets(UDataTable* DataTable)
{
#if WITH_EDITOR
	if (!IsValid(DataTable))
	{
		DataTable = UGenericAssetSortingSettings::GetDefaultDataTable();
		if (!IsValid(DataTable))
		{
			ShowWarningNotification(INVTEXT("Data Table is invalid"));
			return;
		}
	}

	const TMap<FName, uint8*>& RowMap = DataTable->GetRowMap();
	TMap<int32, TObjectPtr<UObject>> SortedAssets;

	// Read the data table in a map to apply it later
	int32 i = 0;
	for (const auto& [RowName, Memory] : RowMap)
	{
		const int32 CurrentIndex = i;
		i++;

		const auto* Entry = reinterpret_cast<FGenericSortableAssetDataTableEntry*>(Memory);
		if (!IsValid(Entry->Asset))
		{
			ShowWarningNotification(FText::FromString(
				FString::Printf(TEXT("Asset index [%d] is invalid"),
					CurrentIndex)));

			continue;
		}

		if (!SupportsSpecificAssetType(Entry->Asset))
		{
			ShowWarningNotification(FText::FromString(
				FString::Printf(TEXT("Asset [%s] isn't supported"),
					*Entry->Asset->GetName())));

			continue;
		}

		SortedAssets.Emplace(CurrentIndex, Entry->Asset);
	}

	// Checkout the assets
	TArray<TObjectPtr<UObject>> Assets;
	SortedAssets.GenerateValueArray(Assets);
	if (!UEditorAssetLibrary::CheckoutLoadedAssets(Assets))
	{
		ShowWarningNotification(INVTEXT("Failed to checkout assets to sort"));
		return;
	}

	// Assign everything
	auto* EditorAssetSubsystem = GEditor->GetEditorSubsystem<UEditorAssetSubsystem>();
	for (const auto& [CurrentIndex, Asset] : SortedAssets)
	{
		SetAssetSortingPriority(Asset, CurrentIndex);

		// Make sure that the changes are saved
		Asset->MarkPackageDirty();
		EditorAssetSubsystem->SaveAsset(Asset->GetName(), false);
	}

	UE_LOGFMT(LogGenericAssetSorting, Log, "Assigned {0} ordering indices", SortedAssets.Num());
#endif
}

bool UGenericAssetSortingUtility::SupportsSpecificAssetType_Implementation(const UObject* InAsset) const
{
	if (InAsset->Implements<UGenericSortableAsset>())
	{
		return true;
	}

	return false;
}

bool UGenericAssetSortingUtility::CompareSpecificAssets_Implementation(const UObject* Lhs, const UObject* Rhs,
	EArraySortOrder SortOrder) const
{
	const auto SortingCallback = [SortOrder](int32 Lhs, int32 Rhs)
	{
		return SortOrder == EArraySortOrder::Ascending ? Lhs < Rhs : Lhs > Rhs;
	};

	if (Lhs->Implements<UGenericSortableAsset>() && Rhs->Implements<UGenericSortableAsset>())
	{
		return SortingCallback(
			IGenericSortableAsset::Execute_GetSortingPriority(Lhs),
			IGenericSortableAsset::Execute_GetSortingPriority(Rhs));
	}

	return false;
}

void UGenericAssetSortingUtility::SetAssetSortingPriority_Implementation(UObject* InAsset, int32 InPriority) const
{
	if (InAsset->Implements<UGenericSortableAsset>())
	{
		IGenericSortableAsset::Execute_SetSortingPriority(InAsset, InPriority);
	}
}

UDataTable* UGenericAssetSortingSettings::GetDefaultDataTable()
{
	const auto* This = GetDefault<ThisClass>();

	// This is bad, but it's a testing DT either way
	if (This->DefaultDataTable.IsNull())
	{
		FSoftObjectPath Path(TEXT("/AssetsSorting/DT_Sorting.DT_Sorting"));
		TSoftObjectPtr<UDataTable> SoftPtr(Path);
		return SoftPtr.LoadSynchronous();
	}

	return This->DefaultDataTable.LoadSynchronous();
}

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
