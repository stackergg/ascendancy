#include "../includes.h"

#include "../UTILS/interfaces.h"
#include "../SDK/CClientEntityList.h"
#include "../SDK/IEngine.h"
#include "../SDK/CBaseEntity.h"
#include "../SDK/CGlobalVars.h"
#include "../SDK/ISurface.h"

#include "../FEATURES/Backtracking.h"
#include "../FEATURES/Resolver.h"
#include "../FEATURES/Visuals.h"

#include "EventListener.h"
#include <playsoundapi.h>

#pragma comment(lib, "Winmm.lib")

CGameEvents::PlayerHurtListener player_hurt_listener;
CGameEvents::BulletImpactListener bullet_impact_listener;

void CGameEvents::InitializeEventListeners()
{
	//INTERFACES::GameEventManager->AddListener(&item_purchase_listener, "item_purchase", false);
	INTERFACES::GameEventManager->AddListener(&player_hurt_listener, "player_hurt", false);
	INTERFACES::GameEventManager->AddListener(&bullet_impact_listener, "bullet_impact", false);
}

void CGameEvents::PlayerHurtListener::FireGameEvent(SDK::IGameEvent* game_event)
{
	if (!game_event)
		return;

	auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
	if (!local_player)
		return;

	auto entity = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetPlayerForUserID(game_event->GetInt("userid")));
	if (!entity)
		return;

	auto entity_attacker = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetPlayerForUserID(game_event->GetInt("attacker")));
	if (!entity_attacker)
		return;

	if (entity->GetTeam() == local_player->GetTeam())
		return;

	SDK::player_info_t player_info;
	if (!INTERFACES::Engine->GetPlayerInfo(entity->GetIndex(), &player_info))
		return;

	if (entity_attacker == local_player)
	{
		hitmarker_time = 255;
		//PlaySound("C:\\stackhack\\hitsound.wav", NULL, SND_ASYNC);
		INTERFACES::Surface->IPlaySound("buttons\\arena_switch_press_02.wav");
		shots_hit[entity->GetIndex()]++;
	}
}

void CGameEvents::BulletImpactListener::FireGameEvent(SDK::IGameEvent* game_event)
{
	if (!game_event)
		return;

	int iUser = INTERFACES::Engine->GetPlayerForUserID(game_event->GetInt("userid"));

	auto entity = reinterpret_cast<SDK::CBaseEntity*>(INTERFACES::ClientEntityList->GetClientEntity(iUser));

	if (!entity)
		return;

	if (entity->GetIsDormant())
		return;

	float x, y, z;
	x = game_event->GetFloat("x");
	y = game_event->GetFloat("y");
	z = game_event->GetFloat("z");

	
	UTILS::BulletImpact_t impact(entity, Vector(x, y, z), INTERFACES::Globals->curtime, iUser == INTERFACES::Engine->GetLocalPlayer() ? GREEN : RED);

	visuals->Impacts.push_back(impact);
}

/*void ItemPurchaseListener::FireGameEvent(SDK::IGameEvent* game_event)
{
	if (!game_event)
		return;

	auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());
	if (!local_player)
		return;

	auto entity = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetPlayerForUserID(game_event->GetInt("userid")));
	if (!entity)
		return;

	if (entity->GetTeam() == local_player->GetTeam())
		return;

	SDK::player_info_t player_info;
	if (!INTERFACES::Engine->GetPlayerInfo(entity->GetIndex(), &player_info))
		return;

	MISC::InGameLogger::Log log;

	log.color_line.PushBack(player_info.name, HOTPINK);
	log.color_line.PushBack(" bought a ", CColor(200, 200, 200));
	log.color_line.PushBack(game_event->GetString("weapon"), HOTPINK);

	MISC::in_game_logger.AddLog(log);
};*/