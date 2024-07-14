// Copyright 2024 Cajun Pro LLC. All Rights Reserved.

#pragma once

#include "ChunkDownloaderProgress.generated.h"

USTRUCT(BlueprintType, meta=(DisplayName="Progress"))
struct CHUNKDOWNLOADERSUBSYSTEM_API FChunkDownloaderProgress
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	int32 BytesDownloaded = 0;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	int32 TotalBytesToDownload = 0;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	float DownloadPercent = 0.0f;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	int32 ChunksMounted = 0;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	int32 TotalChunksToMount = 0;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	float MountPercent = 0.0f;
};
