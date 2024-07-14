// Copyright 2024 Cajun Pro LLC. All Rights Reserved.

#include "ChunkDownloaderSubsystem.h"

#include "ChunkDownloader.h"
#include "ChunkDownloaderProgress.h"
#include "ChunkDownloaderSubsystemRuntimeSettings.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogChunkDownloaderSubsystem)


void UChunkDownloaderSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	Settings = GetDefault<UChunkDownloaderSubsystemRuntimeSettings>();
	
	// initialize the chunk downloader with chosen platform
	TSharedRef<FChunkDownloader> Downloader = FChunkDownloader::GetOrCreate();
	Downloader->Initialize(UGameplayStatics::GetPlatformName(), 8);

	// load the cached build ID
	Downloader->LoadCachedBuild(Settings->DeploymentName);

	// update the build manifest file
	TFunction<void(bool bSuccess)> UpdateCompleteCallback = [&](bool bSuccess) { bIsDownloadManifestUpToDate = true; };
	Downloader->UpdateBuild(Settings->DeploymentName, Settings->ContentBuildId, UpdateCompleteCallback);
}

FChunkDownloaderProgress UChunkDownloaderSubsystem::GetLoadingProgress()
{
	// Get a reference to ChunkDownloader
	TSharedRef<FChunkDownloader> Downloader = FChunkDownloader::GetChecked();

	// Get the loading stats struct
	FChunkDownloader::FStats LoadingStats = Downloader->GetLoadingStats();

	FChunkDownloaderProgress Progress = {
		// Get the bytes downloaded and bytes to download
		static_cast<int32>(LoadingStats.BytesDownloaded),
		static_cast<int32>(LoadingStats.TotalBytesToDownload),
		0,
		// Get the number of chunks mounted and chunks to download
		LoadingStats.ChunksMounted,
		LoadingStats.TotalChunksToMount,
	};
	
	// Calculate the download and mount percent using the above stats
	Progress.DownloadPercent = static_cast<float>(Progress.BytesDownloaded) / static_cast<float>(Progress.TotalBytesToDownload) * 100.0f;
	Progress.MountPercent = static_cast<float>(Progress.ChunksMounted) / static_cast<float>(Progress.TotalChunksToMount) * 100.0f;

	return Progress;
}

bool UChunkDownloaderSubsystem::Patch()
{
	// make sure the download manifest is up-to-date
	if (bIsDownloadManifestUpToDate)
	{
		// get the chunk downloader
		TSharedRef<FChunkDownloader> Downloader = FChunkDownloader::GetChecked();

		// report current chunk status
		for (int32 ChunkID : ChunkDownloadList)
		{
			int32 ChunkStatus = static_cast<int32>(Downloader->GetChunkStatus(ChunkID));
			UE_LOG(LogChunkDownloaderSubsystem, Display, TEXT("PatchGame::Chunk %i status: %i"), ChunkID, ChunkStatus);
		}

		// ReSharper disable once CppParameterMayBeConst
		TFunction<void (bool bSuccess)> DownloadCompleteCallback = [&](bool bSuccess) { OnDownloadComplete(bSuccess); };
		Downloader->DownloadChunks(ChunkDownloadList, DownloadCompleteCallback, 1);

		// start loading mode
		// ReSharper disable once CppParameterMayBeConst
		TFunction<void (bool bSuccess)> LoadingModeCompleteCallback = [&](bool bSuccess) { OnLoadingModeComplete(bSuccess); };
		Downloader->BeginLoadingMode(LoadingModeCompleteCallback);
		return true;
	}

	// you couldn't contact the server to validate your Manifest, so you can't patch
	UE_LOG(LogChunkDownloaderSubsystem, Error, TEXT("PatchGame::Manifest Update Failed. Can't patch the game"));

	return false;
}

void UChunkDownloaderSubsystem::OnLoadingModeComplete(const bool bSuccess)
{
	OnDownloadComplete(bSuccess);
}

void UChunkDownloaderSubsystem::OnMountComplete(const bool bSuccess)
{
	OnPatchComplete.Broadcast(bSuccess);
}

void UChunkDownloaderSubsystem::OnManifestUpdateComplete(const bool bSuccess)
{
	bIsDownloadManifestUpToDate = bSuccess;
}

void UChunkDownloaderSubsystem::OnDownloadComplete(const bool bSuccess)
{
	if (bSuccess)
	{
		UE_LOG(LogTemp, Display, TEXT("Download complete"));

		// get the chunk downloader
		TSharedRef<FChunkDownloader> Downloader = FChunkDownloader::GetChecked();
		FJsonSerializableArrayInt DownloadedChunks;

		for (int32 ChunkID : ChunkDownloadList)
		{
			DownloadedChunks.Add(ChunkID);
		}

		//Mount the chunks
		TFunction<void(bool Success)> MountCompleteCallback = [&](const bool Success) { OnMountComplete(Success); };
		Downloader->MountChunks(DownloadedChunks, MountCompleteCallback);

		OnPatchComplete.Broadcast(true);
	}
	else
	{
		UE_LOG(LogChunkDownloaderSubsystem, Display, TEXT("OnDownloadComplete::Load process failed"));

		// call the delegate
		OnPatchComplete.Broadcast(false);
	}
}