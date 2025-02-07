#include "ImGuiX.h"

#include "ImGuiDesignIcons.h"
#include "imgui_internal.h"
#include "Core/Windows/Window.h"
#include "GLFW/glfw3.h"

#define IMDRAW_DEBUG

namespace Lumina::ImGuiX
{
    void ItemTooltip(const char* fmt, ...)
    {
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 4, 4 ) );
        if ( ImGui::IsItemHovered() && GImGui->HoveredIdTimer > 0.4f )
        {
            va_list args;
            va_start( args, fmt );
            ImGui::SetTooltipV( fmt, args );
            va_end( args );
        }
        ImGui::PopStyleVar();
    }

    void TextTooltip(const char* fmt, ...)
    {
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 4, 4 ) );
        if ( ImGui::IsItemHovered() )
        {
            va_list args;
            va_start( args, fmt );
            ImGui::SetTooltipV( fmt, args );
            va_end( args );
        }
        ImGui::PopStyleVar();
    }

    bool ButtonEx(char const* pIcon, char const* pLabel, ImVec2 const& size, const ImColor& backgroundColor, const ImColor& iconColor, const ImColor& foregroundColor, bool shouldCenterContents)
    {
         bool wasPressed = false;

        ImU32 const hoveredColor = backgroundColor * 1.15f;
        ImU32 const activeColor = backgroundColor * 1.25f;

        //-------------------------------------------------------------------------

        if ( pIcon == nullptr || strlen( pIcon ) == 0 )
        {
            ImGui::PushStyleColor( ImGuiCol_Button, (ImVec4) backgroundColor );
            ImGui::PushStyleColor( ImGuiCol_ButtonHovered, hoveredColor );
            ImGui::PushStyleColor( ImGuiCol_ButtonActive, activeColor );
            ImGui::PushStyleColor( ImGuiCol_Text, (ImVec4) foregroundColor );
            ImGui::PushStyleVar( ImGuiStyleVar_ButtonTextAlign, shouldCenterContents ? ImVec2( 0.5f, 0.5f ) : ImVec2( 0.0f, 0.5f ) );
            wasPressed = ImGui::Button( pLabel, size );
            ImGui::PopStyleColor( 4 );
            ImGui::PopStyleVar();
        }
        else // Icon button
        {
            ImGuiContext& g = *GImGui;

            ImGuiWindow* pWindow = ImGui::GetCurrentWindow();
            if ( pWindow->SkipItems )
            {
                return false;
            }

            // Calculate ID
            //-------------------------------------------------------------------------

            char const* pID = nullptr;
            if ( pLabel == nullptr || strlen( pLabel ) == 0 )
            {
                pID = pIcon;
            }
            else
            {
                pID = pLabel;
            }

            ImGuiID const ID = pWindow->GetID( pID );

            // Calculate sizes
            //-------------------------------------------------------------------------

            ImGuiStyle const& style = ImGui::GetStyle();
            ImVec2 const iconSize = ImGui::CalcTextSize( pIcon, nullptr, true );
            ImVec2 const labelSize = ImGui::CalcTextSize( pLabel, nullptr, true );
            float const spacing = ( iconSize.x > 0 && labelSize.x > 0 ) ? style.ItemSpacing.x : 0.0f;
            float const buttonWidth = labelSize.x + iconSize.x + spacing;
            float const buttonWidthWithFramePadding = buttonWidth + ( style.FramePadding.x * 2.0f );
            float const textHeightMax = std::max( iconSize.y, labelSize.y );
            float const buttonHeight = std::max( ImGui::GetFrameHeight(), textHeightMax );

            ImVec2 const pos = pWindow->DC.CursorPos;
            ImVec2 const finalButtonSize = ImGui::CalcItemSize( size, buttonWidthWithFramePadding, buttonHeight );

            // Add item and handle input
            //-------------------------------------------------------------------------

            ImGui::ItemSize( finalButtonSize, 0 );
            ImRect const bb( pos, pos + finalButtonSize );
            if ( !ImGui::ItemAdd( bb, ID ) )
            {
                return false;
            }

            bool hovered, held;
            wasPressed = ImGui::ButtonBehavior( bb, ID, &hovered, &held, 0 );

            // Render Button
            //-------------------------------------------------------------------------

            ImGui::PushStyleColor( ImGuiCol_Button, (ImVec4) backgroundColor );
            ImGui::PushStyleColor( ImGuiCol_ButtonHovered, hoveredColor );
            ImGui::PushStyleColor( ImGuiCol_ButtonActive, activeColor );
            ImGui::PushStyleColor( ImGuiCol_Text, (ImVec4) foregroundColor );

            // Render frame
            ImU32 const color = ImGui::GetColorU32( ( held && hovered ) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button );
            //ImGui::RenderNavCursor( bb, ID );
            ImGui::RenderFrame( bb.Min, bb.Max, color, true, style.FrameRounding );

            bool const isDisabled = g.CurrentItemFlags & ImGuiItemFlags_Disabled;
            const ImU32 finalIconColor = isDisabled ? iconColor : iconColor;

            if ( shouldCenterContents )
            {
                // Icon and Label - ensure label is centered!
                if ( labelSize.x > 0 )
                {
                    ImVec2 const textOffset( ( bb.GetWidth() / 2 ) - ( buttonWidthWithFramePadding / 2 ) + iconSize.x + spacing + style.FramePadding.x, 0 );
                    ImGui::RenderTextClipped( bb.Min + textOffset, bb.Max, pLabel, NULL, &labelSize, ImVec2( 0, 0.5f ), &bb );

                    float const offsetX = textOffset.x - iconSize.x - spacing;
                    float const offsetY = ( ( bb.GetHeight() - textHeightMax ) / 2.0f );
                    ImVec2 const iconOffset( offsetX, offsetY );
                    pWindow->DrawList->AddText( pos + iconOffset, finalIconColor, pIcon );
                }
                else // Only an icon
                {
                    float const offsetX = ( bb.GetWidth() - iconSize.x ) / 2.0f;
                    float const offsetY = ( ( bb.GetHeight() - iconSize.y ) / 2.0f );
                    ImVec2 const iconOffset( offsetX, offsetY );
                    pWindow->DrawList->AddText( pos + iconOffset, finalIconColor, pIcon );
                }
            }
            else // No centering
            {
                ImVec2 const textOffset( iconSize.x + spacing + style.FramePadding.x, 0 );
                ImGui::RenderTextClipped( bb.Min + textOffset, bb.Max, pLabel, NULL, &labelSize, ImVec2( 0, 0.5f ), &bb );

                float const iconHeightOffset = ( ( bb.GetHeight() - iconSize.y ) / 2.0f );
                pWindow->DrawList->AddText( pos + ImVec2( style.FramePadding.x, iconHeightOffset ), finalIconColor, pIcon );
            }

            ImGui::PopStyleColor( 4 );
        }

        //-------------------------------------------------------------------------

        return wasPressed;
    }

    void SameLineSeparator(float width, const ImColor& color)
    {
        const ImColor separatorColor = ImGui::GetStyleColorVec4( ImGuiCol_Separator);
        const ImVec2 seperatorSize( width <= 0 ? ( ImGui::GetStyle().ItemSpacing.x * 2 ) + 1 : width, ImGui::GetFrameHeight() );

        ImGui::SameLine( 0, 0 );

        ImVec2 const canvasPos = ImGui::GetCursorScreenPos();
        float const startPosX = canvasPos.x + std::floor( seperatorSize.x / 2 );
        float const startPosY = canvasPos.y + 1;
        float const endPosY = startPosY + seperatorSize.y - 2;

        ImDrawList* pDrawList = ImGui::GetWindowDrawList();
        pDrawList->AddLine( ImVec2( startPosX, startPosY ), ImVec2( startPosX, endPosY ), separatorColor, 1 );

        ImGui::Dummy( seperatorSize );
        ImGui::SameLine( 0, 0 );
    }

    void ApplicationTitleBar::DrawWindowControls()
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

        GLFWwindow* Window = (GLFWwindow*) ImGui::GetWindowViewport()->PlatformHandle;
        
        if (ImGuiX::FlatButton(LE_ICON_WINDOW_MINIMIZE "##Min", ImVec2(s_windowControlButtonWidth, -1)))
        {
            glfwIconifyWindow(Window);
        }

        ImGui::SameLine();

        if (ImGuiX::FlatButton(LE_ICON_WINDOW_RESTORE "##Res", ImVec2(s_windowControlButtonWidth, -1)))
        {
            glfwRestoreWindow(Window);
        }

        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.7f, 0.1f, 0.1f, 1.0f));
        if (ImGuiX::FlatButton(LE_ICON_WINDOW_CLOSE "##X", ImVec2(s_windowControlButtonWidth, -1)))
        {
            glfwSetWindowShouldClose(Window, GLFW_TRUE);
        }

        ImGui::PopStyleColor();
        ImGui::PopStyleVar(2);
    }


    void ApplicationTitleBar::Draw( TFunction<void()>&& menuDrawFunction, float menuSectionDesiredWidth, TFunction<void()>&& controlsSectionDrawFunction, float controlsSectionDesiredWidth )
    {
        Rect = glm::vec4(1.0f);
    
        ImVec2 const titleBarPadding(0, 8);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, titleBarPadding);
        ImGuiViewport* pViewport = ImGui::GetMainViewport();
    
        if (ImGui::BeginViewportSideBar("TitleBar", pViewport, ImGuiDir_Up, 40, ImGuiWindowFlags_NoDecoration))
        {
            ImGui::PopStyleVar();
    
            // Calculate sizes
            float const titleBarWidth = ImGui::GetWindowSize().x;
            float const titleBarHeight = ImGui::GetWindowSize().y;
            Rect = glm::vec4(0.0f, 0.0f, titleBarWidth, titleBarHeight);
    
            float const windowControlsWidth = GetWindowsControlsWidth();
            float const windowControlsStartPosX = std::max( 0.0f, ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - windowControlsWidth );
            ImVec2 const windowControlsStartPos( windowControlsStartPosX, ImGui::GetCursorPosY() - titleBarPadding.y );

            // Calculate the available space
            float const availableSpace = titleBarWidth - windowControlsWidth - s_minimumDraggableGap - ( s_sectionPadding * 2 );
            float remainingSpace = availableSpace;

            // Calculate section widths
            float const menuSectionFinalWidth = ( remainingSpace - menuSectionDesiredWidth ) > 0 ? menuSectionDesiredWidth : std::max( 0.0f, remainingSpace );
            remainingSpace -= menuSectionFinalWidth;
            ImVec2 const menuSectionStartPos( s_sectionPadding, ImGui::GetCursorPosY() );

            float const controlSectionFinalWidth = ( remainingSpace - controlsSectionDesiredWidth ) > 0 ? controlsSectionDesiredWidth : std::max( 0.0f, remainingSpace );
            remainingSpace -= controlSectionFinalWidth;
            ImVec2 const controlSectionStartPos( windowControlsStartPos.x - s_sectionPadding - controlSectionFinalWidth, ImGui::GetCursorPosY() );

            ImVec2 DragAreaStartPos = menuSectionStartPos;
            DragAreaStartPos.x += menuSectionFinalWidth + 10;
            
            // Dragging Logic
            GLFWwindow* Window = (GLFWwindow*)ImGui::GetWindowViewport()->PlatformHandle;
            ImGui::SetCursorPos(DragAreaStartPos);
            ImGui::InvisibleButton("TitleBarDragZone", ImVec2(remainingSpace, titleBarHeight));
            
            static bool isDragging = false;
            static ImVec2 initialClickPos;
            static ImVec2 initialWindowPos;

            if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
            {
                isDragging = true;
                initialClickPos = ImGui::GetMousePos();

                int winX, winY;
                glfwGetWindowPos(Window, &winX, &winY);
                initialWindowPos = ImVec2((float)winX, (float)winY);
            }

            if (isDragging && ImGui::IsMouseDown(ImGuiMouseButton_Left))
            {
                ImVec2 mousePos = ImGui::GetMousePos();
                ImVec2 delta = ImVec2(mousePos.x - initialClickPos.x, mousePos.y - initialClickPos.y);

                glfwSetWindowPos(Window, (int)(initialWindowPos.x + delta.x), (int)(initialWindowPos.y + delta.y));
            }
            else
            {
                isDragging = false;
            }

    
            // Draw menu section
            if (menuSectionDesiredWidth > 0)
            {
                ImGui::SetCursorPos(menuSectionStartPos);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImGui::GetStyle().FramePadding + ImVec2(0, 2));
                bool const drawMenuSection = ImGui::BeginChild("Left", ImVec2(menuSectionDesiredWidth, titleBarHeight), 
                                                               ImGuiChildFlags_AlwaysUseWindowPadding, 
                                                               ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_MenuBar);
                
                
                ImGui::PopStyleVar(2);
                if (drawMenuSection)
                {
                    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(16, 8));
                    if (ImGui::BeginMenuBar())
                    {
                        menuDrawFunction();
                        ImGui::EndMenuBar();
                    }
                    ImGui::PopStyleVar();
                }
                ImGui::EndChild();
            }
            
            if (controlsSectionDesiredWidth > 0)
            {
                ImGui::SetCursorPos( controlSectionStartPos );
                ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0, 0 ) );
                bool const drawControlsSection = ImGui::BeginChild( "Right", ImVec2( controlSectionFinalWidth, titleBarHeight ),
                    ImGuiChildFlags_AlwaysUseWindowPadding,
                    ImGuiWindowFlags_NoDecoration );
                
                ImGui::PopStyleVar();

                if ( drawControlsSection )
                {
                    controlsSectionDrawFunction();
                }
                ImGui::EndChild();

            }

    
            ImGui::SetCursorPos(windowControlsStartPos);
            if (ImGui::BeginChild("WindowControls", ImVec2(windowControlsWidth, titleBarHeight), false, ImGuiWindowFlags_NoDecoration))
            {
                DrawWindowControls();
                ImGui::EndChild();
            }
            
            ImGui::End();
        }
    }
}

#undef IMDRAW_DEBUG