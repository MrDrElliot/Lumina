#pragma once

#include "EdNodeGraphPin.h"
#include <imgui.h>
#include "Containers/Array.h"
#include "Containers/Name.h"
#include "Core/Math/Math.h"
#include "Core/Object/Object.h"
#include "GUID/GUID.h"
#include "EdGraphNode.generated.h"

namespace Lumina
{
    class CEdNodeGraphPin;
    
    enum class ENodePinDirection : uint8
    {
        Input       = 0,
        Output      = 1,

        Count       = 2,
    };

    LUM_CLASS()
    class CEdGraphNode : public CObject
    {
        GENERATED_BODY()
        
    public:


        friend class CEdNodeGraph;

        CEdGraphNode() = default;
        
        virtual ~CEdGraphNode();

        virtual void BuildNode() { }

        void Serialize(FArchive& Ar) override;

        FString GetNodeFullName() { return FullName; }
        virtual FString GetNodeDisplayName() const { return "Node"; }
        virtual FString GetNodeTooltip() const { return "No Tooltip"; }
        virtual uint32 GetNodeTitleColor() const { return IM_COL32(200, 35, 35, 255); }
        virtual ImVec2 GetMinNodeSize() const { return ImVec2(100, 150); }

        void SetDebugExecutionOrder(uint32 Order) { DebugExecutionOrder = Order; }
        uint32 GetDebugExecutionOrder() const { return DebugExecutionOrder; }

        void SetError(const FString& InError) { Error = InError; bHasError = true; }
        FString GetError() const { return Error; }
        bool HasError() const { return bHasError; }
        void ClearError() { Error = FString(); bHasError = false; }
        
        CEdNodeGraphPin* GetPin(uint32 ID, ENodePinDirection Direction);
        CEdNodeGraphPin* GetPinByIndex(uint32 Index, ENodePinDirection Direction);
        

        const TVector<CEdNodeGraphPin*>& GetInputPins() const { return NodePins[uint32(ENodePinDirection::Input)]; }
        const TVector<CEdNodeGraphPin*>& GetOutputPins() const { return NodePins[uint32(ENodePinDirection::Output)]; }

        
        template<typename T, ENodePinDirection Direction, typename... Args>
        requires(std::is_base_of_v<CEdNodeGraphPin, T>)
        T* CreatePin(Args&&... args);
    
    protected:

        TArray<TVector<CEdNodeGraphPin*>, uint32(ENodePinDirection::Count)> NodePins;

        uint32 DebugExecutionOrder;

        FName       FactoryID;
        FString     FullName;
        uint16      GUID;
        FString     Error;
        bool        bHasError;
        
    };

    
    template <typename T, ENodePinDirection Direction, typename... Args>
    requires(std::is_base_of_v<CEdNodeGraphPin, T>)
    T* CEdGraphNode::CreatePin(Args&&... args)
    {
        /*CEdNodeGraphPin* NewPin = NewObject<T>();
        NewPin->GUID = Math::RandRange<uint16>(0, UINT16_MAX);
        NewPin->bInputPin = Direction == ENodePinDirection::Input;
        NewPin->OwningNode = this;
        NodePins[uint32(Direction)].push_back(NewPin);*/
            
        return nullptr;
    }
}
