// Copyright 2026 kirzo

#include "Input/Components/KzInputModifierComponent.h"

UKzInputModifierComponent::UKzInputModifierComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = false;
}

// -------------------------------------------------------------------------
// Movement
// -------------------------------------------------------------------------

void UKzInputModifierComponent::PushMoveModifier(UKzInputModifier* Modifier)
{
	if (Modifier)
	{
		MoveInputStack.Push(Modifier);
	}
}

void UKzInputModifierComponent::RemoveMoveModifier(UKzInputModifier* Modifier)
{
	MoveInputStack.Remove(Modifier);
}

UKzInputModifier* UKzInputModifierComponent::AddMoveModifierByClass(TSubclassOf<UKzInputModifier> ModifierClass)
{
	if (!ModifierClass)
	{
		return nullptr;
	}

	UKzInputModifier* NewMod = NewObject<UKzInputModifier>(this, ModifierClass);
	PushMoveModifier(NewMod);
	return NewMod;
}

FVector UKzInputModifierComponent::ProcessMoveInput(const FVector& RawInput) const
{
	return MoveInputStack.Process(RawInput);
}

// -------------------------------------------------------------------------
// Look
// -------------------------------------------------------------------------

void UKzInputModifierComponent::PushLookModifier(UKzInputModifier* Modifier)
{
	if (Modifier)
	{
		LookInputStack.Push(Modifier);
	}
}

void UKzInputModifierComponent::RemoveLookModifier(UKzInputModifier* Modifier)
{
	LookInputStack.Remove(Modifier);
}

UKzInputModifier* UKzInputModifierComponent::AddLookModifierByClass(TSubclassOf<UKzInputModifier> ModifierClass)
{
	if (!ModifierClass)
	{
		return nullptr;
	}

	UKzInputModifier* NewMod = NewObject<UKzInputModifier>(this, ModifierClass);
	PushLookModifier(NewMod);
	return NewMod;
}

FVector UKzInputModifierComponent::ProcessLookInput(const FVector& RawInput) const
{
	return LookInputStack.Process(RawInput);
}