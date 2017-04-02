#include <string>
#include "PluginSDK.h"
#include <vector>
#include <WinUser.h>



const char* PluginN = "Divine Walker";
const int Version = 1.0;

const Vec3 UP_AXIS = Vec3(0, 0, 1), RIGHT_AXIS = Vec3(1, 0, 0);
const float CooldownT = 0.2;
float LastT;
bool HadManualInput;

IUnit* MyPlayer;


PluginSetup(pluginN);
 
PLUGIN_EVENT(void) OnGameUpdate()
{
	short UpState = GetAsyncKeyState(VK_NUMPAD8);
	short DownState = GetAsyncKeyState(VK_NUMPAD5);
	short LeftState = GetAsyncKeyState(VK_NUMPAD4);
	short RightState = GetAsyncKeyState(VK_NUMPAD6);

	if (HadManualInput && (!UpState && !DownState && !LeftState && !RightState) )
	{
		GGame->IssueOrder(MyPlayer, eGameObjectOrder::kHoldPosition, MyPlayer->ServerPosition());
		HadManualInput = false;
		return;
	}

	if (GGame->Time() - LastT < CooldownT || !GUtility->IsLeagueWindowFocused() || GGame->IsChatOpen()) return;

	LastT = GGame->Time();
	Vec3 CurrPos = MyPlayer->GetPosition();
	Vec3 MoveDest = CurrPos;
	float StepSize = 100 + MyPlayer->MovementSpeed()*CooldownT;
	Vec3 xStep = RIGHT_AXIS*StepSize;
	Vec3 zStep = UP_AXIS*StepSize;

	if (UpState || DownState) MoveDest += UpState?zStep:-zStep;
	if (RightState  || LeftState) MoveDest += RightState ? xStep : -xStep;
		
	if (MoveDest != CurrPos)
	{
		HadManualInput = true;
		MoveDest.y = GNavMesh->GetHeightForPoint(Vec2(MoveDest.x, MoveDest.z));
		GGame->IssueOrder(MyPlayer, eGameObjectOrder::kMoveTo, MoveDest);
	}
}

PLUGIN_API void OnLoad(IPluginSDK* PluginSDK)
{
	PluginSDKSetup(PluginSDK);
	GGame->PrintChat(std::string("[").append(PluginN).append("] Welcome ! (v ").append(std::to_string(Version)).append(")").c_str());
	MyPlayer = GEntityList->Player();
	LastT = GGame->Time();
	GEventManager->AddEventHandler(kEventOnGameUpdate, OnGameUpdate);
}


PLUGIN_API void OnUnload()
{
	GEventManager->RemoveEventHandler(kEventOnGameUpdate, OnGameUpdate);	 
}