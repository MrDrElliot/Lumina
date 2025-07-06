#pragma once

#include "EdGraphNode.h"
#include "imgui.h"
#include "Containers/Array.h"
#include "Containers/String.h"
#include "Core/Math/Color.h"
#include "Core/Object/Object.h"
#include "EdNodeGraphPin.generated.h"

namespace Lumina
{
    LUM_CLASS()
    class CEdNodeGraphPin : public CObject
    {
        GENERATED_BODY()
    public:

        friend class CEdNodeGraph;
        friend class CEdGraphNode;

        CEdNodeGraphPin()
            : PinID(0)
            , bSingleInput(0)
            , bInputPin(0)
            , bDrawPinEditor(false)
            , bHidePinDuringConnection(true)
        {}
        
        virtual float DrawPin() { return 1.5f; }
        
        void SetPinName(const FString& Name) { PinName = Name; }
        const FString& GetPinName() const { return PinName; }
        const FString& GetPinTooltip() const { return PinName; }

        bool ShouldHideDuringConnection() const { return bHidePinDuringConnection; }
        void SetHideDuringConnection(bool bHide) { bHidePinDuringConnection = bHide; }
        
        virtual uint32 GetPinColor() const { return PinColor; }
        void SetPinColor(uint32 Color) { PinColor = Color;}
                
        void AddConnection(CEdNodeGraphPin* Pin) { Connections.push_back(Pin); }
        void RemoveConnection(CEdNodeGraphPin* Pin);
        TVector<CEdNodeGraphPin*> GetConnections() const { return Connections; }

        bool IsSingleInput() const { return bSingleInput; }
        bool HasConnection() const { return !Connections.empty(); }
        uint16 GetGUID() const { return PinID; }
        CEdGraphNode* GetOwningNode() const { return OwningNode; }

        bool ShouldDrawEditor() const { return bDrawPinEditor; }
        void SetShouldDrawEditor(bool bNew) { bDrawPinEditor = bNew; }
        
        template<typename T>
        requires(std::is_base_of_v<CEdGraphNode, T>)
        T* GetOwningNode()
        {
            return static_cast<T*>(GetOwningNode());
        }
        
    
    public:

        FString                         PinName;
        uint32                          PinColor = IM_COL32(255, 255, 255, 255);

        /** Index of the pin relative to the owning node */
        uint16                          PinID;
        
        TVector<CEdNodeGraphPin*>       Connections;
        
        CEdGraphNode*                   OwningNode = nullptr;
        uint8                           bSingleInput:1;
        uint8                           bInputPin:1;
        uint8                           bDrawPinEditor:1;
        uint8                           bHidePinDuringConnection:1;
    };
}
