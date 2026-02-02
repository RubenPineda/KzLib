// Copyright 2026 kirzo

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Input/KzInputModifierStack.h"
#include "KzInputModifierComponent.generated.h"

/**
 * Component that manages stacks of input modifiers.
 * It allows external actors to affect the owner's input processing
 * without knowing the specific owner class (Character, Pawn, Vehicle, etc).
 */
UCLASS(ClassGroup = (Input), meta = (BlueprintSpawnableComponent))
class KZLIB_API UKzInputModifierComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UKzInputModifierComponent();

protected:
	/** Stack of modifiers affecting Movement Input */
	UPROPERTY(Transient)
	FKzInputModifierStack MoveInputStack;

	/** Stack of modifiers affecting Look/Aim Input */
	UPROPERTY(Transient)
	FKzInputModifierStack LookInputStack;

public:
	// -------------------------------------------------------------------------
	// Movement Modifier API
	// -------------------------------------------------------------------------

	/** Adds a new modifier instance to the movement stack */
	UFUNCTION(BlueprintCallable, Category = "KzLib|Input|Modifiers")
	void PushMoveModifier(UKzInputModifier* Modifier);

	/** Removes a specific modifier instance from the movement stack */
	UFUNCTION(BlueprintCallable, Category = "KzLib|Input|Modifiers")
	void RemoveMoveModifier(UKzInputModifier* Modifier);

	/** Helper to create and add a modifier by class */
	UFUNCTION(BlueprintCallable, Category = "KzLib|Input|Modifiers", meta = (DeterminesOutputType = "ModifierClass"))
	UKzInputModifier* AddMoveModifierByClass(TSubclassOf<UKzInputModifier> ModifierClass);

	/** Processes a raw movement vector through the stack */
	UFUNCTION(BlueprintCallable, Category = "KzLib|Input|Processing")
	FVector ProcessMoveInput(const FVector& RawInput) const;

	// -------------------------------------------------------------------------
	// Look/Aim Modifier API
	// -------------------------------------------------------------------------

	/** Adds a new modifier instance to the look stack */
	UFUNCTION(BlueprintCallable, Category = "KzLib|Input|Modifiers")
	void PushLookModifier(UKzInputModifier* Modifier);

	/** Removes a specific modifier instance from the look stack */
	UFUNCTION(BlueprintCallable, Category = "KzLib|Input|Modifiers")
	void RemoveLookModifier(UKzInputModifier* Modifier);

	/** Helper to create and add a modifier by class */
	UFUNCTION(BlueprintCallable, Category = "KzLib|Input|Modifiers", meta = (DeterminesOutputType = "ModifierClass"))
	UKzInputModifier* AddLookModifierByClass(TSubclassOf<UKzInputModifier> ModifierClass);

	/** Processes a raw look vector through the stack */
	UFUNCTION(BlueprintCallable, Category = "KzLib|Input|Processing")
	FVector ProcessLookInput(const FVector& RawInput) const;
};