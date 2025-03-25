#include "MaterialOutputNode.h"

#include "UI/Tools/NodeGraph/Material/MaterialCompiler.h"
#include "UI/Tools/NodeGraph/Material/MaterialInput.h"

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
        BaseColorPin = CreatePin<FMaterialInput, ENodePinDirection::Input>(EMaterialInputType::Float3);
        BaseColorPin->SetPinName("Base Color (RGBA)");
    
        // Metallic (Determines if the material is metal or non-metal)
        MetallicPin = CreatePin<FMaterialInput, ENodePinDirection::Input>(EMaterialInputType::Float);
        MetallicPin->SetPinName("Metallic");
        
        // Roughness (Controls how smooth or rough the surface is)
        RoughnessPin = CreatePin<FMaterialInput, ENodePinDirection::Input>(EMaterialInputType::Float);
        RoughnessPin->SetPinName("Roughness");

        // Specular (Affects intensity of reflections for non-metals)
        SpecularPin = CreatePin<FMaterialInput, ENodePinDirection::Input>(EMaterialInputType::Float);
        SpecularPin->SetPinName("Specular");

        // Emissive (Self-illumination, for glowing objects)
        EmissivePin = CreatePin<FMaterialInput, ENodePinDirection::Input>(EMaterialInputType::Float3);
        EmissivePin->SetPinName("Emissive (RGB)");

        // Ambient Occlusion (Shadows in crevices to add realism)
        AOPin = CreatePin<FMaterialInput, ENodePinDirection::Input>(EMaterialInputType::Float);
        AOPin->SetPinName("Ambient Occlusion");

        // Normal Map (For surface detail)
        NormalPin = CreatePin<FMaterialInput, ENodePinDirection::Input>(EMaterialInputType::Float3);
        NormalPin->SetPinName("Normal Map (XYZ)");
        
        // Opacity (For transparent materials)
        OpacityPin = CreatePin<FMaterialInput, ENodePinDirection::Input>(EMaterialInputType::Float);
        OpacityPin->SetPinName("Opacity");

        
    }

    void FMaterialOutputNode::GenerateDefinition(FMaterialCompiler* Compiler)
    {
        FString Output;
        Output += "\n \n";
    
        Output += "MaterialInput Input = (FMaterialInputs)0;\n";
    
        // Base Color
        Output += "Input.Diffuse = ";
        if (BaseColorPin->HasConnection())
        {
            Output += BaseColorPin->GetConnections()[0]->GetOwningNode()->GetNodeFullName() + ".rgb;";
        }
        else
        {
            Output += "vec3(1.0, 1.0, 1.0);";
        }
        Output += "\n";
    
        // Metallic
        Output += "Input.Metallic = ";
        if (MetallicPin->HasConnection())
        {
            Output += MetallicPin->GetConnections()[0]->GetOwningNode()->GetNodeFullName() + ".r;";
        }
        else
        {
            Output += "0.0;";
        }
        Output += "\n";
    
        // Roughness
        Output += "Input.Roughness = ";
        if (RoughnessPin->HasConnection())
        {
            Output += RoughnessPin->GetConnections()[0]->GetOwningNode()->GetNodeFullName() + ".r;";
        }
        else
        {
            Output += "1.0;";
        }
        Output += "\n";
    
        // Specular
        Output += "Input.Specular = ";
        if (SpecularPin->HasConnection())
        {
            Output += SpecularPin->GetConnections()[0]->GetOwningNode()->GetNodeFullName() + ".r;";
        }
        else
        {
            Output += "0.5;";
        }
        Output += "\n";
    
        // Emissive
        Output += "Input.Emissive = ";
        if (EmissivePin->HasConnection())
        {
            Output += EmissivePin->GetConnections()[0]->GetOwningNode()->GetNodeFullName() + ".rgb;";
        }
        else
        {
            Output += "vec3(0.0, 0.0, 0.0);";
        }
        Output += "\n";
    
        // Ambient Occlusion
        Output += "Input.AO = ";
        if (AOPin->HasConnection())
        {
            Output += AOPin->GetConnections()[0]->GetOwningNode()->GetNodeFullName() + ".r;";
        }
        else
        {
            Output += "1.0;";
        }
        Output += "\n";
    
        // Normal
        Output += "Input.Normal = ";
        if (NormalPin->HasConnection())
        {
            Output += "normalize(" + NormalPin->GetConnections()[0]->GetOwningNode()->GetNodeFullName() + ".xyz);";
        }
        else
        {
            Output += "vec3(0.0, 0.0, 1.0);";
        }
        Output += "\n";
    
        // Opacity
        Output += "Input.Opacity = ";
        if (OpacityPin->HasConnection())
        {
            Output += OpacityPin->GetConnections()[0]->GetOwningNode()->GetNodeFullName() + ".r;";
        }
        else
        {
            Output += "1.0;";
        }
        Output += "\n";
    
        Compiler->AddRaw(Output);
    }

}
