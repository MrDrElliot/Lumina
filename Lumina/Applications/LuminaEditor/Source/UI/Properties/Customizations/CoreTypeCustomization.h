#pragma once
#include "Core/Reflection/PropertyCustomization/PropertyCustomization.h"
#include "imgui.h"
#include "Core/Math/Transform.h"
#include "glm/glm.hpp"

namespace Lumina
{
    template<typename T, ImGuiDataType_ DT>
    class FNumericPropertyCustomization : public IPropertyTypeCustomization
    {
        using ValueType = T;
        
    public:
        
        static TSharedPtr<FNumericPropertyCustomization> MakeInstance()
        {
            return MakeSharedPtr<FNumericPropertyCustomization>();
        }
        
        EPropertyChangeOp DrawProperty(TSharedPtr<FPropertyHandle> Property) override
        {
            ImGui::DragScalar("##Value", DT, &DisplayValue);

            return ImGui::IsItemDeactivatedAfterEdit() ? EPropertyChangeOp::Updated : EPropertyChangeOp::None;
        }
        
        void UpdatePropertyValue(TSharedPtr<FPropertyHandle> Property) override
        {
            CachedValue = DisplayValue;
            *(ValueType*)Property->PropertyPointer = CachedValue;
        }

        void HandleExternalUpdate(TSharedPtr<FPropertyHandle> Property) override
        {
            const ValueType ActualValue = *(ValueType*)Property->PropertyPointer;
            if (ActualValue != CachedValue)
            {
                CachedValue = DisplayValue = ActualValue;
            }
        }

        ValueType CachedValue;
        ValueType DisplayValue;
        
    };
    
    class FBoolPropertyCustomization : public IPropertyTypeCustomization
    {
    public:

        static TSharedPtr<FBoolPropertyCustomization> MakeInstance()
        {
            return MakeSharedPtr<FBoolPropertyCustomization>();
        }
        
        EPropertyChangeOp DrawProperty(TSharedPtr<FPropertyHandle> Property) override
        {
            ImGui::Checkbox("##", &bValue);

            return ImGui::IsItemDeactivatedAfterEdit() ? EPropertyChangeOp::Updated : EPropertyChangeOp::None;
        }
        
        void UpdatePropertyValue(TSharedPtr<FPropertyHandle> Property) override
        {
            *(bool*)Property->PropertyPointer = bValue;
        }

        void HandleExternalUpdate(TSharedPtr<FPropertyHandle> Property) override
        {
            bValue = *(bool*)Property->PropertyPointer;
        }

        bool bValue;
    };
    
    class FCObjectPropertyCustomization : public IPropertyTypeCustomization
    {
    public:

        static TSharedPtr<FCObjectPropertyCustomization> MakeInstance()
        {
            return MakeSharedPtr<FCObjectPropertyCustomization>();
        }
        
        EPropertyChangeOp DrawProperty(TSharedPtr<FPropertyHandle> Property) override;
        
        void UpdatePropertyValue(TSharedPtr<FPropertyHandle> Property) override;

        void HandleExternalUpdate(TSharedPtr<FPropertyHandle> Property) override;

    private:

        FObjectHandle ObjectHandle;
    };

    class FEnumPropertyCustomization : public IPropertyTypeCustomization
    {
    public:
        
        static TSharedPtr<FEnumPropertyCustomization> MakeInstance()
        {
            return MakeSharedPtr<FEnumPropertyCustomization>();
        }

        EPropertyChangeOp DrawProperty(TSharedPtr<FPropertyHandle> Property) override;
        void UpdatePropertyValue(TSharedPtr<FPropertyHandle> Property) override;
        void HandleExternalUpdate(TSharedPtr<FPropertyHandle> Property) override;

    private:

        int64 CachedValue = 0;
    };

    class FNamePropertyCustomization : public IPropertyTypeCustomization
    {
    public:
        
        static TSharedPtr<FNamePropertyCustomization> MakeInstance()
        {
            return MakeSharedPtr<FNamePropertyCustomization>();
        }

        EPropertyChangeOp DrawProperty(TSharedPtr<FPropertyHandle> Property) override;
        void UpdatePropertyValue(TSharedPtr<FPropertyHandle> Property) override;
        void HandleExternalUpdate(TSharedPtr<FPropertyHandle> Property) override;
        
    private:

        FName CachedValue;
        FName DisplayValue;
    };

    class FStringPropertyCustomization : public IPropertyTypeCustomization
    {
    public:
        
        static TSharedPtr<FStringPropertyCustomization> MakeInstance()
        {
            return MakeSharedPtr<FStringPropertyCustomization>();
        }

        EPropertyChangeOp DrawProperty(TSharedPtr<FPropertyHandle> Property) override;
        void UpdatePropertyValue(TSharedPtr<FPropertyHandle> Property) override;
        void HandleExternalUpdate(TSharedPtr<FPropertyHandle> Property) override;

    private:

        FString CachedValue;
        FString DisplayValue;
    };

    class FVec2PropertyCustomization : public IPropertyTypeCustomization
    {
    public:
        
        static TSharedPtr<FVec2PropertyCustomization> MakeInstance()
        {
            return MakeSharedPtr<FVec2PropertyCustomization>();
        }

        EPropertyChangeOp DrawProperty(TSharedPtr<FPropertyHandle> Property) override;
        void UpdatePropertyValue(TSharedPtr<FPropertyHandle> Property) override;
        void HandleExternalUpdate(TSharedPtr<FPropertyHandle> Property) override;

    private:

        glm::vec2 CachedValue;
        glm::vec2 DisplayValue;
    };

    class FVec3PropertyCustomization : public IPropertyTypeCustomization
    {
    public:
        
        static TSharedPtr<FVec3PropertyCustomization> MakeInstance()
        {
            return MakeSharedPtr<FVec3PropertyCustomization>();
        }

        EPropertyChangeOp DrawProperty(TSharedPtr<FPropertyHandle> Property) override;
        void UpdatePropertyValue(TSharedPtr<FPropertyHandle> Property) override;
        void HandleExternalUpdate(TSharedPtr<FPropertyHandle> Property) override;

    private:
        
        glm::vec3 CachedValue;
        glm::vec3 DisplayValue;
    };

    class FVec4PropertyCustomization : public IPropertyTypeCustomization
    {
    public:
        
        static TSharedPtr<FVec4PropertyCustomization> MakeInstance()
        {
            return MakeSharedPtr<FVec4PropertyCustomization>();
        }

        EPropertyChangeOp DrawProperty(TSharedPtr<FPropertyHandle> Property) override;
        void UpdatePropertyValue(TSharedPtr<FPropertyHandle> Property) override;
        void HandleExternalUpdate(TSharedPtr<FPropertyHandle> Property) override;

    private:

        glm::vec4 CachedValue;
        glm::vec4 DisplayValue;
    };

    class FTransformPropertyCustomization : public IPropertyTypeCustomization
    {
    public:
        
        static TSharedPtr<FTransformPropertyCustomization> MakeInstance()
        {
            return MakeSharedPtr<FTransformPropertyCustomization>();
        }

        EPropertyChangeOp DrawProperty(TSharedPtr<FPropertyHandle> Property) override;
        void UpdatePropertyValue(TSharedPtr<FPropertyHandle> Property) override;
        void HandleExternalUpdate(TSharedPtr<FPropertyHandle> Property) override;

        FTransform CachedValue;
        FTransform DisplayValue;
    };
    
}
