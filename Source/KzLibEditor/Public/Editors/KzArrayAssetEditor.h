// Copyright 2026 kirzo

#pragma once

#include "CoreMinimal.h"
#include "Misc/NotifyHook.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "Editors/KzArrayEditorTabConfig.h"
#include "Editors/KzExternalStructHost.h"

class IDetailsView;
class SKzPropertyStack;
class IPropertyHandle;
class SBox;
class FStructOnScope;
struct FKzStackRow;

/**
 * Generic asset editor that displays one or more array-backed tabs (each a
 * SKzPropertyStack) plus a shared Element Details panel and a Validation panel.
 *
 * Tabs are configured via FKzArrayEditorTabConfig. Customizers per tab decide how
 * each row is rendered (display text, icons, action buttons).
 *
 * FNotifyHook: struct elements live in raw array memory, so the Element Details view
 * cannot transact them through any UObject on its own; NotifyPreChange snapshots the
 * edited asset before each value write so undo works for struct entries.
 */
class KZLIBEDITOR_API FKzArrayAssetEditor : public FAssetEditorToolkit, public FNotifyHook
{
	friend class FKzArrayAssetDetailCustomization;

public:
	static TSharedRef<FKzArrayAssetEditor> CreateEditor(
		const EToolkitMode::Type Mode,
		const TSharedPtr<IToolkitHost>& InitToolkitHost,
		const TArray<UObject*>& ObjectsToEdit,
		const TArray<FKzArrayEditorTabConfig>& InTabs);

	void InitArrayAssetEditor(
		const EToolkitMode::Type Mode,
		const TSharedPtr<IToolkitHost>& InitToolkitHost,
		UObject* InAsset,
		const TArray<FKzArrayEditorTabConfig>& InTabs);

	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;

	virtual void OnClose() override;

	//~ FNotifyHook
	virtual void NotifyPreChange(FProperty* PropertyAboutToChange) override;

	/** Accessor for customizers that need to inspect the asset (e.g. to look up
	 *  cross-tab data like resolving an alias's lines). */
	UObject* GetEditedAsset() const { return AssetToEdit; }

private:
	UObject* AssetToEdit = nullptr;
	TArray<FKzArrayEditorTabConfig> Tabs;

	TSharedPtr<IDetailsView> AssetDetailsView;
	TSharedPtr<IDetailsView> ElementDetailsView;
	TSharedPtr<SBox> ElementDetailsContainer;

	/** Handle for the property-change subscription on the element details view, so we. */
	FDelegateHandle StructEditChangedHandle;
	TStrongObjectPtr<UKzExternalStructHost> ExternalStructHost;

	/** True while the primary selected row is an immutable (read-only) injected row. Gates editing. */
	bool bSelectedRowImmutable = false;

	/** Subscription to asset-detail edits, used to rebuild injected (inherited) rows on change. */
	FDelegateHandle AssetPropertyChangedHandle;

	/** Per-tab runtime state. */
	struct FTabRuntime
	{
		TArray<FName> PropertyPath;
		FText ItemName;
		TSharedPtr<FKzPropertyStackRowCustomizer> Customizer;

		TSharedPtr<IPropertyHandle> ArrayPropertyHandle;
		TSharedPtr<SKzPropertyStack> StackWidget;

		FName TabId;
	};
	TArray<FTabRuntime> TabRuntimes;

	/** Validation panel (still global to the asset). */
	TSharedPtr<class SKzValidationPanel> ValidationPanel;

	static const FName AssetDetailsTabId;
	static const FName ValidationTabId;

	TSharedRef<SDockTab> SpawnTab_AssetDetails(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnTab_ElementDetails(const FSpawnTabArgs& Args, FName ElementDetailsTabId);
	TSharedRef<SDockTab> SpawnTab_ArrayStack(const FSpawnTabArgs& Args, int32 TabIndex);
	TSharedRef<SDockTab> SpawnTab_Validation(const FSpawnTabArgs& Args);

	void OnElementsSelected(const TArray<TSharedPtr<IPropertyHandle>>& SelectedHandles);
	void OnImmutableRowSelected(TSharedPtr<FKzStackRow> Row);
	void OnArrayStackTabActivated(TSharedRef<SDockTab> ActivatedTab, ETabActivationCause Cause, int32 TabIndex);

	/** Rebuilds injected (inherited) rows when an asset-level property changes. */
	void OnAssetPropertyChanged(const struct FPropertyChangedEvent& Event);

	/** Renders the given struct instances in the shared Element Details panel. Write-back is bound
	 *  only when WriteBackHandles is non-empty (editable selections); immutable snapshots pass none. */
	void ShowStructsInPanel(const TArray<TSharedPtr<FStructOnScope>>& Structs, const TArray<TSharedPtr<IPropertyHandle>>& WriteBackHandles, const FText& HeaderLabel);

	/** Element-details editing gate: disabled while an immutable row is selected. */
	bool IsElementEditingEnabled() const { return !bSelectedRowImmutable; }

	void OnRunValidation();
	TArray<struct FKzValidationIssue> HandleRunValidation();
	void HandleValidationIssueActivated(const struct FKzValidationIssue& Issue);

	void ExtendToolbar();

	/** Returns "Element Details" tab id (singular per editor; only one details panel). */
	static FName GetElementDetailsTabId() { return TEXT("KzArrayEditor_ElementDetails"); }

	/** Returns the unique tab id for a given array stack tab. */
	static FName MakeArrayStackTabId(int32 Index)
	{
		return FName(*FString::Printf(TEXT("KzArrayEditor_ArrayStack_%d"), Index));
	}
};