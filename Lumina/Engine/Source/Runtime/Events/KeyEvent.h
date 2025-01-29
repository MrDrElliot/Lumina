#pragma once
#include <sstream>

#include "Event.h"
#include "KeyCodes.h"

namespace Lumina
{
    class FKeyEvent : public FEvent
    {
    public:
        KeyCode GetKeyCode() const { return keyCode; }

        EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
    protected:
        FKeyEvent(const KeyCode keycode)
            : keyCode(keycode) {}

        KeyCode keyCode;
    };

    class KeyPressedEvent : public FKeyEvent
    {
    public:
        KeyPressedEvent(const KeyCode keycode, bool isRepeat = false)
            : FKeyEvent(keycode), bIsRepeat(isRepeat) {}

        bool IsRepeat() const { return bIsRepeat; }

        FString ToString() const override
        {
            std::stringstream ss;
            ss << "KeyPressedEvent: " << keyCode << " (repeat = " << bIsRepeat << ")";
            return ss.str().c_str();
        }

        EVENT_CLASS_TYPE(KeyPressed)
    private:
        bool bIsRepeat;
    };

    class KeyReleasedEvent : public FKeyEvent
    {
    public:
        KeyReleasedEvent(const KeyCode keycode)
            : FKeyEvent(keycode) {}

        FString ToString() const override
        {
            std::stringstream ss;
            ss << "KeyReleasedEvent: " << keyCode;
            return ss.str().c_str();
        }

        EVENT_CLASS_TYPE(KeyReleased)
    };

    class KeyTypedEvent : public FKeyEvent
    {
    public:
        KeyTypedEvent(const KeyCode keycode)
            : FKeyEvent(keycode) {}

        FString ToString() const override
        {
            std::stringstream ss;
            ss << "KeyTypedEvent: " << keyCode;
            return ss.str().c_str();
        }

        EVENT_CLASS_TYPE(KeyTyped)
    };
}
