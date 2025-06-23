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

        friend struct Construct_CClass_Lumina_CEdNodeGraphPin_Statics;
        friend class CEdNodeGraph;
        friend class CEdGraphNode;

        CEdNodeGraphPin()
            : bSingleInput(0)
            , bInputPin(0)
            , bDrawPinEditor(false)
            , bHidePinDuringConnection(true)
        {}

        
        virtual void DrawPin() { }
        
        FORCEINLINE void SetPinName(const FString& Name) { PinName = Name; }
        FORCEINLINE const FString& GetPinName() const { return PinName; }
        FORCEINLINE const FString& GetPinTooltip() const { return PinName; }

        FORCEINLINE bool ShouldHideDuringConnection() const { return bHidePinDuringConnection; }
        FORCEINLINE void SetHideDuringConnection(bool bHide) { bHidePinDuringConnection = bHide; }
        
        virtual uint32 GetPinColor() const { return PinColor; }
        void SetPinColor(uint32 Color) { PinColor = Color;}
                
        void AddConnection(CEdNodeGraphPin* Pin) { Connections.push_back(Pin); }
        void RemoveConnection(CEdNodeGraphPin* Pin);
        TVector<CEdNodeGraphPin*> GetConnections() const { return Connections; }

        FORCEINLINE bool IsSingleInput() const { return bSingleInput; }
        FORCEINLINE bool HasConnection() const { return !Connections.empty(); }
        FORCEINLINE uint16 GetGUID() const { return PinID; }
        FORCEINLINE CEdGraphNode* GetOwningNode() const { return OwningNode; }

        FORCEINLINE bool ShouldDrawEditor() const { return bDrawPinEditor; }
        FORCEINLINE void SetShouldDrawEditor(bool bNew) { bDrawPinEditor = bNew; }
        
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
