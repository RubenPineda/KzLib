// Copyright 2025 kirzo

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "KzSystemLibrary.generated.h"

struct FKzTransformSource;

// Whether to inline functions at all
#define KZ_KISMET_SYSTEM_INLINE_ENABLED	(!UE_BUILD_DEBUG)

/** General-purpose Blueprint utilities. */
UCLASS(meta = (BlueprintThreadSafe, ScriptName = "KzSystemLibrary"))
class KZLIB_API UKzSystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	// === FKzTransformSource ===

	/** Returns true if the given transform source is valid. */
	UFUNCTION(BlueprintPure, Category = "Kz|System", meta = (Keywords = "transform source"))
	static bool IsValid(const FKzTransformSource& Source);

	/** Converts an KzTransformSource to a Vector */
	UFUNCTION(BlueprintPure, Category = "KzLib|Conversions", meta = (DisplayName = "To Vector (KzTransformSource)", CompactNodeTitle = "->", ScriptMethod = "Vector", Keywords = "cast convert transform source", BlueprintAutocast))
	static FVector Conv_KzTransformSourceToVector(const FKzTransformSource& Source);

	/** Converts an KzTransformSource to a Rotator */
	UFUNCTION(BlueprintPure, Category = "KzLib|Conversions", meta = (DisplayName = "To Rotator (KzTransformSource)", CompactNodeTitle = "->", ScriptMethod = "Rotator", Keywords = "cast convert transform source", BlueprintAutocast))
	static FRotator Conv_KzTransformSourceToRotator(const FKzTransformSource& Source);

	/** Converts an KzTransformSource to a Transform */
	UFUNCTION(BlueprintPure, Category = "KzLib|Conversions", meta = (DisplayName = "To Transform (KzTransformSource)", CompactNodeTitle = "->", ScriptMethod = "Transform", Keywords = "cast convert transform source", BlueprintAutocast))
	static FTransform Conv_KzTransformSourceToTransform(const FKzTransformSource& Source);

	/** Converts an Vector to a KzTransformSource */
	UFUNCTION(BlueprintPure, Category = "KzLib|Conversions", meta = (DisplayName = "To KzTransformSource (Vector)", CompactNodeTitle = "->", ScriptMethod = "KzTransformSource", Keywords = "cast convert transform source", BlueprintAutocast))
	static FKzTransformSource Conv_VectorToKzTransformSource(const FVector& Vector);

	/** Converts an Rotator to a KzTransformSource */
	UFUNCTION(BlueprintPure, Category = "KzLib|Conversions", meta = (DisplayName = "To KzTransformSource (Rotator)", CompactNodeTitle = "->", ScriptMethod = "KzTransformSource", Keywords = "cast convert transform source", BlueprintAutocast))
	static FKzTransformSource Conv_RotatorToKzTransformSource(const FRotator& Rotator);

	/** Converts an Transform to a KzTransformSource */
	UFUNCTION(BlueprintPure, Category = "KzLib|Conversions", meta = (DisplayName = "To KzTransformSource (Transform)", CompactNodeTitle = "->", ScriptMethod = "KzTransformSource", Keywords = "cast convert transform source", BlueprintAutocast))
	static FKzTransformSource Conv_TransformToKzTransformSource(const FTransform& Source);

	/** Converts an Actor to a KzTransformSource, using its root component's world transform. */
	UFUNCTION(BlueprintPure, Category = "KzLib|Conversions", meta = (DisplayName = "To KzTransformSource (Actor)", CompactNodeTitle = "->", ScriptMethod = "KzTransformSource", Keywords = "cast convert transform source", BlueprintAutocast))
	static FKzTransformSource Conv_ActorToKzTransformSource(const AActor* Actor);

	/** Converts a SceneComponent to a KzTransformSource. */
	UFUNCTION(BlueprintPure, Category = "KzLib|Conversions", meta = (DisplayName = "To KzTransformSource (SceneComponent)", CompactNodeTitle = "->", ScriptMethod = "KzTransformSource", Keywords = "cast convert transform source", BlueprintAutocast))
	static FKzTransformSource Conv_SceneComponentToKzTransformSource(const USceneComponent* Component, const FName SocketName = NAME_None);
};

// Inline implementations
#if KZ_KISMET_SYSTEM_INLINE_ENABLED
#include "KzSystemLibrary.inl"
#endif