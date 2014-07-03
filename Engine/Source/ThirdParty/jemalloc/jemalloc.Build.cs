// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class jemalloc : ModuleRules
{
	public jemalloc(TargetInfo Target)
	{
		Type = ModuleType.External;

        if (Target.Platform == UnrealTargetPlatform.Linux)
        {
		    // includes may differ depending on target platform
		    PublicIncludePaths.Add(UEBuildConfiguration.UEThirdPartySourceDirectory + "jemalloc/include/Linux/" + Target.Architecture);
            if (Target.IsMonolithic)
            {
                PublicAdditionalLibraries.Add(UEBuildConfiguration.UEThirdPartySourceDirectory + "jemalloc/lib/Linux/" + Target.Architecture + "/libjemalloc.a");
            }
            else
            {
                PublicLibraryPaths.Add(UEBuildConfiguration.UEThirdPartySourceDirectory + "jemalloc/lib/Linux/" + Target.Architecture);
                PublicAdditionalLibraries.Add("jemalloc");
            }
		    Definitions.Add("PLATFORM_SUPPORTS_JEMALLOC=1");
        }
	}
}
