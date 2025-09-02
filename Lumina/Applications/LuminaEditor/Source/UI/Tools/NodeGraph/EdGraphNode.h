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

        void PostCreateCDO() override;
        
        ~CEdGraphNode() override;

        virtual void BuildNode() { }

        void Serialize(FArchive& Ar) override;

        virtual FInlineString GetNodeCategory() const { return "General"; }
        
        FString GetNodeFullName() { return FullName; }
        virtual bool WantsTitlebar() const { return true; }
        virtual FString GetNodeDisplayName() const { return "Node"; }
        virtual FString GetNodeTooltip() const { return "No Tooltip"; }
        virtual uint32 GetNodeTitleColor() const { return IM_COL32(200, 35, 35, 255); }
        virtual ImVec2 GetMinNodeBodySize() const { return ImVec2(100, 150); }
        virtual ImVec2 GetMinNodeTitleBarSize() const { return ImVec2(100, 28); }

        virtual void DrawNodeBody() { };

        void SetDebugExecutionOrder(uint32 Order) { DebugExecutionOrder = Order; }
        uint32 GetDebugExecutionOrder() const { return DebugExecutionOrder; }

        virtual void PushNodeStyle();
        virtual void PopNodeStyle();

        virtual void DrawContextMenu() { }
        virtual void DrawNodeTitleBar();

        void SetError(const FString& InError) { Error = InError; bHasError = true; }
        FString GetError() const { return Error; }
        bool HasError() const { return bHasError; }
        void ClearError() { Error = FString(); bHasError = false; }
        
        CEdNodeGraphPin* GetPin(uint16 ID, ENodePinDirection Direction);
        CEdNodeGraphPin* GetPinByIndex(uint32 Index, ENodePinDirection Direction);
        
        uint64 GetNodeID() const { return NodeID; }

        void SetGridPos(float X, float Y) { GridX = X; GridY = Y; }
        float GetNodeX() const { return GridX; }
        float GetNodeY() const { return GridY; }

        const TVector<CEdNodeGraphPin*>& GetInputPins() const { return NodePins[uint32(ENodePinDirection::Input)]; }
        const TVector<CEdNodeGraphPin*>& GetOutputPins() const { return NodePins[uint32(ENodePinDirection::Output)]; }

        CEdNodeGraphPin* CreatePin(CClass* InClass, const FString& Name, ENodePinDirection Direction, EMaterialInputType Type);

        LUM_PROPERTY()
        float GridX;

        LUM_PROPERTY()
        float GridY;

    protected:

        TArray<TVector<CEdNodeGraphPin*>, uint32(ENodePinDirection::Count)> NodePins;

        uint32 DebugExecutionOrder;


        uint64      NodeID = 0;
        SIZE_T      Index = 0;
        FString     FullName;
        FString     Error;
        bool        bHasError;
        bool        bInitialPosSet = false;
        bool        bWasBuild = false;
    };
    
}
