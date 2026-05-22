// Author: Antonio Sidenko (Tonetfal), May 2026

#pragma once

#include "UObject/Interface.h"

#include "GenericSortableAsset.generated.h"

USTRUCT(BlueprintType)
struct ASSETSSORTING_API FGenericSortableAssetDataTableEntry
	: public FTableRowBase
{
	GENERATED_BODY()

public:
	FGenericSortableAssetDataTableEntry() = default;
	FGenericSortableAssetDataTableEntry(UObject* InAsset)
		: Asset(InAsset)
	{
	}

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UObject> Asset = nullptr;
};

UINTERFACE()
class ASSETSSORTING_API UGenericSortableAsset
	: public UInterface
{
	GENERATED_BODY()
};

class ASSETSSORTING_API IGenericSortableAsset
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category="Generic Sorting")
	int32 GetSortingPriority() const;

	UFUNCTION(BlueprintNativeEvent, Category="Generic Sorting")
	void SetSortingPriority(int32 InSortingPriority);
};
