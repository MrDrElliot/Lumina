#pragma once
#include "imgui.h"
#include "Core/Templates/Forward.h"
#include "Containers/Array.h"
#include "Core/Math/Color.h"
#include "Memory/Memory.h"

namespace Lumina
{
    class FEdNodeGraphPin;

    inline uint32 GPinID;

    
    enum class ENodePinDirection : uint8
    {
        Input       = 0,
        Output      = 1,

        Count       = 2,
    };
    
    class FEdGraphNode
    {
    public:

        FEdGraphNode() = default;
        
        virtual ~FEdGraphNode();

        virtual void BuildNode() = 0;

        virtual FString GetNodeDisplayName() const = 0;
        virtual FString GetNodeTooltip() const = 0;
        virtual uint32 GetNodeTitleColor() const { return IM_COL32(200, 35, 35, 255); }
        virtual ImVec2 GetMinNodeSize() const { return ImVec2(100, 150); }

        void SetDebugExecutionOrder(uint32 Order) { DebugExecutionOrder = Order; }
        uint32 GetDebugExecutionOrder() const { return DebugExecutionOrder; }

        void SetError(const FString& InError) { Error = InError; bHasError = true; }
        FString GetError() const { return Error; }
        bool HasError() const { return bHasError; }
        void ClearError() { Error = FString(); bHasError = false; }
        
        FEdNodeGraphPin* GetPin(uint32 ID, ENodePinDirection Direction);
        FEdNodeGraphPin* GetPinByIndex(uint32 Index, ENodePinDirection Direction);
        

        const TVector<FEdNodeGraphPin*>& GetInputPins() const { return NodePins[uint32(ENodePinDirection::Input)]; }
        const TVector<FEdNodeGraphPin*>& GetOutputPins() const { return NodePins[uint32(ENodePinDirection::Output)]; }

        
        template<typename T, ENodePinDirection Direction, typename... Args>
        requires(std::is_base_of_v<FEdNodeGraphPin, T>)
        T* CreatePin(Args&&... args);
    
    protected:

        TArray<TVector<FEdNodeGraphPin*>, uint32(ENodePinDirection::Count)> NodePins;

        uint32 DebugExecutionOrder;

        FString Error;
        bool bHasError;
        
    };

    
    template <typename T, ENodePinDirection Direction, typename... Args>
    requires(std::is_base_of_v<FEdNodeGraphPin, T>)
    T* FEdGraphNode::CreatePin(Args&&... args)
    {
        T* NewPin = FMemory::New<T>(TForward<Args>(args)...);
        NewPin->GUID = GPinID++;
        NewPin->bInputPin = Direction == ENodePinDirection::Input;
        NewPin->OwningNode = this;
        NodePins[uint32(Direction)].push_back(NewPin);
            
        return NewPin;
    }
}

#define LUMINA_ED_GRAPH_NODE(NodeClass, DisplayName, TooltipText)        \
struct Info                                                              \
{                                                                        \
    static constexpr const char* Name = DisplayName;                     \
    static constexpr const char* Tooltip = TooltipText;                  \
    using NodeType = NodeClass;                                          \
                                                                         \
    static FString StaticDisplayName() { return FString(DisplayName); }  \
    static FString StaticTooltip() { return FString(TooltipText); }      \
                                                                         \
    static FEdGraphNode* CreateInstance() { return FMemory::New<NodeClass>(); }    \
};


