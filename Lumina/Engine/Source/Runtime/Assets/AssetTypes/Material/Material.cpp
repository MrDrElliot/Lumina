
#include "Material.h"
#include "Renderer/RHIIncl.h"

namespace Lumina
{

    void CMaterial::PostLoad()
    {
        for (const CObject* Node : MaterialNodes)
        {
            if (Node)
            {
                Node->AddRef();
            }
        }
    }

}
