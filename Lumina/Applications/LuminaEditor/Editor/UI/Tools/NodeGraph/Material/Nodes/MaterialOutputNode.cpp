#include "MaterialOutputNode.h"

#include "UI/Tools/NodeGraph/Material/Pins/MaterialExpressions.h"

namespace Lumina
{
    FString FMaterialOutputNode::GetNodeDisplayName() const
    {
        return "Material Output";
    }
    
    FString FMaterialOutputNode::GetNodeTooltip() const
    {
        return "The final output to the shader";
    }


    void FMaterialOutputNode::BuildNode()
    {
        // Base Color (Albedo)
        CreatePin<FMaterialNodePin_Float4, ENodePinDirection::Input>("Base Color (RGBA)", this, true, EMaterialParamPinFlags::None);
    
        // Metallic (Determines if the material is metal or non-metal)
        CreatePin<FMaterialNodePin_Scalar, ENodePinDirection::Input>("Metallic", this, true, EMaterialParamPinFlags::None);

        // Roughness (Controls how smooth or rough the surface is)
        CreatePin<FMaterialNodePin_Scalar, ENodePinDirection::Input>("Roughness", this, true, EMaterialParamPinFlags::None);

        // Specular (Affects intensity of reflections for non-metals)
        CreatePin<FMaterialNodePin_Scalar, ENodePinDirection::Input>("Specular", this, true, EMaterialParamPinFlags::None);

        // Emissive (Self-illumination, for glowing objects)
        CreatePin<FMaterialNodePin_Float3, ENodePinDirection::Input>("Emissive (RGB)", this, true, EMaterialParamPinFlags::None);

        // Ambient Occlusion (Shadows in crevices to add realism)
        CreatePin<FMaterialNodePin_Scalar, ENodePinDirection::Input>("Ambient Occlusion", this, true, EMaterialParamPinFlags::None);

        // Normal Map (For surface detail)
        CreatePin<FMaterialNodePin_Float3, ENodePinDirection::Input>("Normal Map (XYZ)", this, true, EMaterialParamPinFlags::None);

        // Opacity (For transparent materials)
        CreatePin<FMaterialNodePin_Scalar, ENodePinDirection::Input>("Opacity", this, true, EMaterialParamPinFlags::None);

        
    }

    FString FMaterialOutputNode::Evaluate(FMaterialCompiler* Compiler)
    {
        FEdNodeGraphPin* BaseColorPin = GetPinByIndex(0, ENodePinDirection::Input);
        FEdNodeGraphPin* MetallicPin = GetPinByIndex(1, ENodePinDirection::Input);
        FEdNodeGraphPin* RoughnessPin = GetPinByIndex(2, ENodePinDirection::Input);
        FEdNodeGraphPin* SpecularPin = GetPinByIndex(3, ENodePinDirection::Input);
        FEdNodeGraphPin* EmissivePin = GetPinByIndex(4, ENodePinDirection::Input);
        FEdNodeGraphPin* AOPin = GetPinByIndex(5, ENodePinDirection::Input);
        FEdNodeGraphPin* NormalPin = GetPinByIndex(6, ENodePinDirection::Input);
        FEdNodeGraphPin* OpacityPin = GetPinByIndex(7, ENodePinDirection::Input);
    
        std::stringstream SS;
    
        SS << "\n\tMaterialInputs Input = (MaterialInputs)0;\n";
    
        // Base Color
        if (BaseColorPin->HasConnection())
        {
            SS << "\tInput.BaseColor = " << BaseColorPin->GetConnections()[0]->GetOwningNode<FMaterialGraphNode>()->Evaluate(Compiler).c_str() << ";\n";
        }
        else
        {
            SS << "\tInput.BaseColor = vec3(0.8f, 0.8f, 0.8f);\n";
        }
    
        // Metallic
        if (MetallicPin->HasConnection())
        {
            SS << "\tInput.Metallic = " << MetallicPin->GetConnections()[0]->GetOwningNode<FMaterialGraphNode>()->Evaluate(Compiler).c_str() << ";\n";
        }
        else
        {
            SS << "\tInput.Metallic = 0.0;\n";
        }
    
        // Roughness
        if (RoughnessPin->HasConnection())
        {
            SS << "\tInput.Roughness = " << RoughnessPin->GetConnections()[0]->GetOwningNode<FMaterialGraphNode>()->Evaluate(Compiler).c_str() << ";\n";
        }
        else
        {
            SS << "\tInput.Roughness = 1.0;\n";
        }
    
        // Specular
        if (SpecularPin->HasConnection())
        {
            SS << "\tInput.Specular = " << SpecularPin->GetConnections()[0]->GetOwningNode<FMaterialGraphNode>()->Evaluate(Compiler).c_str() << ";\n";
        }
        else
        {
            SS << "\tInput.Specular = 0.5;\n"; // Default moderate specular reflection
        }
    
        // Emissive
        if (EmissivePin->HasConnection())
        {
            SS << "\tInput.Emissive = " << EmissivePin->GetConnections()[0]->GetOwningNode<FMaterialGraphNode>()->Evaluate(Compiler).c_str() << ";\n";
        }
        else
        {
            SS << "\tInput.Emissive = vec3(0.0, 0.0, 0.0);\n"; // No emission by default
        }
    
        // Ambient Occlusion
        if (AOPin->HasConnection())
        {
            SS << "\tInput.AmbientOcclusion = " << AOPin->GetConnections()[0]->GetOwningNode<FMaterialGraphNode>()->Evaluate(Compiler).c_str() << ";\n";
        }
        else
        {
            SS << "\tInput.AmbientOcclusion = 1.0;\n"; // Default full ambient occlusion
        }
    
        // Normal Map
        if (NormalPin->HasConnection())
        {
            SS << "\tInput.Normal = " << NormalPin->GetConnections()[0]->GetOwningNode<FMaterialGraphNode>()->Evaluate(Compiler).c_str() << ";\n";
        }
        else
        {
            SS << "\tInput.Normal = vec3(0.0, 0.0, 1.0);\n"; // Default flat normal map
        }
    
        // Opacity
        if (OpacityPin->HasConnection())
        {
            SS << "\tInput.Opacity = " << OpacityPin->GetConnections()[0]->GetOwningNode<FMaterialGraphNode>()->Evaluate(Compiler).c_str() << ";\n";
        }
        else
        {
            SS << "\tInput.Opacity = 1.0;\n"; // Fully opaque by default
        }
    
        return FString(SS.str().c_str());
    }

}
