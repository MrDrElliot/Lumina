#pragma once

#include "imgui.h"
#include "imgui_internal.h"
#include "ToolFlags.h"
#include "Core/Math/Hash/Hash.h"
#include "Containers/Array.h"
#include "Containers/String.h"
#include "Core/UpdateContext.h"
#include "Core/Functional/Function.h"
#include "Memory/RefCounted.h"
#include "Scene/Scene.h"
#include "Scene/Entity/Entity.h"
#include "Tools/UI/ImGui/ImGuiDesignIcons.h"

namespace Lumina
{
    class FPrimitiveDrawManager;
    enum class EEditorToolFlags : uint8;
    class IEditorToolContext;
    class FUpdateContext;
}

namespace Lumina
{
    class FEditorTool
    {
    public:

        friend class FEditorUI;
        virtual ~FEditorTool() = default;

        constexpr static char const* const ViewportWindowName = "Viewport";

        //--------------------------------------------------------------------------
        
        class FToolWindow
        {
            friend class FEditorTool;
            friend class FEditorUI;

        public:

            FToolWindow(const FString& InName, const TFunction<void(const FUpdateContext&, bool)>& InDrawFunction, const ImVec2& InWindowPadding = ImVec2(-1, -1), bool bDisableScrolling = false)
                : Name(InName)
                , DrawFunction(InDrawFunction)
                , WindowPadding(InWindowPadding)
            {}
        
        protected:
            
            FString                                                       Name;
            TFunction<void(const FUpdateContext& UpdateContext, bool)>    DrawFunction;
            ImVec2                                                        WindowPadding;
            bool                                                          bViewport = false;
            bool                                                          bOpen = true;
            
        };

        //--------------------------------------------------------------------------

    public:

        FEditorTool(const IEditorToolContext* Context, const FString& DisplayName, FScene* Scene = nullptr);

        virtual void Initialize(const FUpdateContext& UpdateContext);
        virtual void Deinitialize(const FUpdateContext& UpdateContext);
        
        FORCEINLINE const FString& GetToolName() const { return ToolName; }
        

        FORCEINLINE ImGuiID CalculateDockspaceID() const
        {
            int32_t dockspaceID = CurrLocationID;
            char const* const pEditorToolTypeName = GetUniqueTypeName();
            dockspaceID = ImHashData( pEditorToolTypeName, strlen( pEditorToolTypeName ), dockspaceID );
            return dockspaceID;
        }

        FORCEINLINE FInlineString GetToolWindowName(const FString& Name) const { return GetToolWindowName(Name.c_str(), CurrDockspaceID); }
        
        FORCEINLINE ImGuiWindowClass* GetWindowClass() { return &ToolWindowsClass; }
        FORCEINLINE EEditorToolFlags GetToolFlags() const { return ToolFlags; }
        FORCEINLINE bool HasFlag(EEditorToolFlags Flag) const {  return (ToolFlags & Flag) == Flag; }

        FORCEINLINE FScene* GetScene() const { return Scene; }
        FORCEINLINE bool HasScene() const { return Scene != nullptr; }
        FORCEINLINE ImGuiID GetCurrentDockspaceID() const { return CurrDockspaceID; }

        virtual void InitializeDockingLayout(ImGuiID InDockspaceID, const ImVec2& InDockspaceSize) const;
        
        virtual void OnInitialize(const FUpdateContext& UpdateContext) = 0;
        virtual void OnDeinitialize(const FUpdateContext& UpdateContext) = 0;

        virtual bool IsSingleWindowTool() const { return false; }

        // Get the hash of the unique type ID for this tool
        virtual uint32 GetUniqueTypeID() const = 0;

        // Get the unique typename for this tool to be used for docking
        virtual char const* GetUniqueTypeName() const = 0;
        
        /** Called just before updating the scene at each stage */
        virtual void SceneUpdate(const FUpdateContext& UpdateContext) { }

        /** Once per-frame update */
        virtual void Update(const FUpdateContext& UpdateContext) { }
        
        /** Optionally draw a tool bar at the top of the window */
        void DrawMainToolbar(const FUpdateContext& UpdateContext);

        /** Allows the child to draw specific menu actions */
        virtual void DrawToolMenu(const FUpdateContext& UpdateContext) { }

        /** Viewport overlay to draw any elements to the window's viewport */
        virtual void DrawViewportOverlayElements(const FUpdateContext& UpdateContext, ImTextureID ViewportTexture) { }
        
        /** Draw the optional viewport for this tool window, returns true if focused. */
        virtual bool DrawViewport(const FUpdateContext& UpdateContext, ImTextureID ViewportTexture);
        
        /** Can there only ever be one of this tool? */
        virtual bool IsSingleton() const { return HasFlag(EEditorToolFlags::Tool_Singleton); }
        
        /** Optional title bar icon override */
        virtual const char* GetTitlebarIcon() const { return LE_ICON_CAR_WRENCH; }

        
        FToolWindow* CreateToolWindow(const FString& InName, const TFunction<void(const FUpdateContext&, bool)>& DrawFunction, const ImVec2& WindowPadding = ImVec2( -1, -1 ), bool DisableScrolling = false);


        /** Changes the movability of the editor camera */
        void SetEditorCameraEnabled(bool bNewEnable);
        
    protected:

        static FInlineString GetToolWindowName(char const* ToolWindowName, ImGuiID InDockspaceID)
        {
            Assert(ToolWindowName != nullptr);
            return { FInlineString::CtorSprintf(), "%s##%08X", ToolWindowName, InDockspaceID };
        }

        /** Draw a help menu for this tool */
        virtual void DrawHelpMenu(const FUpdateContext& UpdateContext) { DrawHelpTextRow("No Help Available", ""); }
        
        /** Helper to add a simple entry to the help menu */
        void DrawHelpTextRow(const char* pLabel, const char* pText) const;
        
        void SetDisplayName(FString NewName);

    protected:

        ImGuiID                         ID = 0;                 // Document identifier (unique)
        
        ImGuiID                         CurrDockID = 0;
        ImGuiID                         DesiredDockID = 0;      // The dock we wish to be in
        ImGuiID                         CurrLocationID = 0;     // Current Dock node we are docked into _OR_ window ID if floating window
        ImGuiID                         PrevLocationID = 0;     // Previous dock node we are docked into _OR_ window ID if floating window
        ImGuiID                         CurrDockspaceID = 0;    // Dockspace ID ~~ Hash of LocationID + DocType (with MYEDITOR_CONFIG_SAME_LOCATION_SHARE_LAYOUT=1)
        ImGuiID                         PrevDockspaceID = 0;
        ImGuiWindowClass                ToolWindowsClass;       // All our tools windows will share the same WindowClass (based on ID) to avoid mixing tools from different top-level editor

        const IEditorToolContext*       ToolContext = nullptr;
        FString                         ToolName;
        
        TVector<FToolWindow*>           ToolWindows;
        
        FScene*                         Scene = nullptr;
        Entity                          EditorEntity;
        ImTextureID                     SceneViewportTexture = nullptr;

        EEditorToolFlags                ToolFlags = EEditorToolFlags::Tool_WantsToolbar;

        bool                            bViewportFocused = false;
        bool                            bViewportHovered = false;
    };
    
}

#define LUMINA_EDITOR_TOOL( TypeName ) \
constexpr static char const* const s_uniqueTypeName = #TypeName;\
constexpr static uint32_t const s_toolTypeID = Lumina::Hash::FNV1a::GetHash32( #TypeName );\
constexpr static bool const s_isSingleton = false; \
virtual char const* GetUniqueTypeName() const override { return s_uniqueTypeName; }\
virtual uint32_t GetUniqueTypeID() const override final { return TypeName::s_toolTypeID; }

//-------------------------------------------------------------------------

#define LUMINA_SINGLETON_EDITOR_TOOL( TypeName ) \
constexpr static char const* const s_uniqueTypeName = #TypeName;\
constexpr static uint32_t const s_toolTypeID = Lumina::Hash::FNV1a::GetHash32( #TypeName ); \
constexpr static bool const s_isSingleton = true; \
virtual char const* GetUniqueTypeName() const { return s_uniqueTypeName; }\
virtual uint32_t GetUniqueTypeID() const override final { return TypeName::s_toolTypeID; }\
virtual bool IsSingleton() const override final { return true; }