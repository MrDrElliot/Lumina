#pragma once
#include "Containers/Array.h"
#include "Containers/Name.h"
#include "Memory/Memory.h"

namespace Lumina
{
    class FNodeGraphPin;

    
    enum class ENodePinDirection : uint8
    {
        Input,
        Output,

        Count = Output,
        Max,
    };
    
    class FEdGraphNode
    {
    public:

        
        FNodeGraphPin* GetPin(const FName& PinName, ENodePinDirection Direction);

        template<typename T, ENodePinDirection Direction, typename... Args>
        T* CreatePin(const FName& PinName, Args&&... args);
    
    private:

        THashMap<FName, FNodeGraphPin*> NodePins[ENodePinDirection::Count];
        
    };


    

    template <typename T, ENodePinDirection Direction, typename ... Args>
    T* FEdGraphNode::CreatePin(const FName& PinName, Args&&... args)
    {
        T* NewPin = FMemory::New<T>(TForward<Args>(args)...);

        NodePins[uint32(Direction)].insert_or_assign(PinName, NewPin);
            
        return NewPin;
    }
}
