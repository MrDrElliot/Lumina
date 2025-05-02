#include "ProjectSolution.h"

#include "Core/Assertions/Assert.h"

namespace Lumina::Reflection
{
    FProjectSolution::FProjectSolution(const std::filesystem::path& SlnPath)
        : Path(SlnPath.string().c_str())
        , ParentPath(SlnPath.parent_path().string().c_str())
    {
    }
}
