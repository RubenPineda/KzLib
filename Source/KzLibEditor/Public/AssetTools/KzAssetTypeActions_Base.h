// Copyright 2026 kirzo

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"

class FKzAssetTypeActions_Base : public FAssetTypeActions_Base
{
public:
	explicit FKzAssetTypeActions_Base(EAssetTypeCategories::Type AssetCategory)
		: AssetCategory(AssetCategory)
	{
	}

	virtual uint32 GetCategories() override { return AssetCategory; }
	
private:
	EAssetTypeCategories::Type AssetCategory;
};

class FKzAssetTypeActions : public FKzAssetTypeActions_Base
{
public:
	const FText Name;
	const FColor Color;
	UClass* const SupportedClass;

	FKzAssetTypeActions(EAssetTypeCategories::Type AssetCategory, const FText& Name, FColor Color, UClass* SupportedClass)
		: FKzAssetTypeActions_Base(AssetCategory)
		, Name(Name)
		, Color(Color)
		, SupportedClass(SupportedClass)
	{
	}

	virtual FText GetName() const override { return Name; }
	virtual FColor GetTypeColor() const override { return Color; }
	virtual UClass* GetSupportedClass() const override { return SupportedClass; }
};