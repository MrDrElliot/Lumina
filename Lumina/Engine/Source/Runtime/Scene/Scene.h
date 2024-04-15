#pragma once
#include "Camera.h"
#include "Source/Runtime/Assets/Asset.h"


namespace Lumina
{
    class LScene : public LAsset
    {
    public:
        
        LScene();
        ~LScene();

        void OnUpdate(float DeltaTime);

        FCamera* GetEditorCamera() { return EditorCamera.get(); }
        

    private:

        std::unique_ptr<FCamera> EditorCamera;
        
    };
}
