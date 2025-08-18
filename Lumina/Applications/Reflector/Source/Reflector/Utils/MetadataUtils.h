#pragma once
#include "EASTL/string.h"
#include "EASTL/vector.h"

struct FMetadataPair
{
    eastl::string Key;
    eastl::string Value;
};

class FMetadataParser
{
public:

    FMetadataParser(const eastl::string& Raw)
    {
        Parse(Raw);
    }

    void Parse(const eastl::string& Raw);

    auto begin() { return Metadata.begin(); }
    auto end() { return Metadata.end(); }

    auto begin() const { return Metadata.begin(); }
    auto end() const { return Metadata.end(); }
    
    eastl::vector<FMetadataPair> Metadata;
    
};
