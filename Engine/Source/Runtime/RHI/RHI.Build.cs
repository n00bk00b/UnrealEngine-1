// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System;

public class RHI : ModuleRules
{
	public RHI(TargetInfo Target)
	{
		PrivateDependencyModuleNames.Add("Core");

		if (UEBuildConfiguration.bCompileAgainstEngine)
		{
            DynamicallyLoadedModuleNames.Add("NullDrv");

			// UEBuildAndroid.cs adds VulkanRHI for Android builds if it is enabled
			if ((Target.Platform == UnrealTargetPlatform.Win32) || (Target.Platform == UnrealTargetPlatform.Win64))
			{
				DynamicallyLoadedModuleNames.Add("D3D11RHI");

				//#todo-rco: D3D12 requires different SDK headers not compatible with WinXP
				DynamicallyLoadedModuleNames.Add("D3D12RHI");
            }

			if (Target.Platform == UnrealTargetPlatform.Win64)
			{
				string VulkanSDKPath = Environment.GetEnvironmentVariable("VULKAN_SDK");
				if (String.IsNullOrEmpty(VulkanSDKPath))
				{
					VulkanSDKPath = Environment.GetEnvironmentVariable("VK_SDK_PATH");
				}
				if (!String.IsNullOrEmpty(VulkanSDKPath))
				{
					DynamicallyLoadedModuleNames.Add("VulkanRHI");
				}
			}

			if ((Target.Platform == UnrealTargetPlatform.Win32) ||
				(Target.Platform == UnrealTargetPlatform.Win64) ||
				(Target.Platform == UnrealTargetPlatform.Mac)   ||
                (Target.Platform == UnrealTargetPlatform.Linux && Target.Type != TargetRules.TargetType.Server) ||  // @todo should servers on all platforms skip this?
                (Target.Platform == UnrealTargetPlatform.HTML5))
			{
				DynamicallyLoadedModuleNames.Add("OpenGLDrv");
			}
        }

		if (Target.Configuration != UnrealTargetConfiguration.Shipping)
		{
			PrivateIncludePathModuleNames.AddRange(new string[] { "TaskGraph" });
		}
	}
}
