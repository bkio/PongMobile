// Copyright 2016 Burak Kara - BurakIO.com

using UnrealBuildTool;

public class Pong : ModuleRules
{
	public Pong(TargetInfo Target)
	{
		PublicDependencyModuleNames.AddRange(new string[]
            { "Core"
            , "CoreUObject"
            , "Engine"
            , "InputCore"
            });

		PrivateDependencyModuleNames.AddRange(new string[]
            {
            });
	}
}
