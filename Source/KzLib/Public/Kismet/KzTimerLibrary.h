// Copyright 2026 kirzo

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TimerManager.h"
#include "KzTimerLibrary.generated.h"

/**
 * Blueprint API for UKzTimerSubsystem: a mirror of the engine's timer nodes
 * (UKismetSystemLibrary) that runs on the pause-proof Kz timer manager instead
 * of the world's. Same node set, same semantics -- the only difference is that
 * these timers keep counting while the game is paused.
 *
 * Handles are regular FTimerHandles: the stock "Is Valid Timer Handle" and
 * "Invalidate Timer Handle" nodes work on them. But a handle belongs to the
 * manager that created it -- always manipulate Kz timers through Kz nodes.
 */
UCLASS()
class KZLIB_API UKzTimerLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// --- Set ------------------------------------------

	/**
	 * Set a pause-proof timer to execute a delegate. Setting an existing timer will reset that timer with updated parameters.
	 * @param Event						Event. Can be a K2 function or a Custom Event.
	 * @param Time						How long to wait before executing the delegate, in seconds. Setting a timer to <= 0 seconds will clear it if it is set.
	 * @param bLooping					True to keep executing the delegate every Time seconds, false to execute delegate only once.
	 * @param bMaxOncePerFrame			For looping timers, whether to execute only once when the timer would otherwise expire multiple times in the current frame.
	 * @param InitialStartDelay			Initial delay passed to the timer manager, in seconds.
	 * @param InitialStartDelayVariance	Use this to add some variance to when the timer starts in lieu of doing a random range on the InitialStartDelay input, in seconds.
	 * @return							The timer handle to pass to other Kz timer functions to manipulate this timer.
	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Kz Timer by Event", ScriptName = "SetKzTimerDelegate", AdvancedDisplay = "InitialStartDelay, InitialStartDelayVariance"), Category = "KzLib|Timer")
	static FTimerHandle K2_SetTimerDelegate(UPARAM(DisplayName = "Event") FTimerDynamicDelegate Delegate, float Time, bool bLooping, bool bMaxOncePerFrame = false, float InitialStartDelay = 0.f, float InitialStartDelayVariance = 0.f);

	/**
	 * Set a pause-proof timer to execute a delegate. Setting an existing timer will reset that timer with updated parameters.
	 * @param Object					Object that implements the delegate function. Defaults to self (this blueprint).
	 * @param FunctionName				Delegate function name. Can be a K2 function or a Custom Event.
	 * @param Time						How long to wait before executing the delegate, in seconds. Setting a timer to <= 0 seconds will clear it if it is set.
	 * @param bLooping					True to keep executing the delegate every Time seconds, false to execute delegate only once.
	 * @param bMaxOncePerFrame			For looping timers, whether to execute only once when the timer would otherwise expire multiple times in the current frame.
	 * @param InitialStartDelay			Initial delay passed to the timer manager, in seconds.
	 * @param InitialStartDelayVariance	Use this to add some variance to when the timer starts in lieu of doing a random range on the InitialStartDelay input, in seconds.
	 * @return							The timer handle to pass to other Kz timer functions to manipulate this timer.
	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Kz Timer by Function Name", ScriptName = "SetKzTimer", DefaultToSelf = "Object", AdvancedDisplay = "InitialStartDelay, InitialStartDelayVariance"), Category = "KzLib|Timer")
	static FTimerHandle K2_SetTimer(UObject* Object, FString FunctionName, float Time, bool bLooping, bool bMaxOncePerFrame = false, float InitialStartDelay = 0.f, float InitialStartDelayVariance = 0.f);

	/**
	 * Set a pause-proof timer to execute a delegate on the manager's next tick. Beware: if set before this frame's
	 * world tick (e.g. from an input event), "next tick" is still THIS frame -- use a small Time for a true frame delay.
	 * @param Event	Event. Can be a K2 function or a Custom Event.
	 * @return		The timer handle to pass to other Kz timer functions to manipulate this timer.
	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Kz Timer for Next Tick by Event", ScriptName = "SetKzTimerForNextTickDelegate"), Category = "KzLib|Timer")
	static FTimerHandle K2_SetTimerForNextTickDelegate(UPARAM(DisplayName = "Event") FTimerDynamicDelegate Delegate);

	/**
	 * Set a pause-proof timer to execute a delegate on the manager's next tick. Beware: if set before this frame's
	 * world tick (e.g. from an input event), "next tick" is still THIS frame -- use a small Time for a true frame delay.
	 * @param Object		Object that implements the delegate function. Defaults to self (this blueprint).
	 * @param FunctionName	Delegate function name. Can be a K2 function or a Custom Event.
	 * @return				The timer handle to pass to other Kz timer functions to manipulate this timer.
	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Kz Timer for Next Tick by Function Name", ScriptName = "SetKzTimerForNextTick", DefaultToSelf = "Object"), Category = "KzLib|Timer")
	static FTimerHandle K2_SetTimerForNextTick(UObject* Object, FString FunctionName);

	// --- By handle ------------------------------------

	/** Clears a set Kz timer and invalidates the handle. */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Clear and Invalidate Kz Timer by Handle", ScriptName = "ClearAndInvalidateKzTimerHandle", WorldContext = "WorldContextObject"), Category = "KzLib|Timer")
	static void K2_ClearAndInvalidateTimerHandle(const UObject* WorldContextObject, UPARAM(ref) FTimerHandle& Handle);

	/** Pauses a set Kz timer at its current elapsed time. */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Pause Kz Timer by Handle", ScriptName = "PauseKzTimerHandle", WorldContext = "WorldContextObject"), Category = "KzLib|Timer")
	static void K2_PauseTimerHandle(const UObject* WorldContextObject, FTimerHandle Handle);

	/** Resumes a paused Kz timer from its current elapsed time. */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Unpause Kz Timer by Handle", ScriptName = "UnPauseKzTimerHandle", WorldContext = "WorldContextObject"), Category = "KzLib|Timer")
	static void K2_UnPauseTimerHandle(const UObject* WorldContextObject, FTimerHandle Handle);

	/** Returns true if a Kz timer exists and is active for the given handle. */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Is Kz Timer Active by Handle", ScriptName = "IsKzTimerActiveHandle", WorldContext = "WorldContextObject"), Category = "KzLib|Timer")
	static bool K2_IsTimerActiveHandle(const UObject* WorldContextObject, FTimerHandle Handle);

	/** Returns true if a Kz timer exists and is paused for the given handle. */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Is Kz Timer Paused by Handle", ScriptName = "IsKzTimerPausedHandle", WorldContext = "WorldContextObject"), Category = "KzLib|Timer")
	static bool K2_IsTimerPausedHandle(const UObject* WorldContextObject, FTimerHandle Handle);

	/** Returns true if a Kz timer for the given handle exists. */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Does Kz Timer Exist by Handle", ScriptName = "KzTimerExistsHandle", WorldContext = "WorldContextObject"), Category = "KzLib|Timer")
	static bool K2_TimerExistsHandle(const UObject* WorldContextObject, FTimerHandle Handle);

	/** Returns elapsed time for the given handle (time since current countdown iteration began). -1 if no such timer. */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Kz Timer Elapsed Time by Handle", ScriptName = "GetKzTimerElapsedTimeHandle", WorldContext = "WorldContextObject"), Category = "KzLib|Timer")
	static float K2_GetTimerElapsedTimeHandle(const UObject* WorldContextObject, FTimerHandle Handle);

	/** Returns time until the Kz timer will next execute its delegate. -1 if no such timer. */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Kz Timer Remaining Time by Handle", ScriptName = "GetKzTimerRemainingTimeHandle", WorldContext = "WorldContextObject"), Category = "KzLib|Timer")
	static float K2_GetTimerRemainingTimeHandle(const UObject* WorldContextObject, FTimerHandle Handle);

	// --- By function name -----------------------------

	/** Clears a set Kz timer. Object defaults to self. */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Clear Kz Timer by Function Name", ScriptName = "ClearKzTimer", DefaultToSelf = "Object"), Category = "KzLib|Timer")
	static void K2_ClearTimer(UObject* Object, FString FunctionName);

	/** Pauses a set Kz timer at its current elapsed time. Object defaults to self. */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Pause Kz Timer by Function Name", ScriptName = "PauseKzTimer", DefaultToSelf = "Object"), Category = "KzLib|Timer")
	static void K2_PauseTimer(UObject* Object, FString FunctionName);

	/** Resumes a paused Kz timer from its current elapsed time. Object defaults to self. */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Unpause Kz Timer by Function Name", ScriptName = "UnPauseKzTimer", DefaultToSelf = "Object"), Category = "KzLib|Timer")
	static void K2_UnPauseTimer(UObject* Object, FString FunctionName);

	/** Returns true if a Kz timer exists and is active for the given delegate. Object defaults to self. */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Is Kz Timer Active by Function Name", ScriptName = "IsKzTimerActive", DefaultToSelf = "Object"), Category = "KzLib|Timer")
	static bool K2_IsTimerActive(UObject* Object, FString FunctionName);

	/** Returns true if a Kz timer exists and is paused for the given delegate. Object defaults to self. */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Is Kz Timer Paused by Function Name", ScriptName = "IsKzTimerPaused", DefaultToSelf = "Object"), Category = "KzLib|Timer")
	static bool K2_IsTimerPaused(UObject* Object, FString FunctionName);

	/** Returns true if a Kz timer for the given delegate exists. Object defaults to self. */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Does Kz Timer Exist by Function Name", ScriptName = "KzTimerExists", DefaultToSelf = "Object"), Category = "KzLib|Timer")
	static bool K2_TimerExists(UObject* Object, FString FunctionName);

	/** Returns elapsed time for the given delegate (time since current countdown iteration began). -1 if no such timer. Object defaults to self. */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Kz Timer Elapsed Time by Function Name", ScriptName = "GetKzTimerElapsedTime", DefaultToSelf = "Object"), Category = "KzLib|Timer")
	static float K2_GetTimerElapsedTime(UObject* Object, FString FunctionName);

	/** Returns time until the Kz timer will next execute its delegate. -1 if no such timer. Object defaults to self. */
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Kz Timer Remaining Time by Function Name", ScriptName = "GetKzTimerRemainingTime", DefaultToSelf = "Object"), Category = "KzLib|Timer")
	static float K2_GetTimerRemainingTime(UObject* Object, FString FunctionName);
};
