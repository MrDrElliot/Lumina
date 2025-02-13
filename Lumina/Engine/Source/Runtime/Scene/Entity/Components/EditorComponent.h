#pragma once


namespace Lumina
{
    /**
     * Essentially a tag component for specifying an entity as editor functionality
     */
    class FEditorComponent : public FEntityComponent
    {
    public:

        FORCEINLINE bool IsEnabled() const { return bEnabled; }
        FORCEINLINE void SetEnabled(bool bNewEnabled) { bEnabled = bNewEnabled; }


    private:

        bool bEnabled = true;
    };
}
