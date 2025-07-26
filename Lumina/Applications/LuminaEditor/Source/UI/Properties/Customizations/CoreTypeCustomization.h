#pragma once
#include "Core/Reflection/PropertyCustomization/PropertyCustomization.h"


namespace Lumina
{
    class FNumericPropertyCustomization : public IPropertyTypeCustomization
    {
    public:

        static TSharedPtr<FNumericPropertyCustomization> MakeInstance()
        {
            return MakeSharedPtr<FNumericPropertyCustomization>();
        }
        
        void DrawProperty(TSharedPtr<FPropertyHandle> Property) override;
    };

    class FBoolPropertyCustomization : public IPropertyTypeCustomization
    {
    public:

        static TSharedPtr<FBoolPropertyCustomization> MakeInstance()
        {
            return MakeSharedPtr<FBoolPropertyCustomization>();
        }
        
        void DrawProperty(TSharedPtr<FPropertyHandle> Property) override;
    };
    
    class FCObjectPropertyCustomization : public IPropertyTypeCustomization
    {
    public:

        static TSharedPtr<FCObjectPropertyCustomization> MakeInstance()
        {
            return MakeSharedPtr<FCObjectPropertyCustomization>();
        }
        
        void DrawProperty(TSharedPtr<FPropertyHandle> Property) override;
    };

    class FEnumPropertyCustomization : public IPropertyTypeCustomization
    {
    public:
        
        static TSharedPtr<FEnumPropertyCustomization> MakeInstance()
        {
            return MakeSharedPtr<FEnumPropertyCustomization>();
        }

        void DrawProperty(TSharedPtr<FPropertyHandle> Property) override;
    };

    class FNamePropertyCustomization : public IPropertyTypeCustomization
    {
    public:
        
        static TSharedPtr<FNamePropertyCustomization> MakeInstance()
        {
            return MakeSharedPtr<FNamePropertyCustomization>();
        }

        void DrawProperty(TSharedPtr<FPropertyHandle> Property) override;
    };

    class FVec2PropertyCustomization : public IPropertyTypeCustomization
    {
    public:
        
        static TSharedPtr<FVec2PropertyCustomization> MakeInstance()
        {
            return MakeSharedPtr<FVec2PropertyCustomization>();
        }

        void DrawProperty(TSharedPtr<FPropertyHandle> Property) override;
    };

    class FVec3PropertyCustomization : public IPropertyTypeCustomization
    {
    public:
        
        static TSharedPtr<FVec3PropertyCustomization> MakeInstance()
        {
            return MakeSharedPtr<FVec3PropertyCustomization>();
        }

        void DrawProperty(TSharedPtr<FPropertyHandle> Property) override;
    };

    class FVec4PropertyCustomization : public IPropertyTypeCustomization
    {
    public:
        
        static TSharedPtr<FVec4PropertyCustomization> MakeInstance()
        {
            return MakeSharedPtr<FVec4PropertyCustomization>();
        }

        void DrawProperty(TSharedPtr<FPropertyHandle> Property) override;
    };

    class FTransformPropertyCustomization : public IPropertyTypeCustomization
    {
    public:
        
        static TSharedPtr<FTransformPropertyCustomization> MakeInstance()
        {
            return MakeSharedPtr<FTransformPropertyCustomization>();
        }

        void DrawProperty(TSharedPtr<FPropertyHandle> Property) override;
    };
    
}
