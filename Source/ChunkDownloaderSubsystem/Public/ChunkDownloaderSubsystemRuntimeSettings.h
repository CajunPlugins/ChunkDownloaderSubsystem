// Copyright 2024 Cajun Pro LLC. All Rights Reserved.

#pragma once

#include "Engine/DeveloperSettings.h"
#include "ChunkDownloaderSubsystemRuntimeSettings.generated.h"

UCLASS(Config=Game, DefaultConfig, meta=(DisplayName="Chunk Downloader Subsystem"))
class CHUNKDOWNLOADERSUBSYSTEM_API UChunkDownloaderSubsystemRuntimeSettings : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category=Config)
	FString DeploymentName = "MyGame";

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category=Config)
	FString ContentBuildId = "Patching";
};
