// Copyright 2025 kirzo

#pragma once

#include "SimpleHandle.generated.h"

/**
 * Lightweight, generation-based handle used to safely reference elements in dense containers.
 *
 * A handle consists of:
 *   - An integer index into an indirection table or array slot.
 *   - A generation counter that invalidates old references once a slot is reused.
 */
USTRUCT(BlueprintType)
struct FSimpleHandle
{
	GENERATED_BODY()

	/** Index into the owning container's slot table or array. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Handle")
	int32 Index = INDEX_NONE;

	/** Generation counter for invalidation tracking. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Handle")
	int32 Generation = 0;

	/** Default constructor: creates an invalid handle. */
	FSimpleHandle() = default;

	/** Creates a new handle from index and generation. */
	FSimpleHandle(int32 InIndex, int32 InGeneration)
		: Index(InIndex), Generation(InGeneration)
	{
	}

	/** Copy constructor to allow safe inheritance. */
	FSimpleHandle(const FSimpleHandle&) = default;

	/** Returns whether this handle is valid (non-negative index). */
	bool IsValid() const
	{
		return Index != INDEX_NONE;
	}

	bool operator==(const FSimpleHandle& Other) const
	{
		return Index == Other.Index && Generation == Other.Generation;
	}

	bool operator!=(const FSimpleHandle& Other) const
	{
		return !(*this == Other);
	}

	friend uint32 GetTypeHash(const FSimpleHandle& Handle)
	{
		return HashCombineFast(::GetTypeHash(Handle.Index), ::GetTypeHash(Handle.Generation));
	}
};