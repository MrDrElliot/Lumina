#pragma once

#include "EdNodeGraphPin.h"
#include <imgui.h>
#include "Containers/Array.h"
#include "Containers/Name.h"
#include "Core/Math/Math.h"
#include "Core/Object/Object.h"
#include "EdGraphNode.generated.h"

namespace Lumina
{
    enum class EMaterialInputType : uint8;
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

        CEdNodeGraphPin* CreatePin(CClass* InClass, const FString& Name, ENodePinDirection Direction, EMaterialInputType Type);
    
    protected:

        TArray<TVector<CEdNodeGraphPin*>, uint32(ENodePinDirection::Count)> NodePins;

        uint32 DebugExecutionOrder;

        FName       FactoryID;
        FString     FullName;
        uint16      GUID;
        FString     Error;
        bool        bHasError;
        
    };
    
}
