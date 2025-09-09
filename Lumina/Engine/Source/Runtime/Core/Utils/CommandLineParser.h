#pragma once

#include "Containers/Array.h"
#include "Containers/Name.h"
#include "Core/Templates/Optional.h"

namespace Lumina
{
    class FCommandLineParser
    {
    public:
    
        FCommandLineParser() = default;
    
        FCommandLineParser(int argc, char* argv[])
        {
            Parse(argc, argv);
        }
    
        void Parse(int argc, char* argv[])
        {
            for (int i = 1; i < argc; ++i)
            {
                FString arg = argv[i];
    
                if (StringUtils::StartsWith(arg, "--"))
                {
                    FString key = NormalizeKey(arg.substr(2));
                    FString value;
    
                    size_t eq = key.find('=');
                    if (eq != FString::npos)
                    {
                        value = key.substr(eq + 1);
                        key = key.substr(0, eq);
                    }
                    else if (i + 1 < argc && StringUtils::StartsWith(FString(argv[i + 1]), "--") == false)
                    {
                        value = argv[++i];
                    }
    
                    Args[key] = value;
                }
                else if (StringUtils::StartsWith(arg, "-") && arg.size() > 1)
                {
                    for (size_t j = 1; j < arg.size(); ++j)
                    {
                        FString key(1, arg[j]);
                        Args[NormalizeKey(key)] = "true";
                    }
                }
                else
                {
                    PositionalArgs.push_back(arg);
                }
            }
        }
    
        NODISCARD bool Has(const FString& name) const
        {
            return Args.find(NormalizeKey(name)) != Args.end();
        }
    
        NODISCARD TOptional<FString> Get(const FString& Name) const
        {
            auto it = Args.find(NormalizeKey(Name));
            return it != Args.end() ? it->second : TOptional<FString>();
        }
    
        NODISCARD TOptional<int> GetInt(const FString& name) const
        {
            auto it = Args.find(NormalizeKey(name));
            return it != Args.end() ? std::stoi(it->second.c_str()) : TOptional<int>();
        }
    
        NODISCARD TOptional<bool> GetBool(const FString& name) const
        {
            auto it = Args.find(NormalizeKey(name));
            if (it == Args.end())
            {
                return TOptional<bool>();
            }
    
            FString val = StringUtils::ToLower(it->second);
            return val.empty() || val == "1" || val == "true" || val == "yes";
        }
    
        const TVector<FString>& GetPositionalArgs() const
        {
            return PositionalArgs;
        }
    
    private:
    
        static FString NormalizeKey(const FString& raw)
        {
            return StringUtils::ToLower(raw);
        }
    
    private:
        THashMap<FName, FString> Args;
        TVector<FString> PositionalArgs;
    };

}
