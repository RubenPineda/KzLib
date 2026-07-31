// Copyright 2026 kirzo

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"
#include "TimerManager.h"
#include "KzTimerSubsystem.generated.h"

/**
 * A GameInstance-lifetime FTimerManager that keeps ticking while the game is paused.
 *
 * UWorld::GetTimerManager() forwards to the GameInstance's manager, and UWorld::Tick only
 * ticks it when the world is not paused -- so there is no engine-provided SetTimer that
 * fires during pause. This subsystem owns its own FTimerManager, identical otherwise
 * (same tick delta, survives level travel), and drives it as a tickable-when-paused object.
 * Use it for timers that must fire with the game paused: menus, UI, captures.
 */
UCLASS()
class KZLIB_API UKzTimerSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	/** The pause-proof timer manager. Same API as UWorld::GetTimerManager(). */
	FTimerManager& GetTimerManager() const { return *TimerManager; }

	//~ UGameInstanceSubsystem interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	//~ FTickableGameObject interface
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickableWhenPaused() const override { return true; }
	virtual ETickableTickType GetTickableTickType() const override { return IsTemplate() ? ETickableTickType::Never : ETickableTickType::Always; }
	virtual UWorld* GetTickableGameObjectWorld() const override;
	virtual TStatId GetStatId() const override;

private:
	/** Owned manager. Created in Initialize (not inline) so it can know its game instance, like the engine one. */
	TUniquePtr<FTimerManager> TimerManager;
};
