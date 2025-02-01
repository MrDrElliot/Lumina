
#include "SandboxApp.h"

#include "Assets/AssetManager/AssetManager.h"
#include "Assets/AssetRegistry/AssetRegistry.h"

void FSandboxApp::OnUpdate()
{
	
}

void FSandboxApp::OnInit()
{
	FApplication::OnInit();

	Lumina::FAssetManager* Manager = GetSubsystem<Lumina::FAssetManager>();

	//Manager->LoadAsset()
	
}
