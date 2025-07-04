#include "MaterialOutputNode.h"

#include "UI/Tools/NodeGraph/Material/MaterialInput.h"
#include "UI/Tools/NodeGraph/Material/MaterialCompiler.h"

namespace Lumina
{
    FString CMaterialOutputNode::GetNodeDisplayName() const
    {
        return "Material Output";
    }
    
    FString CMaterialOutputNode::GetNodeTooltip() const
    {
        return "The final output to the shader";
    }


    void CMaterialOutputNode::BuildNode()
    {
        // Base Color (Albedo)
        BaseColorPin = CreatePin(CMaterialInput::StaticClass(), "Base Color", ENodePinDirection::Input, EMaterialInputType::Float3);
        BaseColorPin->SetPinName("Base Color (RGBA)");
    
        // Metallic (Determines if the material is metal or non-metal)
        MetallicPin = CreatePin(CMaterialInput::StaticClass(), "Metallic", ENodePinDirection::Input, EMaterialInputType::Float);
        MetallicPin->SetPinName("Metallic");
        
        // Roughness (Controls how smooth or rough the surface is)
        RoughnessPin = CreatePin(CMaterialInput::StaticClass(), "Roughness", ENodePinDirection::Input, EMaterialInputType::Float);
        RoughnessPin->SetPinName("Roughness");

        // Specular (Affects intensity of reflections for non-metals)
        SpecularPin = CreatePin(CMaterialInput::StaticClass(), "Specular", ENodePinDirection::Input, EMaterialInputType::Float);
        SpecularPin->SetPinName("Specular");

        // Emissive (Self-illumination, for glowing objects)
        EmissivePin = CreatePin(CMaterialInput::StaticClass(), "Emissive", ENodePinDirection::Input, EMaterialInputType::Float3);
        EmissivePin->SetPinName("Emissive (RGB)");

        // Ambient Occlusion (Shadows in crevices to add realism)
        AOPin = CreatePin(CMaterialInput::StaticClass(), "Ambient Occlusion", ENodePinDirection::Input, EMaterialInputType::Float);
        AOPin->SetPinName("Ambient Occlusion");

        // Normal Map (For surface detail)
        NormalPin = CreatePin(CMaterialInput::StaticClass(), "Normal Map (XYZ)", ENodePinDirection::Input, EMaterialInputType::Float3);
        NormalPin->SetPinName("Normal Map (XYZ)");
        
        // Opacity (For transparent materials)
        OpacityPin = CreatePin(CMaterialInput::StaticClass(), "Opacity", ENodePinDirection::Input, EMaterialInputType::Float);
        OpacityPin->SetPinName("Opacity");
    }

    void CMaterialOutputNode::GenerateDefinition(FMaterialCompiler* Compiler)
    {
        FString Output;
        Output += "\n \n";
    
        Output += "SMaterialInputs GetMaterialInputs()\n{\n";
        Output += "\tSMaterialInputs Input;\n";
        
        // Base Color
        Output += "\tInput.Diffuse = ";
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
        Output += "\tInput.Metallic = ";
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
        Output += "\tInput.Roughness = ";
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
        Output += "\tInput.Specular = ";
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
        Output += "\tInput.Emissive = ";
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
        Output += "\tInput.AmbientOcclusion = ";
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
        Output += "\tInput.Normal = ";
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
        Output += "\tInput.Opacity = ";
        if (OpacityPin->HasConnection())
        {
            Output += OpacityPin->GetConnections()[0]->GetOwningNode()->GetNodeFullName() + ".r;";
        }
        else
        {
            Output += "1.0;";
        }
        Output += "\n\treturn Input;\n}\n";
    
        Compiler->AddRaw(Output);
    }

}
