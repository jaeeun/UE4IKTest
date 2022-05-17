// Some copyright should be here...

using UnrealBuildTool;
using Path = System.IO.Path;

public class MollisenHAND : ModuleRules
{
    public MollisenHAND(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(new string[] {
			// ... add public include paths required here ...
		});
		PrivateIncludePaths.AddRange(new string[] {
			// ... add other private include paths required here ...
		});
		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			// ... add other public dependencies that you statically link with here ...
		});
		PrivateDependencyModuleNames.AddRange(new string[] {
			"CoreUObject",
			"Engine",
			"Slate",
			"SlateCore",
			// ... add private dependencies that you statically link with here ...	
            "Projects"
        });
		DynamicallyLoadedModuleNames.AddRange(new string[] {
			// ... add any modules that your module loads dynamically here ...
		});
        PublicSystemIncludePaths.Add(MollisenAPIPathInclude);
        PublicLibraryPaths.Add(MollisenAPIPathLib(Target));
        PublicDelayLoadDLLs.Add(MollisenAPINameDLL(Target));
        RuntimeDependencies.Add(new RuntimeDependency(Path.Combine(MollisenAPIPathLib(Target), MollisenAPINameDLL(Target))));

        if (Target.Platform == UnrealTargetPlatform.Win64) {
            PublicAdditionalLibraries.Add(MollisenAPINameLib(Target));
        }

        if (Target.Platform == UnrealTargetPlatform.Android) {
            System.Console.WriteLine("Module Path: " + ModuleDirectory);
            AdditionalPropertiesForReceipt.Add(new ReceiptProperty("AndroidPlugin", Path.Combine(ModuleDirectory, "Android_UPL.xml")));
            PublicAdditionalLibraries.Add(Path.Combine(MollisenAPIPathLib(Target), MollisenAPINameLib(Target)));
        }

        System.Console.WriteLine("Include Path: {0}", MollisenAPIPathInclude);
        System.Console.WriteLine("Library Path: {0}", MollisenAPIPathLib(Target));
        System.Console.WriteLine("LibFile Path: {0}", Path.Combine(MollisenAPIPathLib(Target), MollisenAPINameLib(Target)));
    }

    private string MollisenAPIPath 
    {
        get { return Path.GetFullPath(Path.Combine(ModuleDirectory, "../../ThirdParty/MollisenAPI")); }
    }

    private string MollisenAPIPathInclude
    {
        get { return Path.GetFullPath(Path.Combine(MollisenAPIPath, "include")); }
    }

    private string MollisenAPIPathLib(ReadOnlyTargetRules Target)
    {
        if (Target.Platform == UnrealTargetPlatform.Win64)
            return Path.GetFullPath(Path.Combine(MollisenAPIPath, "lib/x86_64"));
        if (Target.Platform == UnrealTargetPlatform.Android)
            return Path.GetFullPath(Path.Combine(MollisenAPIPath, "lib/android"));
        return Path.GetFullPath(Path.Combine(MollisenAPIPath, "lib"));
    }

    private string MollisenAPINameLib(ReadOnlyTargetRules Target)
    {
        if (Target.Platform == UnrealTargetPlatform.Win64)
            return "ftsame.api.lib";
        if (Target.Platform == UnrealTargetPlatform.Android)
            return "libFtsameAPI.so";
        return "";
    }

    private string MollisenAPINameDLL(ReadOnlyTargetRules Target)
    {
        if (Target.Platform == UnrealTargetPlatform.Win64)
            return "ftsame.api.dll";
        if (Target.Platform == UnrealTargetPlatform.Android)
            return "libFtsameAPI.so";
        return "";
    }
}
