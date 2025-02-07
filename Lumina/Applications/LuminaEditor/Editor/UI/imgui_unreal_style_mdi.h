
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"

//-----------------------------------------------------------------------------------
// DEMONSTRATE UNREAL-LIKE MULTI DOCUMENT SYSTEM WITH LAYOUT PER EDITOR-TYPE
// Call MyEditor_ShowDemo(MyEditor* editor_instance) to test.
//-----------------------------------------------------------------------------------

//#include <stdio.h>
//#include "../imgui-wip/imgui_debug.h"

#define _CRT_SECURE_NO_WARNINGS
#ifdef _MSC_VER
#pragma warning (disable: 4996) // warning C4996: sprintf,sscanf: This function or variable may be unsafe.
#endif

#include "imgui_internal.h"

// Configuration
#define MYEDITOR_CONFIG_SAME_LOCATION_SHARE_LAYOUT     1

// We need differentiate top level documents/editor windows from individual tool windows.
// For this purpose we push pink colors for title bars and tabs.
// FIXME: But merging two top levels into a floating dock node won't inherit this because the floating node are submitted by NewFrame().
static void PushDocumentColors()
{
#if 0
    ImVec4* colors = ImGui::GetStyle().Colors;
    ImGui::PushStyleColor(ImGuiCol_TitleBg,             ImVec4(0.04f + 0.40f, 0.14f, 0.24f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive,       ImVec4(0.16f + 0.50f, 0.29f, 0.48f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed,    ImVec4(0.04f + 0.40f, 0.14f, 0.24f, 1.00f));

    ImGui::PushStyleColor(ImGuiCol_Tab,                 ImLerp(ImVec4(0.26f + 0.40f, 0.59f, 0.98f, 0.31f),  colors[ImGuiCol_TitleBgActive], 0.80f));
    ImGui::PushStyleColor(ImGuiCol_TabActive,           ImLerp(ImVec4(0.26f + 0.40f, 0.59f, 0.98f, 1.00f),  colors[ImGuiCol_TitleBgActive], 0.60f));
    ImGui::PushStyleColor(ImGuiCol_TabUnfocused,        ImLerp(colors[ImGuiCol_Tab],          colors[ImGuiCol_TitleBg], 0.80f));
    ImGui::PushStyleColor(ImGuiCol_TabUnfocusedActive,  ImLerp(colors[ImGuiCol_TabActive],    colors[ImGuiCol_TitleBg], 0.40f));
    ImGui::PushStyleColor(ImGuiCol_TabHovered,          ImVec4(0.26f + 0.40f, 0.59f, 0.98f, 0.80f));
#endif
}

static void PopDocumentColors()
{
#if 0
    ImGui::PopStyleColor(3+5);
#endif
}

enum MyEditorDocType
{
    MyEditorType_None,
    MyEditorType_Scene,
    MyEditorType_Mesh
};

enum MyEditorToolType
{
    MyEditorToolType_Log,
    MyEditorToolType_Mesh,
    MyEditorToolType_Properties,
    MyEditorToolType_Extra,
    MyEditorToolType_Count_
};

static const char* g_EditorToolNames[] =
{
    "Log",
    "Mesh",
    "Properties",
    "Extra"
};

struct MyEditorDoc
{
    MyEditorDocType Type;
    const char*     Name;               // Document name
    ImGuiID         ID;                 // Document identifier (unique)
    ImGuiID         SetDockID;
    bool            Open;

    ImGuiID         CurrDockID;
    ImGuiID         CurrLocationID;     // Current Dock node we are docked into _OR_ window ID if floating window
    ImGuiID         PrevLocationID;     // Previous dock node we are docked into _OR_ window ID if floating window
    ImGuiID         CurrDockspaceID;    // Dockspace ID ~~ Hash of LocationID + DocType (with MYEDITOR_CONFIG_SAME_LOCATION_SHARE_LAYOUT=1)
    ImGuiID         PrevDockspaceID;
    ImGuiWindowClass ToolWindowsClass;  // All our tools windows will share the same WindowClass (based on ID) to avoid mixing tools from different top-level editor

    // Visibility
    bool            ToolAvail[MyEditorToolType_Count_];
    bool            ToolVisible[MyEditorToolType_Count_];

    // Dummy state
    ImVec4          DummyStateColor;

    MyEditorDoc(MyEditorDocType type, const char* name)
    {
        Type = type;
        ID = ImHashStr(name, 0);
        SetDockID = 0;
        Name = name;
        Open = true;

        CurrDockID = 0;
        CurrLocationID = PrevLocationID = 0;
        CurrDockspaceID = PrevDockspaceID = 0;
        for (int n = 0; n < MyEditorToolType_Count_; n++)
            ToolAvail[n] = ToolVisible[n] = false;
        ResetToolsVisibility();

        DummyStateColor = ImVec4(0.5f, 0.0f, 0.5f, 1.0f);
    }

    void ResetToolsVisibility()
    {
        for (int n = 0; n < MyEditorToolType_Count_; n++)
            ToolAvail[n] = true;

        ToolAvail[MyEditorToolType_Log] = true;
        ToolAvail[MyEditorToolType_Mesh] = (Type == MyEditorType_Mesh);
        ToolAvail[MyEditorToolType_Properties] = true;
        ToolAvail[MyEditorToolType_Extra] = true;

        // FIXME: Visibility data instead of being stored per document could/should be stored keyed by dockspace/layout
        ToolVisible[MyEditorToolType_Log] = (Type == MyEditorType_Scene);
        ToolVisible[MyEditorToolType_Mesh] = (Type == MyEditorType_Mesh);
        ToolVisible[MyEditorToolType_Properties] = true;
        ToolVisible[MyEditorToolType_Extra] = false;
    }

    void CopyToolsVisibilityFrom(const MyEditorDoc* other)
    {
        memcpy(ToolVisible, other->ToolVisible, sizeof(ToolVisible));
    }

    ImGuiID CalcDockspaceID() const
    {
        ImU32 s = 0;
#if MYEDITOR_CONFIG_SAME_LOCATION_SHARE_LAYOUT
        // Shared dockspace per-document type in a same tab-bar
        // - Pros: Always sync by default.
        // - Cons: Ambiguous/weird merging/re-docking.
        s = CurrLocationID;
        s = ImHashData(&Type, sizeof(Type), s);
#else
        // Unique dockspace per document
        // - Cons: Not sync by default. Create more windows/data/garbage.
        s = ID;
        s = ImHashStr("Dockspace", 0, s);
#endif
        return s;
    }
};

static char*   MyEditor_GetToolWindowName(MyEditorToolType tool_type, ImGuiID dockspace_id, char* buf, int buf_size)
{
    IM_ASSERT(tool_type >= 0 && tool_type < MyEditorToolType_Count_);
    const char* tool_name = g_EditorToolNames[tool_type];
    ImFormatString(buf, buf_size, "%s##%08X", tool_name, dockspace_id);
    return buf;
}

// Dummy documents that are possible to open (~ conceptually this is a virtual file-system for the demo/test)
struct MyDummyAvailableDocument
{
    const char*     Name;
    MyEditorDocType Type;
    bool            OpenByDefault;
};
static const MyDummyAvailableDocument g_DummyAvailDocs[] =
{
    // We don't have an easy way to identify Doc vs Tool tab without icon/style modification so we're using uppercase for that as a workaround.
    { "SCENE 1",    MyEditorType_Scene,   true },
    { "SCENE 2",    MyEditorType_Scene,   true },
    { "SCENE 3",    MyEditorType_Scene,   true },
    { "MESH 1",     MyEditorType_Mesh,    false },
    { "MESH 2",     MyEditorType_Mesh,    false },
    { "MESH 3",     MyEditorType_Mesh,    false },
};

struct MyEditor
{
    bool                    Init;
    ImVector<MyEditorDoc*>  Docs;
    ImVector<MyEditorDoc*>  DocsToOpen;
    MyEditorDoc*            LastActiveDocument;
    ImGuiWindowClass        TopLevelEditorWindowClass;
    ImVector<ImGuiID>       TopLevelDockNodeIds;                            // Track active top level docking node to add a + in them
    bool                    OptFloatingToolsOnlyVisibleWhenParentIsFocused; // Option: When multiple documents are open, floating tools only appear for focused one.

    MyEditor()
    {
        Init = true;
        LastActiveDocument = NULL;
        OptFloatingToolsOnlyVisibleWhenParentIsFocused = true;

        // Unique identifier to match/filter top level editors with the top level dockspace. This could be a user enum.
        TopLevelEditorWindowClass.ClassId = ImHashStr("MY_TOPLEVEL_EDITOR", 0);
        TopLevelEditorWindowClass.ViewportFlagsOverrideSet = ImGuiViewportFlags_NoAutoMerge;
        TopLevelEditorWindowClass.ViewportFlagsOverrideClear = ImGuiViewportFlags_NoDecoration | ImGuiViewportFlags_NoTaskBarIcon;
        TopLevelEditorWindowClass.ParentViewportId = 0; // Top level window
        TopLevelEditorWindowClass.DockingAllowUnclassed = false;
        TopLevelEditorWindowClass.DockingAlwaysTabBar = true;
    }

    MyEditorDoc*    OpenDocument(MyEditorDocType type, const char* name_literal, ImGuiID dock_id)
    {
        MyEditorDoc* doc = IM_NEW(MyEditorDoc)(type, name_literal);
        DocsToOpen.push_back(doc);
        doc->SetDockID = dock_id;
        return doc;
    }

    int             GetDocumentsCount() const
    {
        return Docs.Size;
    }

    MyEditorDoc*    FindDocumentByName(const char* name)
    {
        for (int n = 0; n < Docs.Size; n++)
            if (strcmp(name, Docs[n]->Name) == 0)
                return Docs[n];
        return NULL;
    }
};

enum MyEditorLayoutPreset
{
    MyEditorLayoutPreset_Flat,
    MyEditorLayoutPreset_Standard
};

static void MyEditor_LayoutPreset(MyEditorDoc* doc, ImGuiID dockspace_id, ImVec2 dockspace_size, MyEditorLayoutPreset preset, ImGuiDir panel_dir)
{
    doc->ResetToolsVisibility();

    // FIXME: Even though the call to DockBuilderRemoveNodeChildNodes will remove child nodes and move all their child windows to the root of the dockspace,
    // we won't be catching floating windows (e.g. any Tool Window moved outside) so we need to re-dock them explicitly..
    // Perhaps we could store some sort of link between those windows and our dockspace. Could this link be stored on imgui side?
    // Perhaps actually iterate all windows/settings (to find our dockspace_id marker in the name) and setup things based on that?
    // Until then we need an explicit knowledge of our windows.
    char buf[128];
    int buf_size = IM_ARRAYSIZE(buf);

    if (preset == MyEditorLayoutPreset_Flat)
    {
        ImGuiID dock_main_id = dockspace_id;
        ImGui::DockBuilderRemoveNodeChildNodes(dock_main_id);
        ImGui::DockBuilderDockWindow(MyEditor_GetToolWindowName(MyEditorToolType_Log,          doc->CurrDockspaceID, buf, buf_size), dock_main_id);
        ImGui::DockBuilderDockWindow(MyEditor_GetToolWindowName(MyEditorToolType_Properties,   doc->CurrDockspaceID, buf, buf_size), dock_main_id);
        ImGui::DockBuilderDockWindow(MyEditor_GetToolWindowName(MyEditorToolType_Mesh,         doc->CurrDockspaceID, buf, buf_size), dock_main_id);
        ImGui::DockBuilderDockWindow(MyEditor_GetToolWindowName(MyEditorToolType_Extra,        doc->CurrDockspaceID, buf, buf_size), dock_main_id);
        ImGui::DockBuilderFinish(dockspace_id);
    }

    if (preset == MyEditorLayoutPreset_Standard)
    {
        IM_ASSERT(panel_dir == ImGuiDir_Left || panel_dir == ImGuiDir_Right);

        ImGuiID dock_main_id = dockspace_id;
        ImGui::DockBuilderAddNode(dock_main_id, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dock_main_id, dockspace_size);
        ImGuiID dock_id_prop = ImGui::DockBuilderSplitNode(dock_main_id, panel_dir, 0.20f, NULL, &dock_main_id);
        ImGuiID dock_id_bottom = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.20f, NULL, &dock_main_id);

        ImGui::DockBuilderDockWindow(MyEditor_GetToolWindowName(MyEditorToolType_Log,          doc->CurrDockspaceID, buf, buf_size), dock_id_bottom);
        ImGui::DockBuilderDockWindow(MyEditor_GetToolWindowName(MyEditorToolType_Properties,   doc->CurrDockspaceID, buf, buf_size), dock_id_prop);
        ImGui::DockBuilderDockWindow(MyEditor_GetToolWindowName(MyEditorToolType_Mesh,         doc->CurrDockspaceID, buf, buf_size), dock_id_prop);
        ImGui::DockBuilderDockWindow(MyEditor_GetToolWindowName(MyEditorToolType_Extra,        doc->CurrDockspaceID, buf, buf_size), dock_id_prop);
        ImGui::DockBuilderFinish(dockspace_id);
    }
}

static void MyEditor_LayoutCopy(ImGuiID src_dockspace_id, ImGuiID dst_dockspace_id)
{
    IM_ASSERT(src_dockspace_id != 0);
    IM_ASSERT(dst_dockspace_id != 0);

    // Helper to build an array of strings pointer into the same contiguous memory buffer.
    struct ContiguousStringArrayBuilder
    {
        ImVector<char>  Buf;
        ImVector<int>   Offsets;

        void AddEntry(const char* data, size_t data_len)
        {
            Offsets.push_back(Buf.Size);
            int offset = Buf.Size;
            Buf.resize(Buf.Size + (int)data_len);
            memcpy(Buf.Data + offset, data, data_len);
        }
        void BuildPointerArray(ImVector<const char*>* out_array)
        {
            out_array->resize(Offsets.Size);
            for (int n = 0; n < Offsets.Size; n++)
                (*out_array)[n] = Buf.Data + Offsets[n];
        }
    };

    // Build an array of remapped names
    ContiguousStringArrayBuilder name_pairs_builder;
#if 1
    // Iterate all known tools
    name_pairs_builder.Buf.resize(50 * MyEditorToolType_Count_);
    for (int tool_n = 0; tool_n < MyEditorToolType_Count_; tool_n++)
    {
        char window_name[128];
        MyEditor_GetToolWindowName((MyEditorToolType)tool_n, src_dockspace_id, window_name, IM_ARRAYSIZE(window_name));
        name_pairs_builder.AddEntry(window_name, strlen(window_name) + 1);
        MyEditor_GetToolWindowName((MyEditorToolType)tool_n, dst_dockspace_id, window_name, IM_ARRAYSIZE(window_name));
        name_pairs_builder.AddEntry(window_name, strlen(window_name) + 1);
    }
#else
    // Iterate all possible windows, scan for ##XXXXXXXX marker and compare to our source dockspace id
    ImGuiContext& g = *GImGui;
    for (int window_n = 0; window_n < g.Windows.Size; window_n++)
    {
        ImGuiWindow* window = g.Windows[window_n];
        size_t window_name_len = strlen(window->Name);
        ImGuiID window_suffix = 0;
        if (window_name_len > 10 && strncmp(window->Name + window_name_len - 10, "##", 2) == 0)
            if (sscanf(window->Name + window_name_len - 8, "%08X", &window_suffix) == 1 && window_suffix == src_dockspace_id)
            {
                name_pairs_builder.AddEntry(window->Name, window_name_len + 1);
                name_pairs_builder.AddEntry(window->Name, window_name_len + 1);
                ImFormatString(name_pairs_builder.Buf.Data + name_pairs_builder.Offsets.back() + window_name_len - 8, 8 + 1, "%08X", dst_dockspace_id);
            }
    }
#endif

    // Build the same array with char* pointers at it is the input of DockBuilderCopyDockspace() (may change its signature?)
    ImVector<const char*> window_remap_pairs;
    name_pairs_builder.BuildPointerArray(&window_remap_pairs);

    // Perform the cloning
    ImGui::DockBuilderCopyDockSpace(src_dockspace_id, dst_dockspace_id, &window_remap_pairs);
    ImGui::DockBuilderFinish(dst_dockspace_id);
}

static void MyEditor_OpenDocumentSelector(MyEditor* editor)
{
    int avail_count = 0;
    for (int n = 0; n < IM_ARRAYSIZE(g_DummyAvailDocs); n++)
        if (editor->FindDocumentByName(g_DummyAvailDocs[n].Name) == NULL)
        {
            if (ImGui::MenuItem(g_DummyAvailDocs[n].Name))
            {
                ImGuiID default_dock_id = editor->LastActiveDocument ? editor->LastActiveDocument->CurrDockID : 0;
                editor->OpenDocument(g_DummyAvailDocs[n].Type, g_DummyAvailDocs[n].Name, default_dock_id);
            }
            avail_count++;
        }
    if (avail_count == 0)
        ImGui::TextDisabled("<None>");
}

// doc may be NULL here (we have a special case for when there are no document)
static void MyEditor_TopLevelMenuBar(MyEditor* editor, MyEditorDoc* doc, ImGuiID dockspace_id, ImVec2 dockspace_size)
{
    if (!ImGui::BeginMenuBar())
        return;

    if (ImGui::BeginMenu("File"))
    {
        ImGui::MenuItem("New", "Ctrl+N");
        ImGui::MenuItem("Open", "Ctrl+O");
        if (ImGui::BeginMenu("Open Recent"))
        {
            MyEditor_OpenDocumentSelector(editor);
            ImGui::EndMenu();
        }
        ImGui::MenuItem("Save", "Ctrl+S");
        ImGui::MenuItem("Save As", "Ctrl+Shift+S");
        ImGui::Separator();
        ImGui::MenuItem("Close", "Ctrl+W", false, doc != NULL);
        ImGui::Separator();
        ImGui::MenuItem("Exit", "Alt+F4");
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Edit"))
    {
        bool menu_enabled = (doc != NULL);

        // Dummy entries
        ImGui::MenuItem("Undo", "Ctrl+Z", false, menu_enabled);
        ImGui::MenuItem("Redo", "Ctrl+Y", false, menu_enabled);
        ImGui::Separator();
        ImGui::MenuItem("Cut", "Ctrl+X", false, menu_enabled);
        ImGui::MenuItem("Copy", "Ctrl+C", false, menu_enabled);
        ImGui::MenuItem("Paste", "Ctrl+V", false, menu_enabled);
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Windows"))
    {
        bool menu_enabled = (doc != NULL);

        if (doc != NULL)
        {
            for (int n = 0; n < MyEditorToolType_Count_; n++)
                if (doc->ToolAvail[n])
                    ImGui::MenuItem(g_EditorToolNames[n], NULL, &doc->ToolVisible[n], menu_enabled);
            ImGui::Separator();
        }

        if (ImGui::BeginMenu("Layout", menu_enabled))
        {
            if (ImGui::MenuItem("Preset: Flat"))
                MyEditor_LayoutPreset(doc, dockspace_id, dockspace_size, MyEditorLayoutPreset_Flat, ImGuiDir_None);
            if (ImGui::MenuItem("Preset: Default (Left)"))
                MyEditor_LayoutPreset(doc, dockspace_id, dockspace_size, MyEditorLayoutPreset_Standard, ImGuiDir_Left);
            if (ImGui::MenuItem("Preset: Default (Right)"))
                MyEditor_LayoutPreset(doc, dockspace_id, dockspace_size, MyEditorLayoutPreset_Standard, ImGuiDir_Right);
            ImGui::Separator();

            if (ImGui::MenuItem("Apply current layout to all open documents"))
            {
                for (int doc_n = 0; doc_n < editor->Docs.Size; doc_n++)
                {
                    MyEditorDoc* dst_doc = editor->Docs[doc_n];
                    if (dst_doc == doc)
                        continue;
                    dst_doc->CopyToolsVisibilityFrom(doc);
                    MyEditor_LayoutCopy(doc->CurrDockspaceID, dst_doc->CurrDockspaceID);
                }
            }
            ImGui::Separator();

            if (ImGui::MenuItem("*Debug: Clear Layout"))
            {
                doc->ResetToolsVisibility();
                ImGuiID dock_main_id = dockspace_id;
                ImGui::DockBuilderRemoveNodeChildNodes(dock_main_id);
                ImGui::DockBuilderRemoveNodeDockedWindows(dock_main_id);   // Remove everything, including windows we don't know about (e.g. Demo Window)
                ImGui::DockBuilderFinish(dockspace_id);
            }

            if (ImGui::MenuItem("*Debug: Merge all nodes"))
            {
                doc->ResetToolsVisibility();
                ImGuiID dock_main_id = dockspace_id;
                ImGui::DockBuilderRemoveNodeChildNodes(dock_main_id);      // Anything docked into any split child node will be moved to the root of the dockspace.
                ImGui::DockBuilderFinish(dockspace_id);
            }

            ImGui::EndMenu();
        }

        ImGui::EndMenu();
    }

    ImGui::Text("(LastActiveDocument: '%s')", editor->LastActiveDocument ? editor->LastActiveDocument->Name : "NULL");

    ImGui::EndMenuBar();
}

// Demonstrate overriding OS title bar
// For the main viewport core dear imgui would normally not touch it (and leave an application title bar)
// This could probably be done elsewhere and in a better way...
static void MyEditor_UpdatePlatformTitleBar(MyEditor* editor, MyEditorDoc* doc)
{
    IM_UNUSED(editor);

    ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
    ImGuiViewportP* viewport = (ImGuiViewportP*)ImGui::GetWindowViewport();
    if (viewport->PlatformWindowCreated && (viewport->Flags & ImGuiViewportFlags_NoDecoration) == 0 && platform_io.Platform_SetWindowTitle)
    {
        char os_window_title[128];
        ImFormatString(os_window_title, IM_ARRAYSIZE(os_window_title), "%s", doc->Name); // Here one could e.g. strip icons
        platform_io.Platform_SetWindowTitle(viewport, os_window_title);
    }
}

// Submit document so we can retrieve its docking location
static void MyEditor_UpdateDocLocation(MyEditor* editor, MyEditorDoc* doc, ImGuiID toplevel_dockspace_id)
{
    IM_ASSERT(doc != NULL && doc->Open);
    IM_ASSERT(toplevel_dockspace_id != 0);

    // Top level editors can only be docked with each others
    ImGui::SetNextWindowClass(&editor->TopLevelEditorWindowClass);
    if (doc->SetDockID != 0)
    {
        ImGui::SetNextWindowDockID(doc->SetDockID);
        doc->SetDockID = 0;
    }
    else
    {
        ImGui::SetNextWindowDockID(toplevel_dockspace_id, ImGuiCond_FirstUseEver);
    }

    // Create top level editor tab/window
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar;
    PushDocumentColors();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    bool visible = ImGui::Begin(doc->Name, &doc->Open, window_flags);
    ImGui::PopStyleVar();
    PopDocumentColors();

    // Store last focused document
    // This should ideally be a stack so we can handle closure immediately without an awkward gap where LastActiveDocument is NULL.
    if (ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows))
        editor->LastActiveDocument = doc;

    // Override Platform Title Bar
    if (visible && editor->LastActiveDocument == doc)
        MyEditor_UpdatePlatformTitleBar(editor, doc);

    // Set WindowClass based on per-document ID, so tabs from Document A are not dockable in Document B etc. We could be using any ID suiting us, e.g. &doc
    // We also set ParentViewportId to request the platform back-end to set parent/child relationship at the windowing level.
    doc->ToolWindowsClass.ClassId = doc->ID;
    doc->ToolWindowsClass.ViewportFlagsOverrideSet = ImGuiViewportFlags_NoTaskBarIcon | ImGuiViewportFlags_NoDecoration;
    doc->ToolWindowsClass.ParentViewportId = ImGui::GetWindowViewport()->ID; // Make child of the top-level editor window
    doc->ToolWindowsClass.DockingAllowUnclassed = true;

    // Track LocationID change so we can fork/copy the layout data according to where the window is going + reference count
    // LocationID ~~ (DockId != 0 ? DockId : DocumentID) // When we are in a loose floating window we use our own document id instead of the dock id
    doc->CurrDockID = ImGui::GetWindowDockID();
    doc->PrevLocationID = doc->CurrLocationID;
    doc->CurrLocationID = doc->CurrDockID != 0 ? doc->CurrDockID : doc->ID;

    // Dockspace ID ~~ Hash of LocationID + DocType
    // So all editors of a same type inside a same tab-bar will share the same layout.
    // We will also use this value as a suffix to create window titles, but we could perfectly have an indirection to allocate and use nicer names for window names (e.g. 0001, 0002).
    doc->PrevDockspaceID = doc->CurrDockspaceID;
    doc->CurrDockspaceID = doc->CalcDockspaceID();
    IM_ASSERT(doc->CurrDockspaceID != 0);

    // Keep track of top level nodes
    if (doc->CurrDockID != 0 && editor->TopLevelDockNodeIds.contains(doc->CurrDockID) == false)
        editor->TopLevelDockNodeIds.push_back(doc->CurrDockID);

    ImGui::End();
}

// Submit contents and tools for top-level document editor
static void MyEditor_UpdateDocContents(MyEditor* editor, MyEditorDoc* doc)
{
    // This is the second Begin(), as MyEditor_UpdateDocLocationAndLayout() has already done one
    // (Therefore only the p_open and flags of the first call to Begin() applies)
    ImGui::Begin(doc->Name);
    IM_ASSERT(ImGui::GetCurrentWindow()->BeginCount == 2);

    const ImGuiID dockspace_id = doc->CurrDockspaceID;
    const ImVec2 dockspace_size = ImGui::GetContentRegionAvail();

    // Fork settings when extracting to a new location, or Overwrite settings when docking back into an existing location
    if (doc->PrevLocationID != 0 && doc->PrevLocationID != doc->CurrLocationID)
    {
        // Count references to tell if we should Copy or Move the layout.
        int prev_dockspace_ref_count = 0;
        int curr_dockspace_ref_count = 0;
        for (int doc_n = 0; doc_n < editor->Docs.Size; doc_n++)
        {
            MyEditorDoc* other_doc = editor->Docs[doc_n];
            if (other_doc->CurrDockspaceID == doc->PrevDockspaceID)
                prev_dockspace_ref_count++;
            if (other_doc->CurrDockspaceID == doc->CurrDockspaceID)
                curr_dockspace_ref_count++;
        }

        // Fork or overwrite settings
        // FIXME: should be able to do a "move window but keep layout" if curr_dockspace_ref_count > 1.
        // FIXME: when moving, delete settings of old windows
        IMGUI_DEBUG_LOG("LayoutCopy DockID %08X -> DockID %08X requested by doc '%s'\n", doc->PrevDockspaceID, doc->CurrDockspaceID, doc->Name);
        IMGUI_DEBUG_LOG("--> prev_dockspace_ref_count = %d --> %s\n", prev_dockspace_ref_count, (prev_dockspace_ref_count == 0) ? "Remove" : "Keep");
        IMGUI_DEBUG_LOG("--> curr_dockspace_ref_count = %d\n", curr_dockspace_ref_count);
        MyEditor_LayoutCopy(doc->PrevDockspaceID, doc->CurrDockspaceID);

        if (prev_dockspace_ref_count == 0)
        {
            ImGui::DockBuilderRemoveNode(doc->PrevDockspaceID);
#if 0
            // Rely on window name to ditch their .ini settings forever..
            char window_suffix[16];
            ImFormatString(window_suffix, IM_ARRAYSIZE(window_suffix), "##%08X", dockspace_id);
            size_t window_suffix_len = strlen(window_suffix);

            // Mark windows as not saved
            ImGuiContext& g = *GImGui;
            for (ImGuiWindow* window : g.Windows)
            {
                size_t window_name_len = strlen(window->Name);
                if (window_name_len >= window_suffix_len)
                    if (strcmp(window->Name + window_name_len - window_suffix_len, window_suffix) == 0)
                        window->Flags |= ImGuiWindowFlags_NoSavedSettings;
            }

            // Also clear persistent settings (regardless of active windows)
            for (ImGuiWindowSettings* settings = g.SettingsWindows.begin(); settings != NULL; settings = g.SettingsWindows.next_chunk(settings))
            {
                if (settings->ID == 0)
                    continue;
                const char* window_name = settings->GetName();
                size_t window_name_len = strlen(window_name);
                if (window_name_len >= window_suffix_len)
                    if (strcmp(window_name + window_name_len - window_suffix_len, window_suffix) == 0)
                        settings->ID = 0;
            }
#endif
        }
    }
    else if (ImGui::DockBuilderGetNode(doc->CurrDockspaceID) == NULL)
    {
        // Initialize new dockspace with preset
        // (to be able to call DockBuilderSplitNode() we need a node size ahead, so any preset override will probably have to be applied between Begin and Dockspace calls)
        // FIXME: Use g_LastActiveDoc as source reference and do a copy?
        MyEditor_LayoutPreset(doc, dockspace_id, dockspace_size, MyEditorLayoutPreset_Standard, ImGuiDir_Left);
    }

    // FIXME-DOCK: This is a little tricky to explain but we currently need this to use the pattern of sharing a same dockspace between tabs of a same tab bar
    bool visible = true;
#if MYEDITOR_CONFIG_SAME_LOCATION_SHARE_LAYOUT
    if (ImGui::GetCurrentWindow()->Hidden)
        visible = false;
#endif

    if (!visible)
    {
        // Keep alive document dockspace so windows that are docked into it but which visibility are not linked to the dockspace visibility won't get undocked.
        ImGui::DockSpace(dockspace_id, dockspace_size, ImGuiDockNodeFlags_KeepAliveOnly, &doc->ToolWindowsClass);
        ImGui::End();
        return;
    }

    // Submit the menu bar
    MyEditor_TopLevelMenuBar(editor, doc, dockspace_id, dockspace_size);
    //ImGui::Text("IsWindowFocused(_DockHierarchy|_ChidWindows): %d", ImGui::IsWindowFocused(ImGuiFocusedFlags_DockHierarchy | ImGuiFocusedFlags_ChildWindows));

    // Submit the dockspace node
    ImGui::DockSpace(dockspace_id, dockspace_size, ImGuiDockNodeFlags_None, &doc->ToolWindowsClass);
    ImGui::End();

    const bool is_last_focused_doc = (editor->LastActiveDocument == doc);

    // Submit dummy tools
    for (int tool_type = 0; tool_type < MyEditorToolType_Count_; tool_type++)
    {
        if (!doc->ToolAvail[tool_type] || !doc->ToolVisible[tool_type])
            continue;

        char buf[128];
        int buf_size = IM_ARRAYSIZE(buf);
        MyEditor_GetToolWindowName((MyEditorToolType)tool_type, doc->CurrDockspaceID, buf, buf_size);

        if (editor->OptFloatingToolsOnlyVisibleWhenParentIsFocused && !is_last_focused_doc)
            if (ImGuiWindow* window = ImGui::FindWindowByName(buf))
                if (window->DockNode == NULL || ImGui::DockNodeGetRootNode(window->DockNode)->ID != dockspace_id)
                    continue;

        const ImGuiWindowFlags tools_shared_window_flags = ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNavFocus;
        ImGui::SetNextWindowClass(&doc->ToolWindowsClass);
        ImGui::Begin(buf, &doc->ToolVisible[tool_type], 0 | tools_shared_window_flags);

        const char* tool_name = g_EditorToolNames[tool_type];
        ImGui::Text("(%s for \"%s\")", tool_name, doc->Name);
        ImGui::SmallButton("Dummy Button");

        if (tool_type == MyEditorToolType_Log)
            ImGui::Text("Good evening professor. I see you have driven here in your Ferrari.");

        if (tool_type == MyEditorToolType_Properties)
            ImGui::ColorEdit4("Color editor", &doc->DummyStateColor.x, ImGuiColorEditFlags_NoInputs); // This is useful to test because popup viewport parent/child relation may incorrectly affect focus.

        if (ImGui::TreeNode("Tree node"))
        {
            ImGui::Text("..Opened!");
            ImGui::TreePop();
        }
        ImGui::End();
    }
}

static void MyEditor_ShowDemo(MyEditor* editor)
{
    // Open default documents on startup
    if (editor->Init)
    {
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        for (int n = 0; n < IM_ARRAYSIZE(g_DummyAvailDocs); n++)
            if (g_DummyAvailDocs[n].OpenByDefault)
                editor->OpenDocument(g_DummyAvailDocs[n].Type, g_DummyAvailDocs[n].Name, 0);
        editor->Init = false;
    }
    if (editor->DocsToOpen.Size > 0)
    {
        for (auto doc : editor->DocsToOpen)
            editor->Docs.push_back(doc);
        editor->DocsToOpen.resize(0);
    }
    editor->TopLevelDockNodeIds.resize(0);

    // Top Level Dockspace
    ImGuiID root_dockspace_id = 0;
    {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

        ImGuiWindowFlags flags = 0;
        flags |= ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        // When we have open documents, use a window without a title bar with a dockspace inside. Otherwise fake the dockspace tab-bar with a regular title bar.
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        PushDocumentColors();
        int open_count = editor->GetDocumentsCount();
        if (open_count > 0)
            ImGui::Begin("###DocumentRoot", NULL, flags | ImGuiWindowFlags_NoTitleBar);
        else
            ImGui::Begin("No document###DocumentRoot", NULL, flags | ImGuiWindowFlags_MenuBar);
        ImGui::PopStyleVar();

        root_dockspace_id = ImGui::GetID("MyRootDockspace");
        ImGui::DockSpace(root_dockspace_id, ImVec2(0, 0), ImGuiDockNodeFlags_NoSplit | ImGuiDockNodeFlags_AutoHideTabBar, &editor->TopLevelEditorWindowClass);

        //ImGuiDockNode* node = ImGui::DockBuilderGetNode(root_dockspace_id);
        //node->IsHiddenTabBar = true;

        PopDocumentColors();

        if (open_count == 0)
            MyEditor_TopLevelMenuBar(editor, NULL, 0, ImVec2(0, 0));

        ImGui::End();
        ImGui::PopStyleVar();
    }

    // Update the location of all documents
    // Submit Top Level Documents
    for (int n = 0; n < editor->Docs.Size; n++)
    {
        MyEditorDoc* doc = editor->Docs[n];
        MyEditor_UpdateDocLocation(editor, doc, root_dockspace_id);
    }

    // Submit Top Level Documents
    for (int n = 0; n < editor->Docs.Size; n++)
    {
        MyEditorDoc* doc = editor->Docs[n];
        MyEditor_UpdateDocContents(editor, doc);

        // Process closure
        if (doc->Open == false)
        {
            IM_DELETE(doc);
            if (editor->LastActiveDocument == doc)
                editor->LastActiveDocument = NULL;
            editor->Docs.erase(editor->Docs.Data + n);
            n--;
        }
    }

    // Demonstrate appending a + button into tab-bar
    // We collected a list of top-level node into TopLevelDockNodeIds[]
    // We could also iterate g.DockContext->Nodes[] and find active one where node->WindowClass == editor->TopLevelEditorWindowClass...
    for (auto top_level_node_id : editor->TopLevelDockNodeIds)
    {
        ImGuiDockNode* top_level_node = ImGui::DockBuilderGetNode(top_level_node_id);
        if (ImGui::DockNodeBeginAmendTabBar(top_level_node))
        {
            if (ImGui::TabItemButton("+", ImGuiTabItemFlags_Trailing))
                ImGui::OpenPopup("OpenDocument");
            if (ImGui::BeginPopup("OpenDocument"))
            {
                MyEditor_OpenDocumentSelector(editor);
                ImGui::EndPopup();
            }
            ImGui::DockNodeEndAmendTabBar();
        }
    }
}

// Data
static MyEditor g_Editor;

//-----------------------------------------------------------------------------------
// END
//-----------------------------------------------------------------------------------
