
#include "ImGuiFonts.h"

namespace Lumina::ImGuiX::Font
{
    ImFont* FSystemFonts::s_fonts[static_cast<int32_t>(EFont::NumFonts)] = { nullptr, nullptr, nullptr, nullptr };
}
