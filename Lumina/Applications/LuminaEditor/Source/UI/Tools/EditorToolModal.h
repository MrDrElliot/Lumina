#pragma once
#include "imgui.h"
#include "Containers/String.h"
#include "Core/UpdateContext.h"
#include "Containers/Function.h"

namespace Lumina
{
    class FEditorToolModal;
    class FUpdateContext;
}

namespace Lumina
{

    class FEditorModalManager
    {
    public:

        ~FEditorModalManager();

        void CreateModalDialogue(const FString& Title, ImVec2 Size, TFunction<bool(const FUpdateContext&)> DrawFunction);
        void CreateModalDialogue(const FString& Title, ImVec2 Size, FEditorToolModal* Modal);

        FORCEINLINE bool HasModal() const { return ActiveModal != nullptr; }

        void DrawDialogue(const FUpdateContext& UpdateContext);

    private:
        
        FEditorToolModal*       ActiveModal = nullptr;
        
    };
    
    class FEditorToolModal
    {
    public:

        friend class FEditorModalManager;
        
        virtual ~FEditorToolModal() = default;

        FEditorToolModal(const FString& InTitle, ImVec2 InSize)
            : DrawFunction()
            , Title(InTitle)
            , Size(InSize)
        {}

        /** Return true to indicate the modal is ready to close */
        virtual bool DrawModal(const FUpdateContext& UpdateContext) { return DrawFunction(UpdateContext); }
        
    protected:

        TFunction<bool(const FUpdateContext&)>  DrawFunction;
        FString                                 Title;
        ImVec2                                  Size;
    
    };
}
