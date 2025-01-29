#pragma once
#include <sstream>

#include "Event.h"
#include "MouseCodes.h"

namespace Lumina
{
 	class MouseMovedEvent : public FEvent
	{
	public:
		MouseMovedEvent(const float x, const float y)
			: MouseX(x), MouseY(y) {}

		float GetX() const { return MouseX; }
		float GetY() const { return MouseY; }

		FString ToString() const override
		{
            std::stringstream ss;
			ss << "MouseMovedEvent: " << MouseX << ", " << MouseY;
			return ss.str().c_str();
		}

		EVENT_CLASS_TYPE(MouseMoved)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	private:
		float MouseX, MouseY;
	};

	class MouseScrolledEvent : public FEvent
	{
	public:
		MouseScrolledEvent(const float xOffset, const float yOffset)
			: m_XOffset(xOffset), m_YOffset(yOffset) {}

		float GetXOffset() const { return m_XOffset; }
		float GetYOffset() const { return m_YOffset; }

		FString ToString() const override
		{
            std::stringstream ss;
			ss << "MouseScrolledEvent: " << GetXOffset() << ", " << GetYOffset();
			return ss.str().c_str();
		}

		EVENT_CLASS_TYPE(MouseScrolled)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	private:
		float m_XOffset, m_YOffset;
	};

	class MouseButtonEvent : public FEvent
	{
	public:
		MouseCode GetMouseButton() const { return Button; }

		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput | EventCategoryMouseButton)
	protected:
		MouseButtonEvent(const MouseCode button)
			: Button(button) {}

		MouseCode Button;
	};

	class MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(const MouseCode button)
			: MouseButtonEvent(button) {}

		FString ToString() const override
		{
            std::stringstream ss;
			ss << "MouseButtonPressedEvent: " << Button;
			return ss.str().c_str();
		}

		EVENT_CLASS_TYPE(MouseButtonPressed)
	};

	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(const MouseCode button)
			: MouseButtonEvent(button) {}

		FString ToString() const override
		{
            std::stringstream ss;
			ss << "MouseButtonReleasedEvent: " << Button;
			return ss.str().c_str();
		}

		EVENT_CLASS_TYPE(MouseButtonReleased)
	};   
}
