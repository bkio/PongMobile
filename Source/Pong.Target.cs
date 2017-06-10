// Copyright 2016 Burak Kara - BurakIO.com

using UnrealBuildTool;
using System.Collections.Generic;

public class PongTarget : TargetRules
{
	public PongTarget(TargetInfo Target)
	{
		Type = TargetType.Game;
	}

	//
	// TargetRules interface.
	//

	public override void SetupBinaries(
		TargetInfo Target,
		ref List<UEBuildBinaryConfiguration> OutBuildBinaryConfigurations,
		ref List<string> OutExtraModuleNames
		)
	{
		OutExtraModuleNames.AddRange( new string[] { "Pong" } );
	}

    public override void SetupGlobalEnvironment(
     TargetInfo Target,
     ref LinkEnvironmentConfiguration OutLinkEnvironmentConfiguration,
     ref CPPEnvironmentConfiguration OutCPPEnvironmentConfiguration
     )
    {
        UEBuildConfiguration.bCompileRecast = false;
        UEBuildConfiguration.bCompileSpeedTree = false;
        UEBuildConfiguration.bCompileAPEX = false;
        UEBuildConfiguration.bCompileLeanAndMeanUE = true;
        UEBuildConfiguration.bCompilePhysXVehicle = false;
        UEBuildConfiguration.bCompileFreeType = false;
        UEBuildConfiguration.bCompileForSize = true;
    }
}
