#include "MaterialNodeGetTime.h"

#include "UI/Tools/NodeGraph/Material/MaterialCompiler.h"

namespace Lumina
{
    uint32 CMaterialNodeGetTime::GenerateExpression(FMaterialCompiler* Compiler)
    {
        return CMaterialExpression::GenerateExpression(Compiler);
    }

    void CMaterialNodeGetTime::GenerateDefinition(FMaterialCompiler* Compiler)
    {
        Compiler->Time(FullName);
    }
}
