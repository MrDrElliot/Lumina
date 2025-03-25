#pragma once

#include "imgui.h"
#include "Containers/Array.h"
#include "Containers/String.h"
#include "Core/Math/Color.h"
#include "Core/Object/Object.h"

namespace Lumina
{
    class FEdNodeGraphPin : public LEObject
    {
    public:

        friend class FEdNodeGraph;
        friend class FEdGraphNode;

        FEdNodeGraphPin()
            : bSingleInput(0)
            , bInputPin(0)
            , bDrawPinEditor(false)
            , bHidePinDuringConnection(true)
        {}

        
        
        virtual ~FEdNodeGraphPin() = default;

        virtual void DrawPin() { }
        
        FORCEINLINE void SetPinName(const FString& Name) { PinName = Name; }
        FORCEINLINE const FString& GetPinName() const { return PinName; }
        FORCEINLINE const FString& GetPinTooltip() const { return PinName; }

        FORCEINLINE bool ShouldHideDuringConnection() const { return bHidePinDuringConnection; }
        FORCEINLINE void SetHideDuringConnection(bool bHide) { bHidePinDuringConnection = bHide; }
        
        virtual uint32 GetPinColor() const { return PinColor; }
        void SetPinColor(uint32 Color) { PinColor = Color;}
                
        void AddConnection(FEdNodeGraphPin* Pin) { Connections.push_back(Pin); }
        void RemoveConnection(FEdNodeGraphPin* Pin);
        TVector<FEdNodeGraphPin*> GetConnections() const { return Connections; }

        FORCEINLINE bool IsSingleInput() const { return bSingleInput; }
        FORCEINLINE bool HasConnection() const { return !Connections.empty(); }
        FORCEINLINE uint32 GetGUID() const { return GUID; }
        FORCEINLINE FEdGraphNode* GetOwningNode() const { return OwningNode; }

        FORCEINLINE bool ShouldDrawEditor() const { return bDrawPinEditor; }
        FORCEINLINE void SetShouldDrawEditor(bool bNew) { bDrawPinEditor = bNew; }
        
        template<typename T>
        requires(std::is_base_of_v<FEdGraphNode, T>)
        T* GetOwningNode()
        {
            return static_cast<T*>(GetOwningNode());
        }
        
    
    protected:

        FString                         PinName;
        uint32                          PinColor = IM_COL32(255, 255, 255, 255);
        uint16                          GUID = 0;
        
        TVector<FEdNodeGraphPin*>       Connections;
        FEdGraphNode*                   OwningNode = nullptr;
        uint8                           bSingleInput:1;
        uint8                           bInputPin:1;
        uint8                           bDrawPinEditor:1;
        uint8                           bHidePinDuringConnection:1;
    };
}
