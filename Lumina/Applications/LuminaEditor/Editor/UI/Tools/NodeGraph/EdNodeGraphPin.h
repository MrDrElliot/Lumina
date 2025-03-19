#pragma once

#include "imgui.h"
#include "Containers/Array.h"
#include "Containers/String.h"
#include "Core/Math/Color.h"

namespace Lumina
{
    class FEdNodeGraphPin
    {
    public:

        friend class FEdNodeGraph;
        friend class FEdGraphNode;

        FEdNodeGraphPin() = default;
        virtual ~FEdNodeGraphPin() = default;

        virtual void DrawPin() { }
        const FString GetPinName() const { return PinName; }
        const FString GetPinTooltip() const { return PinName; }

        virtual uint32 GetPinColor() const { return PinColor; }
        void SetPinColor(uint32 Color) { PinColor = Color;}
                
        void AddConnection(FEdNodeGraphPin* Pin) { Connections.push_back(Pin); }
        void RemoveConnection(FEdNodeGraphPin* Pin);
        TVector<FEdNodeGraphPin*> GetConnections() const { return Connections; }

        FORCEINLINE bool IsSingleInput() const { return bSingleInput; }
        FORCEINLINE bool HasConnection() const { return !Connections.empty(); }
        FORCEINLINE uint32 GetGUID() const { return GUID; }
        FORCEINLINE FEdGraphNode* GetOwningNode() const { return OwningNode; }

        template<typename T>
        requires(std::is_base_of_v<FEdGraphNode, T>)
        T* GetOwningNode()
        {
            return static_cast<T*>(GetOwningNode());
        }
        
    
    protected:

        FString                         PinName;
        uint32                          PinColor = IM_COL32(255, 255, 255, 255);
        uint32                          GUID = 0;
        
        TVector<FEdNodeGraphPin*>       Connections;
        FEdGraphNode*                   OwningNode = nullptr;
        uint8                           bSingleInput:1;
        uint8                           bInputPin:1;
    };
}
