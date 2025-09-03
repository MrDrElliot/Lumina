#include "ImGuiRenderer.h"

#include "ImGuiDesignIcons.h"
#include "ImGuiFonts.h"
#include "ImGuizmo.h"
#include "Core/Engine/Engine.h"
#include "imgui/misc/freetype/imgui_freetype.h"
#include "Renderer/RHIIncl.h"
#include "TaskSystem/TaskSystem.h"
#include "Tools/UI/Fonts/FontData_Lexend.h"
#include "Tools/UI/Fonts/FontData_MaterialDesign.h"
#include "Tools/UI/Fonts/FontDecompressor.h"
#include "Tools/UI/Notification/ImGuiNotifications.h"

namespace Lumina
{
	static void* ImGuiMemAlloc(size_t size, void* user_data)
	{
		return Memory::Malloc(size);
	}

	static void ImGuiMemFree(void* ptr, void* user_data)
	{
		Memory::Free(ptr);
	}

	
    void IImGuiRenderer::Initialize()
    {

        IMGUI_CHECKVERSION();
		
		ImGui::SetAllocatorFunctions(ImGuiMemAlloc, ImGuiMemFree, nullptr);
		
        ImGuiContext* Context = ImGui::CreateContext();
    	ImGuizmo::SetImGuiContext(Context);
    	
        ImGuiIO& io = ImGui::GetIO();

    	TVector<uint8> FontData, BoldFontData;

    	Fonts::GetDecompressedFontData(Fonts::Lexend::Regular::GetData(), FontData);
		Fonts::GetDecompressedFontData(Fonts::Lexend::Bold::GetData(), BoldFontData);	

		constexpr ImWchar icons_ranges[] = { LE_ICONRANGE_MIN, LE_ICONRANGE_MAX, 0 };
    	TVector<uint8> iconFontData;
    	Fonts::GetDecompressedFontData(Fonts::MaterialDesignIcons::GetData(), iconFontData);

    	ImFontConfig fontConfig;
    	fontConfig.FontDataOwnedByAtlas = false;

    	ImFontConfig iconFontConfig;
    	iconFontConfig.FontDataOwnedByAtlas = false;
    	iconFontConfig.FontLoaderFlags = ImGuiFreeTypeLoaderFlags_LoadColor | ImGuiFreeTypeLoaderFlags_Bitmap;
    	iconFontConfig.MergeMode = true;
    	iconFontConfig.PixelSnapH = true;
    	iconFontConfig.RasterizerMultiply = 1.5f;

    	
    	auto CreateFont = [&] ( Blob& fontData, float fontSize, float iconFontSize, ImGuiX::Font::EFont fontID, char const* pName, ImVec2 const& glyphOffset )
    	{
    		ImFont* pFont = io.Fonts->AddFontFromMemoryTTF( fontData.data(), static_cast<int32_t>(fontData.size()), fontSize, &fontConfig );
		    ImGuiX::Font::GFonts[static_cast<uint8>(fontID)] = pFont;

    		iconFontConfig.GlyphOffset = glyphOffset;
    		iconFontConfig.GlyphMinAdvanceX = iconFontSize;
    		io.Fonts->AddFontFromMemoryTTF( iconFontData.data(), static_cast<int32_t>(iconFontData.size()), iconFontSize, &iconFontConfig, icons_ranges );
    	};

    	constexpr float DPIScale = 1.0f;
    	float const size12 = std::floor( 12 * DPIScale );
    	float const size14 = std::floor( 14 * DPIScale );
    	float const size16 = std::floor( 16 * DPIScale );
    	float const size18 = std::floor( 18 * DPIScale );
    	float const size24 = std::floor( 24 * DPIScale );
    	
    	CreateFont(FontData, size12, size14, ImGuiX::Font::EFont::Tiny, "Tiny", ImVec2( 0, 2 ) );
    	CreateFont(BoldFontData, size12, size14, ImGuiX::Font::EFont::TinyBold, "Tiny Bold", ImVec2( 0, 2 ) );

    	CreateFont(FontData, size14, size16, ImGuiX::Font::EFont::Small, "Small", ImVec2( 0, 2 ) );
    	CreateFont(BoldFontData, size14, size16, ImGuiX::Font::EFont::SmallBold, "Small Bold", ImVec2( 0, 2 ) );

    	CreateFont(FontData, size16, size18, ImGuiX::Font::EFont::Medium, "Medium", ImVec2( 0, 2 ) );
    	CreateFont(BoldFontData, size16, size18, ImGuiX::Font::EFont::MediumBold, "Medium Bold", ImVec2( 0, 2 ) );

    	CreateFont(FontData, size24, size24, ImGuiX::Font::EFont::Large, "Large", ImVec2( 0, 2 ) );
    	CreateFont(BoldFontData, size24, size24, ImGuiX::Font::EFont::LargeBold, "Large Bold", ImVec2( 0, 2 ) );

    	io.Fonts->TexMinWidth = 4096;

    	using namespace ImGuiX::Font;
    	io.FontDefault = GFonts[static_cast<uint8>(EFont::Medium)];
    	
        Assert(GFonts[(uint8)EFont::Small]->IsLoaded())
        Assert(GFonts[(uint8)EFont::SmallBold]->IsLoaded())
        Assert(GFonts[(uint8)EFont::Medium]->IsLoaded())
        Assert(GFonts[(uint8)EFont::MediumBold]->IsLoaded())
        Assert(GFonts[(uint8)EFont::Large]->IsLoaded())
        Assert(GFonts[(uint8)EFont::LargeBold]->IsLoaded())
		
    	
    	io.ConfigWindowsMoveFromTitleBarOnly = true;
    	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    	io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;
    	io.ConfigViewportsNoDefaultParent = true;

        ImGui::StyleColorsDark();
        ImGuiStyle& style = ImGui::GetStyle();
    	
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}
    	
        style.Alpha = 1.0f;
        style.Colors[ImGuiCol_Text] =                   ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        style.Colors[ImGuiCol_TextDisabled] =           ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        style.Colors[ImGuiCol_WindowBg] =               ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
        style.Colors[ImGuiCol_ChildBg] =                ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
        style.Colors[ImGuiCol_PopupBg] =                ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
        style.Colors[ImGuiCol_Border] =                 ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
        style.Colors[ImGuiCol_BorderShadow] =           ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        style.Colors[ImGuiCol_FrameBg] =                ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
        style.Colors[ImGuiCol_FrameBgHovered] =         ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
        style.Colors[ImGuiCol_FrameBgActive] =          ImVec4(0.37f, 0.37f, 0.37f, 0.39f);
        style.Colors[ImGuiCol_TitleBg] =                ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
        style.Colors[ImGuiCol_TitleBgActive] =          ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
        style.Colors[ImGuiCol_TitleBgCollapsed] =       ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
        style.Colors[ImGuiCol_MenuBarBg] =              ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarBg] =            ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
        style.Colors[ImGuiCol_ScrollbarGrab] =          ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarGrabHovered] =   ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarGrabActive] =    ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
        style.Colors[ImGuiCol_CheckMark] =              ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
        style.Colors[ImGuiCol_SliderGrab] =             ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
        style.Colors[ImGuiCol_SliderGrabActive] =       ImVec4(0.08f, 0.50f, 0.72f, 1.00f);
        style.Colors[ImGuiCol_Button] =                 ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        style.Colors[ImGuiCol_ButtonHovered] =          ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
        style.Colors[ImGuiCol_ButtonActive] =           ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
        style.Colors[ImGuiCol_Header] =                 ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
        style.Colors[ImGuiCol_HeaderHovered] =          ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        style.Colors[ImGuiCol_HeaderActive] =           ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
        style.Colors[ImGuiCol_Separator] =              style.Colors[ImGuiCol_Border];
        style.Colors[ImGuiCol_SeparatorHovered] =       ImVec4(0.41f, 0.42f, 0.44f, 1.00f);
        style.Colors[ImGuiCol_SeparatorActive] =        ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
        style.Colors[ImGuiCol_ResizeGrip] =             ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        style.Colors[ImGuiCol_ResizeGripHovered] =      ImVec4(0.29f, 0.30f, 0.31f, 0.67f);
        style.Colors[ImGuiCol_ResizeGripActive] =       ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
        style.Colors[ImGuiCol_Tab] =                    ImVec4(0.08f, 0.08f, 0.09f, 0.83f);
        style.Colors[ImGuiCol_TabHovered] =             ImVec4(0.33f, 0.34f, 0.36f, 0.83f);
        style.Colors[ImGuiCol_TabSelected] =            ImVec4(0.23f, 0.23f, 0.24f, 1.00f);
        style.Colors[ImGuiCol_TabDimmed] =				ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
        style.Colors[ImGuiCol_TabDimmedSelected] =		ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
        style.Colors[ImGuiCol_DockingPreview] =         ImVec4(0.26f, 0.49f, 0.28f, 0.70f);
        style.Colors[ImGuiCol_DockingEmptyBg] =         ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
        style.Colors[ImGuiCol_PlotLines] =              ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
        style.Colors[ImGuiCol_PlotLinesHovered] =       ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        style.Colors[ImGuiCol_PlotHistogram] =          ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        style.Colors[ImGuiCol_PlotHistogramHovered] =   ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
        style.Colors[ImGuiCol_TextSelectedBg] =         ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
        style.Colors[ImGuiCol_DragDropTarget] =         ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
        style.Colors[ImGuiCol_NavCursor] =				ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        style.Colors[ImGuiCol_NavWindowingHighlight] =  ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        style.Colors[ImGuiCol_NavWindowingDimBg] =      ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        style.Colors[ImGuiCol_ModalWindowDimBg] =       ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
    	style.Colors[ImGuiCol_CheckMark]        		= ImVec4(0.25f, 1.0f, 0.25f, 1.0f);

    	
        style.GrabRounding = style.FrameRounding =      2.3f;

    	style.FramePadding = ImVec2( 6, 6 );
    	style.WindowPadding = ImVec2( 8, 8 );
    	style.ChildBorderSize = 0.0f;
    	style.TabBorderSize = 1.0f;
    	style.GrabRounding = 0.0f;
    	style.GrabMinSize = 8.0f;
    	style.WindowRounding = 0.0f;
    	style.WindowBorderSize = 1.0f;
    	style.FrameRounding = 3.0f;
    	style.IndentSpacing = 8;
    	style.ItemSpacing = ImVec2( 4, 6 );
    	style.TabRounding = 6.0f;
    	style.ScrollbarSize = 20.0f;
    	style.ScrollbarRounding = 0.0f;
    	style.CellPadding = ImVec2( 4, 6 );
    }

    void IImGuiRenderer::Deinitialize()
    {
    	ImGui::DestroyContext();
    }

    void IImGuiRenderer::StartFrame(const FUpdateContext& UpdateContext)
    {
    	OnStartFrame(UpdateContext);
    }

    void IImGuiRenderer::EndFrame(const FUpdateContext& UpdateContext)
    {
		
    	ImGuiIO& Io = ImGui::GetIO();
    	Io.DisplaySize.x = (float)GEngine->GetEngineViewport()->GetSize().X;
    	Io.DisplaySize.y = (float)GEngine->GetEngineViewport()->GetSize().Y;

    	ImGuiX::Notifications::Render();
		ImGui::Render();

    	ImGuiIO& io = ImGui::GetIO();
    	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    	{
    		ImGui::UpdatePlatformWindows();
    		ImGui::RenderPlatformWindowsDefault();
    	}
    	
    	OnEndFrame(UpdateContext);
    }
}
