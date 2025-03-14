#pragma once

#include "Containers/String.h"

namespace Lumina
{
    class FEdGraphNode;
}

namespace Lumina
{
    class FNodeGraphPin
    {
    public:
        

        FNodeGraphPin(const FString& Name, FEdGraphNode* Owner)
            : PinName(Name)
            , OwingNode(Owner)
        {}

        virtual ~FNodeGraphPin() = default;

        virtual void Draw();
    
    private:

        FString             PinName;
        FEdGraphNode*       OwingNode = nullptr;
        FNodeGraphPin*      Connection = nullptr;
    };
}
