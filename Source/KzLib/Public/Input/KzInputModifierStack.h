// Copyright 2026 kirzo

#pragma once

#include "CoreMinimal.h"
#include "Input/KzInputModifier.h"
#include "KzInputModifierStack.generated.h"

/** Encapsulates a stack of Input Modifiers and handles the processing logic. */
USTRUCT(BlueprintType)
struct KZLIB_API FKzInputModifierStack
{
	GENERATED_BODY()

private:
	/** The ordered list of modifiers. Processed from index 0 to N. */
	UPROPERTY(Transient)
	TArray<TObjectPtr<UKzInputModifier>> Modifiers;

public:
	/**
	 * Pushes a new modifier onto the stack.
	 * @param Modifier The modifier instance to add.
	 */
	void Push(UKzInputModifier* Modifier)
	{
		if (Modifier)
		{
			Modifiers.Add(Modifier);
		}
	}

	/**
	 * Removes a specific modifier instance from the stack.
	 * @param Modifier The modifier instance to remove.
	 */
	void Remove(UKzInputModifier* Modifier)
	{
		if (Modifier)
		{
			Modifiers.Remove(Modifier);
		}
	}

	/**
	 * Clears all modifiers from the stack.
	 */
	void Clear()
	{
		Modifiers.Empty();
	}

	/**
	 * Processes the input vector through the entire stack of modifiers.
	 * @param RawInput The original input vector from the controller.
	 * @return The final modified input vector.
	 */
	FVector Process(const FVector& RawInput) const
	{
		FVector CurrentInput = RawInput;

		// Iterate through the stack sequentially.
		// Pipeline: Raw -> Mod[0] -> Mod[1] -> ... -> Result
		for (const UKzInputModifier* Mod : Modifiers)
		{
			if (Mod)
			{
				CurrentInput = Mod->ModifyInput(RawInput, CurrentInput);
			}
		}

		return CurrentInput;
	}

	/** Returns true if the stack is empty. */
	bool IsEmpty() const
	{
		return Modifiers.IsEmpty();
	}
};