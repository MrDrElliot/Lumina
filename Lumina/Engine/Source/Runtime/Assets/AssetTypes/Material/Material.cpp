
#include "Material.h"
#include "Renderer/RHIIncl.h"

namespace Lumina
{

    void CMaterial::UpdateStreamableResource()
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
