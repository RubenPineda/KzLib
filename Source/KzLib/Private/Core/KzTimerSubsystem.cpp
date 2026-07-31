// Copyright 2026 kirzo

#include "Core/KzTimerSubsystem.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"

void UKzTimerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	TimerManager = MakeUnique<FTimerManager>(GetGameInstance());
}

void UKzTimerSubsystem::Deinitialize()
{
	TimerManager.Reset();

	Super::Deinitialize();
}

void UKzTimerSubsystem::Tick(float DeltaTime)
{
	// Ticked from UWorld::Tick's tickable pass, which runs paused or not; the pause gate that stops the
	// engine's manager (UWorld::Tick only ticks it when !bIsPaused) is exactly what we bypass here.
	if (TimerManager)
	{
		TimerManager->Tick(DeltaTime);
	}
}

UWorld* UKzTimerSubsystem::GetTickableGameObjectWorld() const
{
	// Tickables only tick when this matches the ticking world: bind to our game instance's current one.
	const UGameInstance* GameInstance = GetGameInstance();
	return GameInstance ? GameInstance->GetWorld() : nullptr;
}

TStatId UKzTimerSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UKzTimerSubsystem, STATGROUP_Tickables);
}
