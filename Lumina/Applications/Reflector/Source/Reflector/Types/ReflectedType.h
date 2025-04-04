#pragma once
#include "Containers/Name.h"

namespace Lumina::Reflection
{
    /** Abstract base to all reflected types */
    class FReflectedType
    {
    public:
        
        virtual ~FReflectedType() = default;
        virtual FString GetTypeAsString() const = 0;
        
        FName       ID;
        FName       HeaderID;
        FString     DisplayName;
    };
    

    /** Reflected enumeration. */
    class FReflectedEnum : public FReflectedType
    {
    public:

        struct FConstant
        {
            FName ID;
            FString Label;
            FString Description;
            uint32 Value;
        };

        FString GetTypeAsString() const override;

        void AddConstant(const FConstant& Constant) { Constants.push_back(Constant); }


        TVector<FConstant> Constants;
        
        
    };

    
    /** Reflected structure */
    class FReflectedStruct : public FReflectedType
    {
    public:

        struct FField
        {
            FName ID;
            uint32 LineNumber;
            FString Name;
        };

        TVector<FField>     Fields;
        
    };

    
    /** Reflected class */
    class FReflectedClass : public FReflectedStruct
    {
    public:
        
    };
}
