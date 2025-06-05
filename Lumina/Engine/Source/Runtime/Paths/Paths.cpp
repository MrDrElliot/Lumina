#include "Paths.h"

namespace Lumina::Paths
{
        FString ResolveVirtualPath(const FString& VirtualPath)
        {
            if (StringUtils::StartsWith(VirtualPath, "project://"))
            {
                FString PathWithoutPrefix = VirtualPath.substr(10);
            
                return FProject::Get()->GetProjectContentDirectory() + "/" + PathWithoutPrefix;
            }
        
            return VirtualPath;
        }

        FString ConvertToVirtualPath(const FString& AbsolutePath)
        {
            FString VirtualPath = FString();
            FString ProjectDir = FProject::Get()->GetProjectContentDirectory();
    
            FString NormalizedAbsolutePath = AbsolutePath;
            StringUtils::ReplaceAllOccurrencesInPlace(NormalizedAbsolutePath, "\\", "/");
        

            if (StringUtils::StartsWith(NormalizedAbsolutePath, ProjectDir.c_str()))
            {
                FString AdditionalString = NormalizedAbsolutePath.substr(ProjectDir.length());

                if (StringUtils::StartsWith(AdditionalString, "/"))
                {
                    AdditionalString.erase(0, 1);
                }
            
                VirtualPath = FString("project://") + AdditionalString;
            }
        
            return Paths::RemoveExtension(VirtualPath);
        }
}
