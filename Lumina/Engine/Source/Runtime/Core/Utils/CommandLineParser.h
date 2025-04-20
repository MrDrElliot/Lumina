#pragma once

#include "Containers/Array.h"
#include "Containers/Name.h"

namespace Lumina
{
    class FCommandLineParser
    {
    public:
        
        FCommandLineParser(int argc, char* argv[])
        {
            for (int i = 1; i < argc; ++i)
            {
                FString arg = argv[i];
                if (StringUtils::StartsWith(arg, "--"))
                {
                    FName key = arg.c_str();
                    FString value;

                    if (i + 1 < argc && StringUtils::StartsWith(FString(argv[i + 1]), "--") == false)
                    {
                        value = argv[++i];
                    }

                    Args[key] = value;
                }
                else
                {
                    PositionalArgs.push_back(arg);
                }
            }
        }

        bool Has(const FName& name) const
        {
            return Args.find(name) != Args.end();
        }

        FString Get(const FName& name, const FString& DefaultValue = "") const
        {
            auto it = Args.find(name);
            return it != Args.end() ? it->second : DefaultValue;
        }

        const TVector<FString>& GetPositionalArgs() const
        {
            return PositionalArgs;
        }

    private:
    
        THashMap<FName, FString>    Args;
        TVector<FString>            PositionalArgs;
    };
}
