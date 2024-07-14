// Copyright 2024 Cajun Pro LLC. All Rights Reserved.

#pragma once

#include "Subsystems/GameInstanceSubsystem.h"
#include "ChunkDownloaderSubsystem.generated.h"

class UChunkDownloaderSubsystemRuntimeSettings;
struct FChunkDownloaderProgress;

DECLARE_LOG_CATEGORY_EXTERN(LogChunkDownloaderSubsystem, Log, All)

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSuccessDelegate, bool, Successful);

/**
 * 
 */
UCLASS()
class CHUNKDOWNLOADERSUBSYSTEM_API UChunkDownloaderSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// Overrides
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	// Gets the current progress of the patch
	UFUNCTION(BlueprintPure, Category=ChunkDownloader)
	static FChunkDownloaderProgress GetLoadingProgress();

	// Starts the game patching process. Returns false if the patching manifest is not up-to-date. */
	UFUNCTION(BlueprintCallable, Category=ChunkDownloader)
	bool Patch();

private:
	// Called when the manifest update has returned
	void OnManifestUpdateComplete(const bool bSuccess);
	// Called when the chunk download process finishes
	void OnDownloadComplete(const bool bSuccess);
	// Called whenever ChunkDownloader's loading mode is finished
	void OnLoadingModeComplete(const bool bSuccess);
	// Called when ChunkDownloader finishes mounting chunks
	void OnMountComplete(const bool bSuccess);

public:
	// List of Chunk IDs to try and download
	UPROPERTY(EditDefaultsOnly, Category=Patching)
	TArray<int32> ChunkDownloadList;

	// Fired when the patching process succeeds or fails
	UPROPERTY(BlueprintAssignable, Category=ChunkDownloader)
	FSuccessDelegate OnPatchComplete;

private:
	// Tracks if our local manifest file is up-to-date with the one hosted on our website
	bool bIsDownloadManifestUpToDate = false;

	const UChunkDownloaderSubsystemRuntimeSettings* Settings;
};