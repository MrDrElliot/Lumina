#pragma once
#include <sstream>

#include "Event.h"

namespace Lumina
{
    
    class WindowResizeEvent : public FEvent
    {
    public:
        WindowResizeEvent(unsigned int width, unsigned int height)
            : Width(width), Height(height) {}

        unsigned int GetWidth() const { return Width; }
        unsigned int GetHeight() const { return Height; }

        std::string ToString() const override
        {
            std::stringstream ss;
            ss << "WindowResizeEvent: " << Width << ", " << Height;
            return ss.str();
        }

        EVENT_CLASS_TYPE(WindowResize)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    private:
        unsigned int Width, Height;
    };

    class WindowCloseEvent : public FEvent
    {
    public:
        WindowCloseEvent() = default;

        EVENT_CLASS_TYPE(WindowClose)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };

    class WindowDropEvent : public FEvent
    {
    public:
        WindowDropEvent(int InPathCount, const char* InPaths[])
        {
            PathCount = InPathCount;
            Path = InPaths[0];
        }

        int GetPathCount() { return PathCount; }
        const char* GetPath() { return Path; }
        EVENT_CLASS_TYPE(WindowDrop)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)

    private:

        int PathCount;
        const char* Path;
    };

    class AppTickEvent : public FEvent
    {
    public:
        AppTickEvent() = default;

        EVENT_CLASS_TYPE(AppTick)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };

    class AppUpdateEvent : public FEvent
    {
    public:
        AppUpdateEvent() = default;

        EVENT_CLASS_TYPE(AppUpdate)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };

    class AppRenderEvent : public FEvent
    {
    public:
        AppRenderEvent() = default;

        EVENT_CLASS_TYPE(AppRender)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };
}
