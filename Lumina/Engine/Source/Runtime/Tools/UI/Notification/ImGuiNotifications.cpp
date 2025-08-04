#include "ImGuiNotifications.h"
#include "Tools/UI/ImGui/ImGuiX.h"
#include "imgui.h"
#include "Containers/Array.h"
#include "Containers/String.h"
#include "GLFW/glfw3.h"
#include "Module/API.h"
#include "Tools/UI/ImGui/ImGuiDesignIcons.h"

namespace Lumina::ImGuiX::Notifications
{

     class LUMINA_API FNotification
     {
         constexpr static float s_defaultLifetime = 3.0f;
         constexpr static float s_defaultFadeTime = 1.0f;

     public:

         enum class EType
         {
             None,
             Success,
             Warning,
             Error,
             Info,
         };

         enum class EPhase
         {
             FadeIn,
             Wait,
             FadeOut,
             Expired,
         };

         enum class EPosition
         {
             TopLeft,
             TopCenter,
             TopRight,
             BottomLeft,
             BottomCenter,
             BottomRight,
             Center,
         };

     public:

         FNotification(EType type, FInlineString&& Message)
             : Type(type)
             , Message(Message)
         {
             CreationTime = glfwGetTime();
         }

         EType GetType() const { return Type; }

         EPhase GetPhase() const
         {
             float const elapsedTime = glfwGetTime() - CreationTime;

             if ( elapsedTime > s_defaultFadeTime + Lifetime + s_defaultFadeTime )
             {
                 return EPhase::Expired;
             }
             else if ( elapsedTime > s_defaultFadeTime + Lifetime )
             {
                 return EPhase::FadeOut;
             }
             else if ( elapsedTime > s_defaultFadeTime )
             {
                 return EPhase::Wait;
             }
             else
             {
                 return EPhase::FadeIn;
             }
         }

         float GetFadePercentage() const
         {
             EPhase const phase = GetPhase();
             float const elapsedTime = glfwGetTime() - CreationTime;

             if ( phase == EPhase::FadeIn )
             {
                 return elapsedTime / s_defaultFadeTime;
             }
             else if ( phase == EPhase::FadeOut )
             {
                 return ( 1.f - ( ( elapsedTime - s_defaultFadeTime - Lifetime) / s_defaultFadeTime ) );
             }

             return 1.f;
         }

         ImVec4 GetColor( float opacity = 1.0f ) const
         {
             switch (Type)
             {
             case EType::Success:
                 return { 0, 255, 0, opacity }; // Green

             case EType::Warning:
                 return { 255, 255, 0, opacity }; // Yellow

             case EType::Error:
                 return { 255, 0, 0, opacity }; // Error

             case EType::Info:
                 return { 0, 157, 255, opacity }; // Blue

             default:
                 break;
             }

             //-------------------------------------------------------------------------

             return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
         }

         char const* GetIcon() const
         {
             switch (Type)
             {
             case EType::Success:
                 return LE_ICON_CHECK_CIRCLE;

             case EType::Warning:
                 return LE_ICON_ALERT;

             case EType::Error:
                 return LE_ICON_CLOSE_CIRCLE;

             case EType::Info:
                 return LE_ICON_INFORMATION;

             default:
                 break;
             }

             //-------------------------------------------------------------------------

             return nullptr;
         }

         char const* GetTitle() const
         {
             switch (Type)
             {
             case EType::Success:
                 return "Success";

             case EType::Warning:
                 return "Warning";

             case EType::Error:
                 return "Error";

             case EType::Info:
                 return "Info";

             default:
                 break;
             }

             //-------------------------------------------------------------------------

             return nullptr;
         };

         char const* GetMessage() const { return Message.c_str(); }
         
     private:

         EType                           Type = EType::None;
         FInlineString                   Message;
         float                           Lifetime = s_defaultLifetime;
         double                          CreationTime = -1.0f;
     };
    
    static TFixedVector<FNotification, 100> GNotifications;
    
    void Initialize()
    {
    }

    void Shutdown()
    {
    }

    void Render()
    {
        constexpr static float paddingX = 20.0f; // Bottom-left X padding
        constexpr static float paddingY = 20.0f; // Bottom-left Y padding
        constexpr static float paddingNotificationY = 10.0f; // Padding Y between each message

         //-------------------------------------------------------------------------

        ImGuiViewport const* pViewport = ImGui::GetMainViewport();
        ImVec2 const viewportSize = pViewport->Size;

        float notificationStartPosY = 0.f;
        for ( auto i = 0; i < GNotifications.size(); i++ )
        {
            FNotification* pNotification = &GNotifications[i];

            // Remove notification if expired
            //-------------------------------------------------------------------------

            if (pNotification->GetPhase() == FNotification::EPhase::Expired)
            {
                GNotifications.erase(GNotifications.begin() + i );
                i--;
                continue;
            }

            // Preparation
            //-------------------------------------------------------------------------

            // Get icon, title and other data
            char const* pIcon = pNotification->GetIcon();
            char const* pTitle = pNotification->GetTitle();
            char const* pMessage = pNotification->GetMessage();
            float const opacity = pNotification->GetFadePercentage(); // Get opacity based of the current phase
            ImVec4 const textColor = pNotification->GetColor( opacity );

            // Generate new unique name for this notification
            FInlineString windowName(FInlineString::CtorSprintf(), "##Notification%d", i);

            // Draw Notification
            //-------------------------------------------------------------------------

            ImGui::SetNextWindowBgAlpha( opacity );
            ImGui::SetNextWindowPos( pViewport->Pos + ImVec2( viewportSize.x - paddingX, viewportSize.y - paddingY - notificationStartPosY ), ImGuiCond_Always, ImVec2(1.0f, 1.0f)) ;
            ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize, 1 );
            ImGui::PushStyleColor( ImGuiCol_Border, textColor );
            if (ImGui::Begin(windowName.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing))
            {
                ImGui::PushTextWrapPos( viewportSize.x / 3.f ); // We want to support multi-line text, this will wrap the text after 1/3 of the screen width

                bool drawSeparator = false;

                // If an icon is set
                if ( pIcon != nullptr )
                {
                    ImGui::TextColored( textColor, pIcon );
                    drawSeparator = true;
                }

                if ( pTitle != nullptr )
                {
                    if ( pIcon != nullptr )
                    {
                        ImGui::SameLine();
                    }

                    ImGui::Text( pTitle ); // Render default title text (Success -> "Success", etc...)
                    drawSeparator = true;
                }

                // In case ANYTHING was rendered in the top, we want to add a small padding so the text (or icon) looks centered vertically
                if ( drawSeparator && pMessage != nullptr )
                {
                    ImGui::SetCursorPosY( ImGui::GetCursorPosY() + 5.f ); // Must be a better way to do this!!!!
                }

                // If a content is set
                if ( pMessage != nullptr )
                {
                    if ( drawSeparator )
                    {
                        ImGui::Separator();
                    }

                    ImGui::Text( pMessage ); // Render content text
                }

                ImGui::PopTextWrapPos();
            }

            // Save height for next notifications
            notificationStartPosY += ImGui::GetWindowHeight() + paddingNotificationY;

            ImGui::End();
            ImGui::PopStyleVar();
            ImGui::PopStyleColor();
        }
    }

    LUMINA_API void NotifyInfo( const char* pFormat, ... )
    {
        va_list args;
        va_start( args, pFormat );
        FInlineString str;
        str.append_sprintf_va_list( pFormat, args );
        GNotifications.emplace_back(FNotification::EType::Info, str.c_str());
        va_end( args );
    }

    LUMINA_API void NotifySuccess( const char* pFormat, ... )
    {
        va_list args;
        va_start( args, pFormat );
        FInlineString str;
        str.append_sprintf_va_list( pFormat, args );
        GNotifications.emplace_back(FNotification::EType::Success, str.c_str());
        va_end( args );
    }

    LUMINA_API void NotifyWarning( const char* pFormat, ... )
    {
        va_list args;
        va_start( args, pFormat );
        FInlineString str;
        str.append_sprintf_va_list( pFormat, args );
        GNotifications.emplace_back(FNotification::EType::Warning, str.c_str());
        va_end( args );
    }

    LUMINA_API void NotifyError( const char* pFormat, ... )
    {
        va_list args;
        va_start( args, pFormat );
        FInlineString str;
        str.append_sprintf_va_list( pFormat, args );
        GNotifications.emplace_back(FNotification::EType::Error, str.c_str());
        va_end( args );
    }
    
}