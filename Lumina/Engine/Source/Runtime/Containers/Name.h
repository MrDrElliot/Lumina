#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <functional>

#include "Platform/GenericPlatform.h"
//@TODO
    class LString;

    class LNameEntry
    {
    public:
        
        uint64 Hash;    // The hashed value of the name.
        std::string Name; // The actual name string.

        LNameEntry(const std::string& InName)
            : Name(InName)
        {
            Hash = std::hash<std::string>{}(Name); // Generate hash for the string.
        }
    };

    class LName
    {
    public:
        // A static map to store unique names (name table).
        static std::unordered_map<uint64_t, LNameEntry*> NameTable;

        uint64_t Hash; // Hash of the name, used for fast comparison.
        LNameEntry* Entry; // Pointer to the corresponding name entry in the table.

        // Constructor that creates or finds an entry in the global name table.
        LName(const std::string& InName)
        {
            // Hash the name and check if it already exists in the table.
            Hash = std::hash<std::string>{}(InName);
        
            // Look for the hash in the global name table.
            auto it = NameTable.find(Hash);
            if (it != NameTable.end())
            {
                // If the name already exists, reuse the entry.
                Entry = it->second;
            }
            else
            {
                // Otherwise, create a new name entry and insert it into the table.
                Entry = new LNameEntry(InName);
                NameTable[Hash] = Entry;
            }
        }

        // Comparison operators
        bool operator==(const LName& Other) const
        {
            return Hash == Other.Hash;
        }

        bool operator!=(const LName& Other) const
        {
            return Hash != Other.Hash;
        }

        bool operator<(const LName& Other) const
        {
            return Hash < Other.Hash;
        }

        // Get the name as a string.
        const std::string& GetName() const
        {
            return Entry->Name;
        }

        // Destructor
        ~LName()
        {
            // Optionally clean up the name table or manage reference counting
        }
    };

    // Initialize the static NameTable
    std::unordered_map<uint64_t, LNameEntry*> LName::NameTable;

