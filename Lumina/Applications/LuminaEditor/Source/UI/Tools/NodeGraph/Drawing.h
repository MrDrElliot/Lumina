#pragma once
#include "imgui.h"
#include "imgui-node-editor/imgui_node_editor.h"

namespace Lumina
{
    
    enum class EIconType: ImU32
    {
        Flow,
        Circle,
        Square,
        Grid,
        RoundSquare,
        Diamond
    };

    void DrawIcon(ImDrawList* drawList, const ImVec2& a, const ImVec2& b, EIconType type, bool filled, ImU32 color, ImU32 innerColor);
    void Icon(const ImVec2& size, EIconType type, bool filled, const ImVec4& color = ImVec4(1, 1, 1, 1), const ImVec4& innerColor = ImVec4(0, 0, 0, 0));

    namespace Graph
    {
        struct GraphNodeBuilder
        {
            GraphNodeBuilder();

            void Begin(ax::NodeEditor::NodeId id);
            void End(bool bWantsTitlebar);

            void Header(const ImVec4& color = ImVec4(1, 1, 1, 1));
            void EndHeader();

            void Input(ax::NodeEditor::PinId id);
            void EndInput();

            void Middle();

            void Output(ax::NodeEditor::PinId id);
            void EndOutput();


        private:
            
            enum class Stage
            {
                Invalid,
                Begin,
                Header,
                Content,
                Input,
                Output,
                Middle,
                End
            };

            bool SetStage(Stage stage);

            void Pin(ax::NodeEditor::PinId, ax::NodeEditor::PinKind kind);
            void EndPin();
            
            ax::NodeEditor::NodeId      CurrentNodeId;
            Stage                       CurrentStage;
            ImU32                       HeaderColor;
            ImVec2                      NodeMin;
            ImVec2                      NodeMax;
            ImVec2                      HeaderMin;
            ImVec2                      HeaderMax;
            ImVec2                      ContentMin;
            ImVec2                      ContentMax;
            bool                        HasHeader;
        };
    }
}
