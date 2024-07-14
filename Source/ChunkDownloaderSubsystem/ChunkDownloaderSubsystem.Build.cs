// Copyright 2024 Cajun Pro LLC. All Rights Reserved.

using System.Diagnostics.CodeAnalysis;
using UnrealBuildTool;

// ReSharper disable once UnusedMember.Global
[SuppressMessage("Design", "CA1050:Declare types in namespaces")]
public class ChunkDownloaderSubsystem : ModuleRules
{
	public ChunkDownloaderSubsystem(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicDependencyModuleNames.AddRange(new[] { "Core" });
		PrivateDependencyModuleNames.AddRange(
			new[] { "CoreUObject", "Engine", "Slate", "SlateCore", "ChunkDownloader" });
	}
}