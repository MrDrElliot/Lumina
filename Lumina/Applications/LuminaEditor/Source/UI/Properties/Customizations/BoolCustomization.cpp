#include "CoreTypeCustomization.h"
#include "imgui.h"

namespace Lumina
{
    void FBoolPropertyCustomization::DrawProperty(TSharedPtr<FPropertyHandle> Property)
    {
        bool* BoolPtr = Property->GetTypePropertyPtr<bool>();

        ImGui::Checkbox("##", BoolPtr);
    }
}
