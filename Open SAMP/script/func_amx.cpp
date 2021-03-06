#include "../main.h"
#include "../pool/class_pickuppool.h"

#ifndef _STANDALONE
#include "../../SDK/amx/amx.h"
#include "../../SDK/plugincommon.h"
#endif

#include "func_amx.h"

#include "amx/func_amx_element.h"
#include "amx/func_amx_object.h"
#include "amx/func_amx_pickup.h"
#include "amx/func_amx_radarzone.h"
#include "amx/func_amx_radaricon.h"

#include "amx/func_amx_deprecated_gangzone.h"
#include "amx/func_amx_deprecated_object.h"
#include "amx/func_amx_textdraw.h"
#include "amx/func_amx_menu.h"
#include "amx/func_amx_players.h"


#include "format.h"

// Open SA:MP
bool bScriptDebug = false; // please use "scriptdebug" console variable in server.cfg or direct input


/**
 *	Detoured
 */

cell* get_amxaddr(AMX *amx, cell amx_addr);

unsigned char *format_amxstring(AMX *amx, cell *params, int parm, int &len)
{
	static unsigned char outbuf[4096];
	memset(outbuf, 0, 4096);

	cell *addr = get_amxaddr(amx, params[parm++]);
	len = atcprintf((char *)outbuf, sizeof(outbuf)-1, addr, amx, params, &parm);

	return outbuf;
}

int set_amxstring(AMX* amx, cell amx_addr, const char* source, int max)
{
	cell* dest = (cell *)(amx->base + (int)(((AMX_HEADER *)amx->base)->dat + amx_addr));
	cell* start = dest;
	while (max--&&*source)
		*dest++=(cell)*source++;
	*dest = 0;
	return dest-start;
}


cell AMX_NATIVE_CALL funcPrint ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcPrint()" );

	CHECK_PARAMS( 1 );

	char* str;
	amx_StrParam(a_AmxInterface, a_Params[ 1 ], str);
	logprintf("%s", str);

	return 0;
}

cell AMX_NATIVE_CALL funcPrintf ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcPrintf()" );
	//CHECK_PARAMS( 1 ); Paramètres infinies dans cette fonction :p

	int len;
	logprintf("%s", format_amxstring(a_AmxInterface, a_Params, 1, len));

	return 0;
}

cell AMX_NATIVE_CALL funcFormat ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcFormat()" );
	//CHECK_PARAMS( 3 ); fonction infinie aussi lol
	
	int len;

	cell* cstr;
	amx_GetAddr(a_AmxInterface, a_Params[1], &cstr);
	amx_SetString(cstr, (char *)format_amxstring(a_AmxInterface, a_Params, 3, len), 0, 0, (int)a_Params[2]);

	return 1;
}

cell AMX_NATIVE_CALL funcSetTimer ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcSetTimer()" );
	CHECK_PARAMS( 3 );

	char* szFuncName;
	amx_StrParam(a_AmxInterface, a_Params[1], szFuncName);
	return __NetGame->scriptTimerManager->New(szFuncName, a_Params[2], a_Params[3], a_AmxInterface);
}

cell AMX_NATIVE_CALL funcKillTimer ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcKillTimer()" );
	CHECK_PARAMS( 1 );

	__NetGame->scriptTimerManager->Kill(a_Params[1]);
	return 1;
}

cell AMX_NATIVE_CALL funcGetTickCount ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcGetTickCount()" );
	return __NetGame->GetTime();
}

cell AMX_NATIVE_CALL funcGetMaxPlayers ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcGetMaxPlayers()" );

	return __Console->GetIntVar("maxplayers");
}

cell AMX_NATIVE_CALL funcSetTimerEx ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcSetTimerEx()" );
	CHECK_PARAMS( 3 );

	if (a_Params[0] < 4 * sizeof (cell))
	{
		logprintf("SCRIPT: Bad parameter count (%d < 4): ", a_Params[0]);
		return 0;
	}
	else if (a_Params[0] > 260 * sizeof (cell))
	{
		logprintf("SCRIPT: Bad parameter count (%d > 260): ", a_Params[0]);
		return 0;
	}

	char* szFuncName;
	amx_StrParam(a_AmxInterface, a_Params[1], szFuncName);
	return __NetGame->scriptTimerManager->NewEx(szFuncName, a_Params[2], a_Params[3], a_Params, a_AmxInterface);
}

cell AMX_NATIVE_CALL funcLimitGlobalChatRadius ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcLimitGlobalChatRadius()" );
	CHECK_PARAMS( 1 );

	//return _funcLimitGlobalChatRadius ( a_AmxInterface, a_Params );
	__NetGame->limitGlobalChatRadius( amx_ctof( a_Params[1] ) );
	__NetGame->blimitGlobalChatRadius = TRUE;

	return 1;
}

cell AMX_NATIVE_CALL funcLimitPlayerMarkerRadius ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcLimitPlayerMarkerRadius()" );
	CHECK_PARAMS( 1 );

	//return _funcLimitPlayerMarkerRadius ( a_AmxInterface, a_Params );
	__NetGame->limitPlayerMarkerRadius( amx_ctof( a_Params[1] ) );
	__NetGame->bLimitPlayerMarkerRadius = TRUE;

	return 1;
}

cell AMX_NATIVE_CALL funcSetWeather ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcSetWeather()" );
	CHECK_PARAMS( 1 );

	__NetGame->setWeather((BYTE)a_Params[1]);
	return 1;
}

cell AMX_NATIVE_CALL funcSetPlayerWeather ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcSetPlayerWeather()" );
	CHECK_PARAMS( 2 );

	if(!__NetGame->playerPool->GetSlotState((_PlayerID)a_Params[1])) return 0;

	RakNet::BitStream bStream;
	bStream.Write((_PlayerID)a_Params[2]);
	__NetGame->PlayerRPC( RPC_ChangeWeather, &bStream, (_PlayerID)a_Params[1], 2 );
	return 1;
}

cell AMX_NATIVE_CALL funcCallRemoteFunction ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcCallRemoteFunction()" );
	CHECK_PARAMS( 3 );

	if (a_Params[0] < 2 * sizeof (cell))
	{
		logprintf("SCRIPT: Bad parameter count (%d < 2): ", a_Params[0]);
		return 0;
	}
	else if (a_Params[0] > 258 * sizeof (cell))
	{
		logprintf("SCRIPT: Bad parameter count (%d > 258): ", a_Params[0]);
		return 0;
	}

	int iLength;
	char* szFuncName;
	char* szParamList;
	AMX* amxFile;
	bool bFound = false;
	
	amx_StrParam(a_AmxInterface, a_Params[1], szFuncName);
	amx_StrParam(a_AmxInterface, a_Params[2], szParamList);
	if (szParamList == NULL) iLength = 0;
	else iLength = strlen(szParamList);
	
	int idx, i, j;
	cell ret = 0;
	
	for (i = -1; i < MAX_FS; i++)
	{
		if (i == -1)
		{
			if (__NetGame->gamemodeManager != NULL && __NetGame->gamemodeManager->gmIsInit)
				amxFile = &(__NetGame->gamemodeManager->gmAMX);
			else
				amxFile = NULL;
		}
		else
		{
			if (__NetGame->filterscriptsManager != NULL)
				amxFile = __NetGame->filterscriptsManager->fsAMX[i];
			else
				amxFile = NULL;
		}
		if (amxFile != NULL)
		{
			if (!amx_FindPublic(amxFile, szFuncName, &idx))
			{
				cell amx_addr[256];
				j = iLength;
				int iOff = 3, numstr;
				for (numstr = 0; numstr < 16; numstr++)
				{
					amx_addr[numstr] = NULL;
				}
				numstr = 0;
				while (j)
				{
					j--;
					if (*(szParamList + j) == 'a')
					{
						cell *paddr;
						int numcells = *get_amxaddr(a_AmxInterface, a_Params[j + iOff + 1]);
						if (amx_Allot(amxFile, numcells, &amx_addr[numstr], &paddr) == AMX_ERR_NONE)
						{
							memcpy(paddr, get_amxaddr(a_AmxInterface, a_Params[j + iOff]), numcells * sizeof (cell));
							amx_Push(amxFile, amx_addr[numstr]);
							numstr++;
						}
					}
					else if (*(szParamList + j) == 's')
					{
						char* szParamText;
						
						amx_StrParam(a_AmxInterface, a_Params[j + iOff], szParamText);
						if (szParamText != NULL && strlen(szParamText) > 0)
						{
							amx_PushString(amxFile, &amx_addr[numstr], NULL, szParamText, 0, 0);
							numstr++;
						}
						else
						{
							*szParamText = 1;
							*(szParamText + 1) = 0;
							amx_PushString(amxFile, &amx_addr[numstr], NULL, szParamText, 0, 0);
						}
					}
					else
						amx_Push(amxFile, *get_amxaddr(a_AmxInterface, a_Params[j + iOff]));
				}
				amx_Exec(amxFile, &ret, idx);
				while (numstr)
				{
					numstr--;
					amx_Release(amxFile, amx_addr[numstr]);
				}
			}
		}
	}
	return (int)ret;
}

cell AMX_NATIVE_CALL funcCallLocalFunction ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcCallLocalFunction()" );
	CHECK_PARAMS( 2 );

	if (a_Params[0] < 2 * sizeof (cell))
	{
		logprintf("SCRIPT: Bad parameter count (%d < 2): ", a_Params[0]);
		return 0;
	}
	else if (a_Params[0] > 258 * sizeof (cell))
	{
		logprintf("SCRIPT: Bad parameter count (%d > 258): ", a_Params[0]);
		return 0;
	}

	int iLength;
	char* szFuncName;
	char* szParamList;
	bool bFound = false;
	
	amx_StrParam(a_AmxInterface, a_Params[1], szFuncName);
	amx_StrParam(a_AmxInterface, a_Params[2], szParamList);
	if (szParamList == NULL) iLength = 0;
	else iLength = strlen(szParamList);
	
	int idx, j;
	cell ret = 0;
	
	if (!amx_FindPublic(a_AmxInterface, szFuncName, &idx))
	{
		cell amx_addr[256];
		j = iLength;
		int numstr, iOff = 3;
		for (numstr = 0; numstr < 16; numstr++)
		{
			amx_addr[numstr] = NULL;
		}
		numstr = 0;
		while (j)
		{
			j--;
			if (*(szParamList + j) == 'a')
			{
				cell *paddr;
				int numcells = *get_amxaddr(a_AmxInterface, a_Params[j + iOff + 1]);
				if (amx_Allot(a_AmxInterface, numcells, &amx_addr[numstr], &paddr) == AMX_ERR_NONE)
				{
					memcpy(paddr, get_amxaddr(a_AmxInterface, a_Params[j + iOff]), numcells * sizeof (cell));
					amx_Push(a_AmxInterface, amx_addr[numstr]);
					numstr++;
				}
			}
			else if (*(szParamList + j) == 's')
			{
				char* szParamText;
				
				amx_StrParam(a_AmxInterface, a_Params[j + iOff], szParamText);
				if (szParamText != NULL && strlen(szParamText) > 0)
				{
					amx_PushString(a_AmxInterface, &amx_addr[numstr], NULL, szParamText, 0, 0);
					numstr++;
				}
				else
				{
					*szParamText = 1;
					*(szParamText + 1) = 0;
					amx_PushString(a_AmxInterface, &amx_addr[numstr], NULL, szParamText, 0, 0);
				}
			}
			else
			{
				amx_Push(a_AmxInterface, *get_amxaddr(a_AmxInterface, a_Params[j + iOff]));
			}
		}
		amx_Exec(a_AmxInterface, &ret, idx);
		while (numstr)
		{
			numstr--;
			amx_Release(a_AmxInterface, amx_addr[numstr]);
		}
	}
	return (int)ret;
}

cell AMX_NATIVE_CALL funcAsin ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcAsin()" );
	CHECK_PARAMS( 1 );

	float res = (float)(asin(amx_ctof(a_Params[1])) * 180 / PI);
	return amx_ftoc(res);
}

cell AMX_NATIVE_CALL funcAcos ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcAcos()" );
	CHECK_PARAMS( 1 );

	float res = (float)(acos(amx_ctof(a_Params[1])) * 180 / PI);
	return amx_ftoc(res);
}

cell AMX_NATIVE_CALL funcAtan2 ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcAtan2()" );
	CHECK_PARAMS( 1 );

	float res = (float)(atan2(amx_ctof(a_Params[1]), amx_ctof(a_Params[2])) * 180 / PI);
	return amx_ftoc(res);
}

cell AMX_NATIVE_CALL funcAtan ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcAtan()" );
	CHECK_PARAMS( 1 );

	float res = (float)(atan(amx_ctof(a_Params[1])) * 180 / PI);
	return amx_ftoc(res);
}

cell AMX_NATIVE_CALL funcGpci ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcGpci()" );
	CHECK_PARAMS( 3 );

	_PlayerID playerId = ( _PlayerID )a_Params[1];
	if(!__NetGame || !__NetGame->playerPool) return 0;

	if(__NetGame->playerPool->GetSlotState(playerId))
	{
		cell* dest = 0;
		amx_GetAddr( a_AmxInterface, a_Params[ 2 ], &dest );
		return amx_SetString( dest, __NetGame->playerPool->playersSerial[playerId], 0, 0, a_Params[ 3 ] );
	}

	//return _funcGpci ( a_AmxInterface, a_Params );
	return 0;
}

cell AMX_NATIVE_CALL funcGameModeExit ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcGameModeExit()" );

	if(__NetGame->setNextGamemodeFile(NULL)) 
		isGamemodeFinished = TRUE;
	else 
	{
		logprintf("The gamemode finished and I couldn't start another script.");
		fcloseall();
		exit(1);
	}

	return 0;
}

cell AMX_NATIVE_CALL funcSetGameModeText ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcSetGameModeText()" );
	CHECK_PARAMS( 1 );

	char* szGameModeText;
	amx_StrParam(a_AmxInterface, a_Params[1], szGameModeText);
	__Console->SetStringVar("gamemodetext", szGameModeText);

	return 0;
}

cell AMX_NATIVE_CALL funcSetTeamCount ( AMX* a_AmxInterface, cell* a_Params )
{
	return 0;
}

cell AMX_NATIVE_CALL funcAddPlayerClass ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcAddPlayerClass()" );
	CHECK_PARAMS( 11 );

	tSPAWNS spawn;

	spawn.Skin = (int)a_Params[ 1 ];
	spawn.posX = amx_ctof( a_Params[2] );
	spawn.posY = amx_ctof( a_Params[3] );
	spawn.posZ = amx_ctof( a_Params[4] );
	spawn.zAngle = amx_ctof( a_Params[5] );
	spawn.weapons[0] = (int)a_Params[ 6 ];
	spawn.ammo[0] = (int)a_Params[ 7 ];
	spawn.weapons[1] = (int)a_Params[ 8 ];
	spawn.ammo[1] = (int)a_Params[ 9 ];
	spawn.weapons[2] = (int)a_Params[ 10 ];
	spawn.ammo[2] = (int)a_Params[ 11 ];
	spawn.Team = 0xFF;

	return __NetGame->addSpawn( spawn );
}

cell AMX_NATIVE_CALL funcAddPlayerClassEx ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcAddPlayerClassEx()" );
	CHECK_PARAMS( 12 );
	//return _funcAddPlayerClassEx ( a_AmxInterface, a_Params );

	tSPAWNS spawn;

	spawn.Skin = (int)a_Params[ 2 ];
	spawn.posX = amx_ctof( a_Params[3] );
	spawn.posY = amx_ctof( a_Params[4] );
	spawn.posZ = amx_ctof( a_Params[5] );
	spawn.zAngle = amx_ctof( a_Params[6] );
	spawn.weapons[0] = (int)a_Params[ 7 ];
	spawn.ammo[0] = (int)a_Params[ 8 ];
	spawn.weapons[1] = (int)a_Params[ 9 ];
	spawn.ammo[1] = (int)a_Params[ 10 ];
	spawn.weapons[2] = (int)a_Params[ 11 ];
	spawn.ammo[2] = (int)a_Params[ 12 ];
	spawn.Team = (uint8_t)a_Params[ 1 ];

	return __NetGame->addSpawn( spawn );
}

cell AMX_NATIVE_CALL funcAddStaticVehicle ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcAddStaticVehicle()" );
	CHECK_PARAMS( 7 );
	
	tVector vecPos;
	vecPos.X = amx_ctof(a_Params[2]);
	vecPos.Y = amx_ctof(a_Params[3]);
	vecPos.Z = amx_ctof(a_Params[4]);

	_VehicleID ret = __NetGame->vehiclePool->New((int)a_Params[1], &vecPos, amx_ctof(a_Params[5]), (int)a_Params[6], (int)a_Params[7], 120000);

	if((int)a_Params[1] == TRAIN_PASSENGER_LOCO) 
	{
		__NetGame->vehiclePool->New(TRAIN_PASSENGER, &vecPos, amx_ctof(a_Params[5]), (int)a_Params[6], (int)a_Params[7], -1);
		__NetGame->vehiclePool->New(TRAIN_PASSENGER, &vecPos, amx_ctof(a_Params[5]), (int)a_Params[6], (int)a_Params[7], -1);
		__NetGame->vehiclePool->New(TRAIN_PASSENGER, &vecPos, amx_ctof(a_Params[5]), (int)a_Params[6], (int)a_Params[7], -1);
	}
	if((int)a_Params[1] == TRAIN_FREIGHT_LOCO) 
	{
		__NetGame->vehiclePool->New(TRAIN_FREIGHT, &vecPos, amx_ctof(a_Params[5]), (int)a_Params[6], (int)a_Params[7], -1);
		__NetGame->vehiclePool->New(TRAIN_FREIGHT, &vecPos, amx_ctof(a_Params[5]), (int)a_Params[6], (int)a_Params[7], -1);
		__NetGame->vehiclePool->New(TRAIN_FREIGHT, &vecPos, amx_ctof(a_Params[5]), (int)a_Params[6], (int)a_Params[7], -1);
	}
	

	return ret;
}

cell AMX_NATIVE_CALL funcAddStaticVehicleEx ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcAddStaticVehicleEx()" );
	CHECK_PARAMS( 8 );
	
	tVector vecPos;

	int vehicleRespawnTime = (int)a_Params[8];
	if(vehicleRespawnTime != (-1)) 
		vehicleRespawnTime *= 1000;

	vecPos.X = amx_ctof(a_Params[2]);
	vecPos.Y = amx_ctof(a_Params[3]);
	vecPos.Z = amx_ctof(a_Params[4]);

	_VehicleID ret = __NetGame->vehiclePool->New((int)a_Params[1], &vecPos, amx_ctof(a_Params[5]), (int)a_Params[6], (int)a_Params[7], vehicleRespawnTime);


	if((int)a_Params[1] == TRAIN_PASSENGER_LOCO) 
	{
		__NetGame->vehiclePool->New(TRAIN_PASSENGER, &vecPos, amx_ctof(a_Params[5]), (int)a_Params[6], (int)a_Params[7], -1);
		__NetGame->vehiclePool->New(TRAIN_PASSENGER, &vecPos, amx_ctof(a_Params[5]), (int)a_Params[6], (int)a_Params[7], -1);
		__NetGame->vehiclePool->New(TRAIN_PASSENGER, &vecPos, amx_ctof(a_Params[5]), (int)a_Params[6], (int)a_Params[7], -1);
	}
	if((int)a_Params[1] == TRAIN_FREIGHT_LOCO) 
	{
		__NetGame->vehiclePool->New(TRAIN_FREIGHT, &vecPos, amx_ctof(a_Params[5]), (int)a_Params[6], (int)a_Params[7], -1);
		__NetGame->vehiclePool->New(TRAIN_FREIGHT, &vecPos, amx_ctof(a_Params[5]), (int)a_Params[6], (int)a_Params[7], -1);
		__NetGame->vehiclePool->New(TRAIN_FREIGHT, &vecPos, amx_ctof(a_Params[5]), (int)a_Params[6], (int)a_Params[7], -1);
	}

	return ret;
}

cell AMX_NATIVE_CALL funcAddStaticPickup ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcAddStaticPickup()" );
	CHECK_PARAMS ( 6 );

	if ( __NetGame->pickupPool->New ( a_Params[1],a_Params[2],amx_ctof(a_Params[3]),amx_ctof(a_Params[4]),amx_ctof(a_Params[5]), a_Params[6] ) != 0xFFFF ) return 1;
	return 0;
}

cell AMX_NATIVE_CALL funcCreatePickup ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcCreatePickup()" );
	CHECK_PARAMS ( 6 );

	return __NetGame->pickupPool->New ( a_Params[1],a_Params[2],amx_ctof(a_Params[3]),amx_ctof(a_Params[4]),amx_ctof(a_Params[5]), a_Params[6] );
}

cell AMX_NATIVE_CALL funcDestroyPickup ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcDestroyPickup()" );
	CHECK_PARAMS( 1 );
	uint16_t pickupID = ( uint16_t ) a_Params[ 1 ];

	if( __NetGame->pickupPool->GetSlotState( pickupID ) )
	{
		__NetGame->pickupPool->Delete( pickupID );
		return 1;
	}
	//return _funcDestroyPickup ( a_AmxInterface, a_Params );
	return 0;
}

cell AMX_NATIVE_CALL funcSetPlayerWorldBounds ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcSetPlayerWorldBounds()" );
	CHECK_PARAMS( 5 );
	
	RakNet::BitStream bStream;
	float fBounds[4];
	
	CPlayer* player = __NetGame->playerPool->GetPlayer((_PlayerID)a_Params[1]);

	if(!player) return 0;

	fBounds[0] = amx_ctof(a_Params[2]);
	fBounds[1] = amx_ctof(a_Params[3]);
	fBounds[2] = amx_ctof(a_Params[4]);
	fBounds[3] = amx_ctof(a_Params[5]);
	
	bStream.Write(fBounds[0]);
	bStream.Write(fBounds[1]);
	bStream.Write(fBounds[2]);
	bStream.Write(fBounds[3]);

	__NetGame->PlayerRPC( RPC_SetWorldBounds, &bStream, (_PlayerID)a_Params[1], 2);

	return 1;
}

cell AMX_NATIVE_CALL funcShowNameTags ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcShowNameTags()" );
	CHECK_PARAMS( 1 );
	
	__NetGame->showNameTags = (bool)( a_Params[1] != 0 );

	return 1;
}

cell AMX_NATIVE_CALL funcShowPlayerMarkers ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcShowPlayerMarkers()" );
	CHECK_PARAMS( 1 );
	
	__NetGame->showPlayerMarkers = (uint32_t)a_Params[1];

	return 1;
}

cell AMX_NATIVE_CALL funcSetWorldTime ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcSetWorldTime()" );
	CHECK_PARAMS( 1 );

	__NetGame->setWorldTime((uint8_t)a_Params[1]);
	return 1;
}

cell AMX_NATIVE_CALL funcGetWeaponName ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcGetWeaponName()" );
	CHECK_PARAMS( 3 );

	if(a_Params[1] > WEAPON_COLLISION) return 0;
	return set_amxstring(a_AmxInterface, a_Params[2], __NetGame->getWeaponName(a_Params[1]), a_Params[3]);
}

cell AMX_NATIVE_CALL funcEnableTirePopping ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcEnableTirePopping()" );

	logprintf("EnableTirePopping() function is removed.");

	return 1;
}

cell AMX_NATIVE_CALL funcAllowInteriorWeapons ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcAllowInteriorWeapons()" );
	CHECK_PARAMS( 1 );

	__NetGame->allowInteriorWeapons = (bool)( a_Params[1] != 0 );
	return 1;
}

cell AMX_NATIVE_CALL funcSetGravity ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcSetGravity()" );
	CHECK_PARAMS( 1 );

	float value = amx_ctof(a_Params[1]);
	__NetGame->setGravity(value);

	return 1;
}

cell AMX_NATIVE_CALL funcGetGravity ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcGetGravity()" );
	
	return amx_ftoc(__NetGame->gravity);
}

cell AMX_NATIVE_CALL funcAllowAdminTeleport ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcAllowAdminTeleport()" );
	CHECK_PARAMS( 1 );
	logprintf( "AllowAdminTeleport was disable since SA-MP 0.3d, use OnPlayerClickMap callback" );
	//if (a_Params[1]) __NetGame->allowAdminTeleport = true;
	//else __NetGame->allowAdminTeleport = false;
	return 1;
}

cell AMX_NATIVE_CALL funcSetDeathDropAmount ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcSetDeathDropAmount()" );
	CHECK_PARAMS( 1 );

	__NetGame->deathDropAmount = a_Params[1];
	return 1;
}

cell AMX_NATIVE_CALL funcCreateExplosion ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcCreateExplosion()" );
	CHECK_PARAMS( 5 );
	
	RakNet::BitStream bStream;

	bStream.Write(a_Params[1]);
	bStream.Write(a_Params[2]);
	bStream.Write(a_Params[3]);
	bStream.Write(a_Params[4]);
	bStream.Write(a_Params[5]);

	__NetGame->GlobalRPC( RPC_CreateExplosion, &bStream, 0xFFFFu, 2);

	return 1;
}

cell AMX_NATIVE_CALL funcSetDisabledWeapons ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcSetDisabledWeapons()" );

	logprintf("SetDisabledWeapons() : function is deprecated. Check weapon in OnPlayerUpdate.");
	return 1;
}

cell AMX_NATIVE_CALL funcUsePlayerPedAnims ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcUsePlayerPedAnims()" );
	
	__NetGame->usePlayerPedAnims = true;
	return 1;
}

cell AMX_NATIVE_CALL funcDisableInteriorEnterExits ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcDisableInteriorEnterExits()" );
	
	__NetGame->disableInteriorEnterExit = true;
	return 1;
}

cell AMX_NATIVE_CALL funcSetNameTagDrawDistance ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcSetNameTagDrawDistance()" );
	CHECK_PARAMS( 1 );

	__NetGame->nameTagDrawDistance = amx_ctof(a_Params[1]);
	return 1;
}

cell AMX_NATIVE_CALL funcDisableNameTagLOS ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcDisableNameTagLOS()" );
	
	__NetGame->disableNameTagLineOfSight = false;
	return 1;
}

cell AMX_NATIVE_CALL funcConnectNPC ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcConnectNPC()" );
	CHECK_PARAMS( 2 );
	
	char szParams[512];
	char szHost[256];
	char* szBotName;
	char* szScriptName;

	amx_StrParam(a_AmxInterface, a_Params[1], szBotName);
	amx_StrParam(a_AmxInterface, a_Params[2], szScriptName);

	if(!strlen(szBotName) || !strlen(szScriptName)) 
	{
		logprintf("ConnectNPC: Bad parameters");
		return 0;
	}

	if(strlen(__Console->GetStringVar("bind"))) 
		strcpy(szHost, __Console->GetStringVar("bind"));
	else 
		strcpy(szHost,"127.0.0.1");
  

    sprintf(szParams,"-h %s -p %d -n %s -m %s", szHost, __Console->GetIntVar("port"), szBotName, szScriptName);

	char* szPass = __Console->GetStringVar("password");

	if(szPass && strlen(szPass)) 
	{
		strcat(szParams," -z ");
		strcat(szParams,szPass);
	}

#ifdef WIN32
		ShellExecuteA(0, "open", "samp-npc.exe", szParams, NULL, SW_HIDE);
#else
		char szCurrentDir[MAX_PATH+1];
		char szCmd[MAX_PATH+256];
		getcwd(szCurrentDir,MAX_PATH+1);
		sprintf(szCmd,"%s/samp-npc %s &",szCurrentDir,szParams);
		system(szCmd);
#endif

	return 1;
}

cell AMX_NATIVE_CALL funcIsPlayerNPC ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcIsPlayerNPC()" );
	CHECK_PARAMS( 1 );
	
	cell ret = 0;

	if(__NetGame) 
	{
		if(__NetGame->playerPool && __NetGame->playerPool->GetSlotState((_PlayerID)a_Params[1]) && __NetGame->playerPool->isNPC((_PlayerID)a_Params[1]))
			ret = 1;
	}

	return ret;
}

cell AMX_NATIVE_CALL funcCreate3DTextLabel ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcCreate3DTextLabel()" );

	char* str = 0;
	amx_StrParam( a_AmxInterface, a_Params[1], str );
	bool useLOS = a_Params[8] ? true : false;

	return __NetGame->text3DLabelsPool->Create3DTextLabel( str, a_Params[2], amx_ctof( a_Params[3] ), amx_ctof( a_Params[4] ), amx_ctof( a_Params[5] ), amx_ctof( a_Params[6] ), a_Params[7], useLOS );/*_funcCreate3DTextLabel ( a_AmxInterface, a_Params );*/
}
cell AMX_NATIVE_CALL funcDelete3DTextLabel ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcDelete3DTextLabel()" );

	CHECK_PARAMS( 1 );
	_Text3DID labelID = ( _Text3DID ) a_Params[ 1 ];
	if( __NetGame->text3DLabelsPool->GetSlotState( labelID ) )
	{
		__NetGame->text3DLabelsPool->Delete3DTextLabel(labelID );
		return 1;
	}
	return 0;/*_funcDelete3DTextLabel ( a_AmxInterface, a_Params );*/
}
cell AMX_NATIVE_CALL funcAttach3DTextLabelToPlayer ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcAttach3DTextLabelToPlayer()" );
	CHECK_PARAMS( 5 );

	_Text3DID labelID = ( _Text3DID ) a_Params[ 1 ];

	if( __NetGame->text3DLabelsPool->GetSlotState( labelID ) )
	{
		__NetGame->text3DLabelsPool->Attach3DTextLabelPlayer( labelID, ( _PlayerID ) a_Params[ 2 ], amx_ctof( a_Params[ 3 ] ), amx_ctof( a_Params[ 4 ] ), amx_ctof( a_Params[ 5 ] ) );
		return 1;
	}
	return 0;
	//return _funcAttach3DTextLabelToPlayer ( a_AmxInterface, a_Params );
}
cell AMX_NATIVE_CALL funcAttach3DTextLabelToVehicle ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcAttach3DTextLabelToVehicle()" );
	CHECK_PARAMS( 5 );

	_Text3DID labelID = ( _Text3DID ) a_Params[ 1 ];

	if( __NetGame->text3DLabelsPool->GetSlotState( labelID ) )
	{
		__NetGame->text3DLabelsPool->Attach3DTextLabelToVehicle( labelID, ( _VehicleID ) a_Params[ 2 ], amx_ctof( a_Params[ 3 ] ), amx_ctof( a_Params[ 4 ] ), amx_ctof( a_Params[ 5 ] ) );
		return 1;
	}
	return 0;
	//return _funcAttach3DTextLabelToVehicle ( a_AmxInterface, a_Params );
}
cell AMX_NATIVE_CALL funcUpdate3DTextLabelText ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcUpdate3DTextLabelText()" );
	CHECK_PARAMS( 3 );

	_Text3DID labelID = ( _Text3DID )a_Params[ 1 ];

	if( __NetGame->text3DLabelsPool->GetSlotState( labelID ) )
	{
		char* str = 0;
		amx_StrParam( a_AmxInterface, a_Params[ 3 ], str );
		__NetGame->text3DLabelsPool->Update3DTextLabelText( labelID, a_Params[ 2 ], str );
		return 1;
	}
	return 0;
	//return _funcUpdate3DTextLabelText ( a_AmxInterface, a_Params );
}
cell AMX_NATIVE_CALL funcCreatePlayer3DTextLabel ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcCreatePlayer3DTextLabel()" );
	CHECK_PARAMS( 10 );

	_PlayerID playerID = ( _PlayerID ) a_Params[1];

	if( __NetGame->playerPool->GetSlotState( playerID ) )
	{
		CPlayer* player = __NetGame->playerPool->GetPlayer( playerID );
		if( player == 0 ) return 0;

		char* str = 0;
		amx_StrParam( a_AmxInterface, a_Params[ 2 ], str );

		return player->getPlayerTextLabels( )->New( str, a_Params[ 3 ], amx_ctof( a_Params[ 4 ] ), amx_ctof( a_Params[ 5 ] ), amx_ctof( a_Params[ 6 ] ), amx_ctof( a_Params[ 7 ] ), ( _PlayerID )a_Params[ 8 ], ( _VehicleID ) a_Params[ 9 ], ( bool )( a_Params[ 10 ] != 0 ? true : false ) );
	}

	return 0xFFFF;
	//return _funcCreatePlayer3DTextLabel ( a_AmxInterface, a_Params );
}
cell AMX_NATIVE_CALL funcDeletePlayer3DTextLabel ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcDeletePlayer3DTextLabel()" );
	CHECK_PARAMS( 2 );

	_PlayerID playerID = ( _PlayerID )a_Params[ 1 ];

	if( __NetGame->playerPool->GetSlotState( playerID ) )
	{
		CPlayer* player = __NetGame->playerPool->GetPlayer( playerID );
		if( player == 0 ) return 0;
		_Text3DID labelID = ( _Text3DID )a_Params[ 2 ];
		if( player->getPlayerTextLabels( )->GetSlotState( labelID ) )
		{
			player->getPlayerTextLabels( )->Delete( labelID );
			return 1;
		}
	}
	return 0;
	//return _funcDeletePlayer3DTextLabel ( a_AmxInterface, a_Params );
}
cell AMX_NATIVE_CALL funcUpdatePlayer3DTextLabelText ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcUpdatePlayer3DTextLabelText()" );
	CHECK_PARAMS( 4 );

	_PlayerID playerID = ( _PlayerID ) a_Params[ 1 ];

	if( __NetGame->playerPool->GetSlotState( playerID ) )
	{
		CPlayer* player = __NetGame->playerPool->GetPlayer( playerID );
		if( player == 0 ) return 0;
		_Text3DID labelID = ( _Text3DID )a_Params[ 2 ];

		if( player->getPlayerTextLabels( )->GetSlotState( labelID ) )
		{
			char* str = 0;
			amx_StrParam( a_AmxInterface, a_Params[ 4 ], str );
			player->getPlayerTextLabels( )->UpdateText( labelID, a_Params[ 3 ], str );
			return 1;
		}
	}
	return 0;
	//return _funcUpdatePlayer3DTextLabelText ( a_AmxInterface, a_Params );
}
cell AMX_NATIVE_CALL funcShowPlayerDialog ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcShowPlayerDialog()" );
	CHECK_PARAMS( 7 );

	RakNet::BitStream bsSend;
	if(!__NetGame->playerPool || !__NetGame->playerPool->GetSlotState((_PlayerID)a_Params[1])) return 0;
    
	_PlayerID playerId = (_PlayerID)a_Params[1];
	uint16_t DialogId = (uint16_t)a_Params[2];
	uint8_t Style = (uint8_t)a_Params[3];
		
	char * szCaption;
	unsigned char aCaptionLen;
	amx_StrParam(a_AmxInterface, a_Params[4], szCaption);
	if(!szCaption) szCaption = "";
	aCaptionLen = (unsigned char)strlen(szCaption);

	char * szButton1;
	unsigned char aButton1Len;
	amx_StrParam(a_AmxInterface, a_Params[6], szButton1);
	if(!szButton1) szButton1 = "";
	aButton1Len = (unsigned char)strlen(szButton1);

	char * szButton2;
	unsigned char aButton2Len;
	amx_StrParam(a_AmxInterface, a_Params[7], szButton2);
	if(!szButton2) szButton2 = "";
	aButton2Len = (unsigned char)strlen(szButton2);
	
	char * szInfo;
	amx_StrParam(a_AmxInterface, a_Params[5], szInfo);
	if(!szInfo) szInfo = "";

	bsSend.Write(DialogId);
	bsSend.Write(Style);
	bsSend.Write(aCaptionLen);
	bsSend.Write(szCaption,aCaptionLen);
	bsSend.Write(aButton1Len);
	bsSend.Write(szButton1,aButton1Len);
	bsSend.Write(aButton2Len);
	bsSend.Write(szButton2,aButton2Len);
	stringCompressor->EncodeString(szInfo, strlen(szInfo)+1, &bsSend);

	__NetGame->PlayerRPC( RPC_ShowPlayerDialog, &bsSend, playerId, 2);

	return 1;
}
cell AMX_NATIVE_CALL funcEnableZoneNames ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcEnableZoneNames()" );
	CHECK_PARAMS( 1 );
	
	logprintf("Warning: EnableZoneNames() is removed since 0.3");
	__NetGame->enableZoneName = (bool)( a_Params[1] != 0 );
	return 1;
}


cell AMX_NATIVE_CALL funcIsPlayerAdmin ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcIsPlayerAdmin()" );
	CHECK_PARAMS( 1 );
	
	if (__NetGame->playerPool->GetSlotState((_PlayerID)a_Params[1]))
		return __NetGame->playerPool->isPlayerAdmin[(_PlayerID)a_Params[1]];

	return 0;
}
cell AMX_NATIVE_CALL funcKick ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcKick()" );
	
	if (__NetGame->playerPool->GetSlotState((_PlayerID)a_Params[1])) 
	{
		__NetGame->KickPlayer((_PlayerID)a_Params[1]);
		return 1;
	}
	return 0;
}
cell AMX_NATIVE_CALL funcBan ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcBan()" );
	
	if (__NetGame->playerPool->GetSlotState((_PlayerID)a_Params[1])) 
	{
		RakServerInterface* pRak = __NetGame->rakServerInterface;
		PlayerID Player = pRak->GetPlayerIDFromIndex(a_Params[1]);

		in_addr in;
		in.s_addr = Player.binaryAddress;
		__NetGame->AddBan(__NetGame->playerPool->getPlayerNick((_PlayerID)a_Params[1]), inet_ntoa(in), "INGAME BAN");
		__NetGame->KickPlayer((_PlayerID)a_Params[1]);
		return 1;
	}
	return 0;
}
cell AMX_NATIVE_CALL funcBanEx ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcBanEx()" );
	CHECK_PARAMS( 2 );
	_PlayerID playerID = ( _PlayerID )a_Params[ 1 ];

	if(__NetGame->playerPool->GetSlotState( playerID ) ) 
	{
		char* str = 0;
		amx_StrParam( a_AmxInterface, a_Params[ 2 ], str );
		RakServerInterface* pRak = __NetGame->rakServerInterface;
		PlayerID Player = pRak->GetPlayerIDFromIndex( playerID );

		in_addr in;
		in.s_addr = Player.binaryAddress;
		__NetGame->AddBan(__NetGame->playerPool->getPlayerNick( playerID ), inet_ntoa(in), str );
		__NetGame->KickPlayer( playerID );
		return 1;
	}
	return 0;
	//return _funcBanEx ( a_AmxInterface, a_Params );
}
cell AMX_NATIVE_CALL funcSendRconCommand ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcSendRconCommand()" );
	CHECK_PARAMS( 1 );

	char* command = 0;
	amx_StrParam( a_AmxInterface, a_Params[ 1 ], command );
	__Console->Execute( command );
	return 1;
	//return _funcSendRconCommand ( a_AmxInterface, a_Params );
}
cell AMX_NATIVE_CALL funcGetServerVarAsString ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcGetServerVarAsString()" );
	CHECK_PARAMS( 3 );
	char* str = 0;
	amx_StrParam( a_AmxInterface, a_Params[ 1 ], str );
	cell* ptr = 0;
	amx_GetAddr( a_AmxInterface, a_Params[ 2 ], &ptr );
	amx_SetString( ptr, __Console->GetStringVar( str ), 0, 0, a_Params[ 3 ] );
	return 0;
	//return _funcGetServerVarAsString ( a_AmxInterface, a_Params );
}
cell AMX_NATIVE_CALL funcGetServerVarAsInt ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcGetServerVarAsInt()" );
	CHECK_PARAMS( 1 );
	char* str = 0;
	amx_StrParam( a_AmxInterface, a_Params[ 1 ], str );
	return __Console->GetIntVar( str );
//	return _funcGetServerVarAsInt ( a_AmxInterface, a_Params );
}
cell AMX_NATIVE_CALL funcGetServerVarAsBool ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcGetServerVarAsBool()" );
	CHECK_PARAMS( 1 );
	char* str = 0;
	amx_StrParam( a_AmxInterface, a_Params[ 1 ], str );
	return (cell)( __Console->GetBoolVar( str ) ? 1 : 0 );
	//return _funcGetServerVarAsBool ( a_AmxInterface, a_Params );
}
cell AMX_NATIVE_CALL funcSetSpawnInfo ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcSetSpawnInfo()" );
	CHECK_PARAMS( 13 );

	_PlayerID playerID = ( _PlayerID ) a_Params[ 1 ];


	if( __NetGame->playerPool->GetSlotState( playerID ) )
	{
		CPlayer* player = __NetGame->playerPool->GetPlayer( playerID );
		if( player == 0 ) return 0;
		tSPAWNS spawn;

		spawn.Team			= a_Params[ 2 ] & 0xFF;
		spawn.Skin			= a_Params[ 3 ];
		spawn.posX			= amx_ctof( a_Params[ 4 ] );
		spawn.posY			= amx_ctof( a_Params[ 5 ] );
		spawn.posZ			= amx_ctof( a_Params[ 6 ] );
		spawn.zAngle		= amx_ctof( a_Params[ 7 ] );
		spawn.weapons[ 0 ]	= a_Params[ 8 ];
		spawn.ammo[ 0 ]		= a_Params[ 9 ];
		spawn.weapons[ 1 ]	= a_Params[ 10 ];
		spawn.ammo[ 1 ]		= a_Params[ 11 ];
		spawn.weapons[ 2 ]	= a_Params[ 12 ];
		spawn.ammo[ 2 ]		= a_Params[ 13 ];

		

		player->setCustomSpawn( &spawn );
		RakNet::BitStream bStream;
#ifdef _BIG_ENDIAN_SYSTEM
		bStream.Write(spawn.Team);
		bStream.Write(spawn.Skin);
		bStream.Write(spawn.unk);
		bStream.Write(spawn.posX);
		bStream.Write(spawn.posY);
		bStream.Write(spawn.posZ);
		bStream.Write(spawn.zAngle);
		bStream.Write(spawn.weapons[0]), bStream.Write(spawn.weapons[1]), bStream.Write(spawn.weapons[2]);
		bStream.Write(spawn.ammo[0]), bStream.Write(spawn.ammo[1]), bStream.Write(spawn.ammo[2]);
#else
		bStream.Write( (char*)&spawn, sizeof( tSPAWNS ) );
#endif
		__NetGame->PlayerRPC( RPC_SetCustomSpawn, &bStream, playerID, 2 );
		return 1;
	}
	return 0;
	//return _funcSetSpawnInfo ( a_AmxInterface, a_Params );
}
cell AMX_NATIVE_CALL funcSpawnPlayer ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcSpawnPlayer()" );
	CHECK_PARAMS( 1 );

	_PlayerID playerID = ( _PlayerID ) a_Params[ 1 ];

	if( __NetGame->playerPool->GetSlotState( playerID ) )
	{
		RakNet::BitStream bStream;
		bStream.Write( ( uint32_t ) 2 );
		__NetGame->PlayerRPC( RPC_RequestSpawn, &bStream, playerID, 2 ); 
		return 1;
	}
	return 0;
	//return _funcSpawnPlayer ( a_AmxInterface, a_Params );
}
cell AMX_NATIVE_CALL funcSetPlayerTeam ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcSetPlayerTeam()" );
	CHECK_PARAMS( 2 );
	_PlayerID playerID = ( _PlayerID ) a_Params[ 1 ];

	if( __NetGame->playerPool->GetSlotState( playerID ) )
	{
		CPlayer* player = __NetGame->playerPool->GetPlayer( playerID );
		if( player == 0 ) return 0;

		player->getCustomSpawn()->Team = a_Params[ 2 ] & 0xFF;
		RakNet::BitStream bStream;
		
		bStream.Write( playerID );
		bStream.Write( ( uint8_t ) a_Params[ 2 ] );
		__NetGame->GlobalRPC( RPC_SetPlayerTeam, &bStream, -1, 2 );
	}
	return 0;
	//return _funcSetPlayerTeam ( a_AmxInterface, a_Params );
}


cell AMX_NATIVE_CALL funcSetPlayerChatBubble ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcSetPlayerChatBubble()" );
	CHECK_PARAMS( 5 );
	_PlayerID playerID = ( _PlayerID ) a_Params[ 1 ];

	if( __NetGame->playerPool->GetPlayer( playerID ) )
	{
		CPlayer* player = __NetGame->playerPool->GetPlayer( playerID );
		if( player == 0 ) return 0;

		if( player->isActive( ) || player->isSpectating( ) )
		{
			char* str = 0;
			amx_StrParam( a_AmxInterface, a_Params[ 2 ], str );

			RakNet::BitStream bStream;
			bStream.Write( playerID );
			bStream.Write( a_Params[ 3 ] );
			bStream.Write( a_Params[ 4 ] );
			bStream.Write( a_Params[ 5 ] );
			bStream.Write( ( uint8_t )strlen( str ) );
			bStream.Write( str, strlen( str ) );
			
			__NetGame->AddedPlayersRPC( RPC_SetPlayerChatBubble, &bStream, playerID, 2 );
		}
	}
	return 0;
	//return _funcSetPlayerChatBubble ( a_AmxInterface, a_Params );
}
cell AMX_NATIVE_CALL funcSetPVarInt ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcSetPVarInt()" );
	CHECK_PARAMS( 3 );

	_PlayerID playerID = ( _PlayerID ) a_Params[ 1 ];
	if( __NetGame->playerPool->GetSlotState( playerID ) )
	{
		CPlayer* player = __NetGame->playerPool->GetPlayer( playerID );
		if( player )
		{
			char* pVarName = NULL;
			amx_StrParam( a_AmxInterface, a_Params[ 2 ], pVarName );
			if( pVarName == 0 ) return 0;

			if(player->getPlayerVars()->SetIntVar(pVarName, a_Params[ 3 ])) return 1;
		}
	}
	return 0;
}
cell AMX_NATIVE_CALL funcSetPVarString ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcSetPVarString()" );
	CHECK_PARAMS( 3 );

	_PlayerID playerID = ( _PlayerID ) a_Params[ 1 ];
	if( __NetGame->playerPool->GetSlotState( playerID ) )
	{
		CPlayer* player = __NetGame->playerPool->GetPlayer( playerID );
		if( player )
		{
			char* pVarName = NULL;
			amx_StrParam( a_AmxInterface, a_Params[ 2 ], pVarName );
			if( pVarName == 0 ) return 0;

			char* pVarValue;
			amx_StrParam( a_AmxInterface, a_Params[ 3 ], pVarValue );
			if( pVarValue == 0 ) pVarValue = "\0";

			if(player->getPlayerVars()->SetStringVar(pVarName, pVarValue)) return 1;
		}
	}
	return 0;
}
cell AMX_NATIVE_CALL funcSetPVarFloat ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcSetPVarFloat()" );
	CHECK_PARAMS( 3 );

	_PlayerID playerID = ( _PlayerID ) a_Params[ 1 ];
	if( __NetGame->playerPool->GetSlotState( playerID ) )
	{
		CPlayer* player = __NetGame->playerPool->GetPlayer( playerID );
		if( player )
		{
			char* pVarName = NULL;
			amx_StrParam( a_AmxInterface, a_Params[ 2 ], pVarName );
			if( pVarName == 0 ) return 0;

			if(player->getPlayerVars()->SetFloatVar(pVarName, amx_ctof(a_Params[ 3 ]))) return 1;
		}
	}
	return 0;
}
cell AMX_NATIVE_CALL funcGetPVarInt ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcGetPVarInt()" );
	CHECK_PARAMS( 2 );

	_PlayerID playerID = ( _PlayerID ) a_Params[ 1 ];
	if( __NetGame->playerPool->GetSlotState( playerID ) )
	{
		CPlayer* player = __NetGame->playerPool->GetPlayer( playerID );
		if( player )
		{
			char* pVarName = NULL;
			amx_StrParam( a_AmxInterface, a_Params[ 2 ], pVarName );
			if( pVarName == 0 ) return 0;

			return player->getPlayerVars()->GetIntVar(pVarName);
		}
	}
	return 0;
}
cell AMX_NATIVE_CALL funcGetPVarString ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcGetPVarString()" );
	CHECK_PARAMS( 4 );

	_PlayerID playerID = ( _PlayerID ) a_Params[ 1 ];
	if( __NetGame->playerPool->GetSlotState( playerID ) )
	{
		CPlayer* player = __NetGame->playerPool->GetPlayer( playerID );
		if( player )
		{
			char* pVarName = NULL;
			amx_StrParam( a_AmxInterface, a_Params[ 2 ], pVarName );
			if( pVarName == 0 ) return 0;

			char* pVarValue = player->getPlayerVars()->GetStringVar(pVarName);
			if( pVarValue == 0 ) return 0;

			cell* dest = 0;
			amx_GetAddr( a_AmxInterface, a_Params[ 3 ], &dest );
			return amx_SetString( dest, pVarValue, 0, 1, a_Params[ 4 ] );
		}
	}
	return 0;
}
cell AMX_NATIVE_CALL funcGetPVarFloat ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcGetPVarFloat()" );
	CHECK_PARAMS( 2 );

	_PlayerID playerID = ( _PlayerID ) a_Params[ 1 ];
	if( __NetGame->playerPool->GetSlotState( playerID ) )
	{
		CPlayer* player = __NetGame->playerPool->GetPlayer( playerID );
		if( player )
		{
			char* pVarName = NULL;
			amx_StrParam( a_AmxInterface, a_Params[ 2 ], pVarName );
			if( pVarName == 0 ) return 0;

			float ret = player->getPlayerVars()->GetFloatVar(pVarName);
			return amx_ftoc(ret);
		}
	}
	return 0;
}
cell AMX_NATIVE_CALL funcDeletePVar ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcDeletePVar()" );
	CHECK_PARAMS( 2 );

	_PlayerID playerID = ( _PlayerID ) a_Params[ 1 ];
	if( __NetGame->playerPool->GetSlotState( playerID ) )
	{
		CPlayer* player = __NetGame->playerPool->GetPlayer( playerID );
		if( player )
		{
			char* pVarName = NULL;
			amx_StrParam( a_AmxInterface, a_Params[ 2 ], pVarName );
			if( pVarName == 0 ) return 0;

			return player->getPlayerVars()->DeleteVar(pVarName);
		}
	}
	return 0;
}
cell AMX_NATIVE_CALL funcGetPVarType ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcGetPVarType()" );
	CHECK_PARAMS( 2 );

	_PlayerID playerID = ( _PlayerID ) a_Params[ 1 ];
	if( __NetGame->playerPool->GetSlotState( playerID ) )
	{
		CPlayer* player = __NetGame->playerPool->GetPlayer( playerID );
		if( player )
		{
			char* pVarName = NULL;
			amx_StrParam( a_AmxInterface, a_Params[ 2 ], pVarName );
			if( pVarName == 0 ) return 0;

			return player->getPlayerVars()->GetVarType(pVarName);
		}
	}
	return 0;
}
cell AMX_NATIVE_CALL funcGetPVarNameAtIndex ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcGetPVarNameAtIndex()" );
	CHECK_PARAMS( 4 );

	_PlayerID playerID = ( _PlayerID ) a_Params[ 1 ];
	if( __NetGame->playerPool->GetSlotState( playerID ) )
	{
		CPlayer* player = __NetGame->playerPool->GetPlayer( playerID );
		if( player )
		{
			char* pVarName = player->getPlayerVars()->GetVarNameFromID(a_Params[ 2 ]);
			cell* dest = 0;
			amx_GetAddr( a_AmxInterface, a_Params[ 3 ], &dest );
			if(pVarName == NULL) 
			{
				amx_SetString( dest, "\0", 0, 0, a_Params[ 4 ] );
				return 0;
			}
			return amx_SetString( dest, pVarName, 0, 0, a_Params[ 4 ] );
		}
	}
	return 0;
}
cell AMX_NATIVE_CALL funcGetPVarsUpperIndex ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcGetPVarsUpperIndex()" );
	CHECK_PARAMS( 1 );

	_PlayerID playerID = ( _PlayerID ) a_Params[ 1 ];
	if( __NetGame->playerPool->GetSlotState( playerID ) )
	{
		CPlayer* player = __NetGame->playerPool->GetPlayer( playerID );
		if( player )
			return player->getPlayerVars()->upperIndex;
	}
	return 0;
}
cell AMX_NATIVE_CALL funcIsValidVehicle ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcIsValidVehicle()" );
	CHECK_PARAMS( 1 );

	_VehicleID vehicleID = ( _VehicleID ) a_Params[ 1 ];
	return ( cell )( __NetGame->vehiclePool->GetSlotState( vehicleID ) ? 1 : 0 );
	//return _funcIsValidVehicle ( a_AmxInterface, a_Params );
}
cell AMX_NATIVE_CALL funcCreateVehicle ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcCreateVehicle()" );
	CHECK_PARAMS( 8 );

	int modelID = (int)a_Params[1];
	
	if( a_Params[8] != -1 ) a_Params[8] *= 1000;

	if( modelID == TRAIN_FREIGHT_LOCO || modelID == TRAIN_PASSENGER_LOCO )
	{
		logprintf("Only create trains with AddStaticVehicle/Ex");
		return 0;
	}

	tVector vecPos;
	vecPos.X = amx_ctof( a_Params[ 2 ] );
	vecPos.Y = amx_ctof( a_Params[ 3 ] );
	vecPos.Z = amx_ctof( a_Params[ 4 ] );
	return __NetGame->vehiclePool->New( modelID, &vecPos, amx_ctof( a_Params[ 5 ] ), a_Params[ 6 ], a_Params[ 7 ], a_Params[ 8 ] );
	//return _funcCreateVehicle ( a_AmxInterface, a_Params );
}
cell AMX_NATIVE_CALL funcDestroyVehicle ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcDestroyVehicle()" );
	CHECK_PARAMS( 1 );

	_VehicleID vehicleID = ( _VehicleID )a_Params[ 1 ];

	if( __NetGame->vehiclePool->GetSlotState( vehicleID ) )
	{
		__NetGame->vehiclePool->Delete( vehicleID );
		return 1;
	}
	return 0;
	//return _funcDestroyVehicle ( a_AmxInterface, a_Params );
}
cell AMX_NATIVE_CALL funcGetVehiclePos ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcGetVehiclePos()" );

	CHECK_PARAMS( 4 );

	_VehicleID vehicleID = ( _VehicleID )a_Params[ 1 ];

	if( __NetGame->vehiclePool->GetSlotState( vehicleID ) )
	{
		CVehicle* vehicle = __NetGame->vehiclePool->GetVehicle( vehicleID );
		if( vehicle == 0 ) return 0;

		cell* ptr = 0;
		amx_GetAddr( a_AmxInterface, a_Params[ 2 ], &ptr );
		*ptr = amx_ftoc( vehicle->vehPos.X );
		amx_GetAddr( a_AmxInterface, a_Params[ 3 ], &ptr );
		*ptr = amx_ftoc( vehicle->vehPos.Y );
		amx_GetAddr( a_AmxInterface, a_Params[ 4 ], &ptr );
		*ptr = amx_ftoc( vehicle->vehPos.Z );
		return 1;
	}
	return 0;	
	//	return _funcGetVehiclePos ( a_AmxInterface, a_Params );
}
cell AMX_NATIVE_CALL funcSetVehiclePos ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcSetVehiclePos()" );
	CHECK_PARAMS( 4 );

	_VehicleID vehicleID = ( _VehicleID )a_Params[ 1 ];

	if( __NetGame->vehiclePool->GetSlotState( vehicleID ) )
	{
		CVehicle* vehicle = __NetGame->vehiclePool->GetVehicle( vehicleID );
		if( vehicle == 0 ) return 0;

		vehicle->vehMatrix.pos.X = amx_ctof( a_Params[ 2 ] );
		vehicle->vehMatrix.pos.Y = amx_ctof( a_Params[ 3 ] );
		vehicle->vehMatrix.pos.Z = amx_ctof( a_Params[ 4 ] );
		vehicle->vehPos = vehicle->vehMatrix.pos;


		for( _PlayerID playerID = 0; playerID < MAX_PLAYERS; playerID ++ )
		{
			if( __NetGame->playerPool->GetSlotState( playerID ) )
			{
				CPlayer* player = __NetGame->playerPool->GetPlayer( playerID );
				if( player == 0) return 0;

				if( player->isVehicleStreamedIn( vehicleID ) )
				{
					vehicle->UpdatePositionForPlayer( playerID, vehicle->vehPos.X, vehicle->vehPos.Y, vehicle->vehPos.Z );
					if( vehicle->vehDriverID == playerID ) 
						player->SetPlaceToBe( vehicle->vehPos.X, vehicle->vehPos.Y, vehicle->vehPos.Z );
				}

			}
		}
		return 1;
	}
	return 0;
	//return _funcSetVehiclePos ( a_AmxInterface, a_Params );
}
cell AMX_NATIVE_CALL funcGetVehicleZAngle ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcGetVehicleZAngle()" );
	CHECK_PARAMS( 2 );

	_VehicleID vehicleID = ( _VehicleID ) a_Params[ 1 ];


	if( __NetGame->vehiclePool->GetSlotState( vehicleID ) )
	{
		CVehicle* vehicle = __NetGame->vehiclePool->GetVehicle( vehicleID );
		if( vehicle == 0 ) return 0;

		cell* ptr = 0;
		amx_GetAddr( a_AmxInterface, a_Params[ 2 ], &ptr );

		if( vehicle->vehMatrix.up.X == 0.0f && vehicle->vehMatrix.up.Y == 0.0f )
		{
			*ptr = amx_ftoc( vehicle->GetSpawnInfo()->vehRot );
		}
		else
		{
			float angle = atan2( -vehicle->vehMatrix.up.X, vehicle->vehMatrix.up.Y ) * 57.2957763671875f;
			if( angle >= 360.0f ) angle -= 360.0f;
			else if( angle < 0 ) angle += 360.0f;

			*ptr = amx_ftoc( angle );
		}
		return 1;
	}
	return 0;
	//return _funcGetVehicleZAngle ( a_AmxInterface, a_Params );
}
cell AMX_NATIVE_CALL funcGetVehicleRotationQuat ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcGetVehicleRotationQuat()" );

	CHECK_PARAMS( 5 );

	_VehicleID vehicleID = ( _VehicleID )a_Params[ 1 ];

	if( __NetGame->vehiclePool->GetSlotState( vehicleID ) )
	{
		CVehicle* vehicle = __NetGame->vehiclePool->GetVehicle( vehicleID );
		if( vehicle == 0 ) return 0;

		tQuaternionVector quater;
		MatrixToQuaternion( &vehicle->vehMatrix, &quater );

		cell* ptr = 0;
		amx_GetAddr( a_AmxInterface, a_Params[ 2 ], &ptr );
		*ptr = amx_ftoc( quater.W );
		amx_GetAddr( a_AmxInterface, a_Params[ 3 ], &ptr );
		*ptr = amx_ftoc( quater.X );
		amx_GetAddr( a_AmxInterface, a_Params[ 4 ], &ptr );
		*ptr = amx_ftoc( quater.Y );
		amx_GetAddr( a_AmxInterface, a_Params[ 5 ], &ptr );
		*ptr = amx_ftoc( quater.Z );

		return 1;
	}
	return 0;
	//return _funcGetVehicleRotationQuat ( a_AmxInterface, a_Params );
}
cell AMX_NATIVE_CALL funcSetVehicleZAngle ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcSetVehicleZAngle()" );
	CHECK_PARAMS( 2 );

	_VehicleID vehicleID = ( _VehicleID ) a_Params[ 1 ];

	if( __NetGame->vehiclePool->GetSlotState( vehicleID ) )
	{
		CVehicle* vehicle = __NetGame->vehiclePool->GetVehicle( vehicleID );
		if( vehicle == 0) return 0;

		if( vehicle->vehDriverID != 0xFFFF )
		{
			RakNet::BitStream bStream;
			bStream.Write( vehicleID );
			bStream.Write( amx_ctof( a_Params[ 2 ] ) );
			
			__NetGame->PlayerRPC( RPC_SetVehicleZAngle, &bStream, vehicle->vehDriverID, 2);
		}
		return 1;
	}
	return 0;
	//return _funcSetVehicleZAngle ( a_AmxInterface, a_Params );
}
cell AMX_NATIVE_CALL funcSetVehicleParamsForPlayer ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcSetVehicleParamsForPlayer()" );
	CHECK_PARAMS( 4 );
	_VehicleID vehicleID = ( _VehicleID ) a_Params[ 1 ];
	_PlayerID playerID = ( _PlayerID ) a_Params[ 2 ];

	if( __NetGame->vehiclePool->GetSlotState( vehicleID ) )
	{
		if( __NetGame->playerPool->GetSlotState( playerID ) )
		{
			CVehicle* vehicle = __NetGame->vehiclePool->GetVehicle( vehicleID );
			if( !vehicle )
				return 0;

			vehicle->SetParamsForPlayer( playerID, ( uint8_t ) a_Params[ 3 ], ( uint8_t ) a_Params[ 4 ] );
			return 1;
		}
	}
	return 0;
	//return _funcSetVehicleParamsForPlayer ( a_AmxInterface, a_Params );
}
cell AMX_NATIVE_CALL funcManualVehicleEngineAndLights ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcManualVehicleEngineAndLights()" );

	__NetGame->useManualVehicleEngineAndLight = ( bool )( a_Params[ 1 ] != 0 );

	return 1;
}
cell AMX_NATIVE_CALL funcSetVehicleParamsEx ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcSetVehicleParamsEx()" );
	CHECK_PARAMS( 8 );
	_VehicleID vehicleID = ( _VehicleID ) a_Params[ 1 ];

	if( __NetGame->vehiclePool->GetSlotState( vehicleID ) )
	{
		CVehicle* vehicle = __NetGame->vehiclePool->GetVehicle( vehicleID );
		if( vehicle == 0 ) return 0;

		VEHPARAM_EX params;
		params.engine = ( uint8_t )( a_Params[ 2 ] );
		params.lights = ( uint8_t )( a_Params[ 3 ] );
		params.alarm = ( uint8_t )( a_Params[ 4 ] );
		params.doors = ( uint8_t )( a_Params[ 5 ] );
		params.bonnet = ( uint8_t )( a_Params[ 6 ] );
		params.boot = ( uint8_t )( a_Params[ 7 ] );
		params.objective = ( uint8_t )( a_Params[ 8 ] );
		vehicle->SetParamsEx( &params );
		return 1;
	}
	return 0;
	//return _funcSetVehicleParamsEx ( a_AmxInterface, a_Params );
}
cell AMX_NATIVE_CALL funcGetVehicleParamsEx ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcGetVehicleParamsEx()" );
	CHECK_PARAMS( 8 );

	_VehicleID vehicleID = ( _VehicleID ) a_Params[ 1 ];

	if( __NetGame->vehiclePool->GetSlotState( vehicleID ) )
	{
		CVehicle* vehicle = __NetGame->vehiclePool->GetVehicle( vehicleID );
		if( vehicle == 0 ) return 0;
		VEHPARAM_EX params = *vehicle->GetParamsEx( );
		cell* ptr = 0;
		amx_GetAddr( a_AmxInterface, a_Params[ 2 ], &ptr );
		*ptr = ( cell )( params.engine );
		amx_GetAddr( a_AmxInterface, a_Params[ 3 ], &ptr );
		*ptr = ( cell )( params.lights );
		amx_GetAddr( a_AmxInterface, a_Params[ 4 ], &ptr );
		*ptr = ( cell )( params.alarm );
		amx_GetAddr( a_AmxInterface, a_Params[ 5 ], &ptr );
		*ptr = ( cell )( params.doors );
		amx_GetAddr( a_AmxInterface, a_Params[ 6 ], &ptr );
		*ptr = ( cell )( params.bonnet );
		amx_GetAddr( a_AmxInterface, a_Params[ 7 ], &ptr );
		*ptr = ( cell )( params.boot );
		amx_GetAddr( a_AmxInterface, a_Params[ 8 ], &ptr );
		*ptr = ( cell )( params.objective );
		return 1;
	}
	return 0;
	//	return _funcGetVehicleParamsEx ( a_AmxInterface, a_Params );
}
cell AMX_NATIVE_CALL funcSetVehicleToRespawn ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcSetVehicleToRespawn()" );
	CHECK_PARAMS( 1 );
	_VehicleID vehicleID = ( _VehicleID ) a_Params[ 1 ];

	if( __NetGame->vehiclePool->GetSlotState( vehicleID ) )
	{
		CVehicle* vehicle = __NetGame->vehiclePool->GetVehicle( vehicleID );
		if( vehicle == 0 ) return 0;
		vehicle->Respawn();
		return 1;
	}
	return 0;
	//return _funcSetVehicleToRespawn ( a_AmxInterface, a_Params );
}
cell AMX_NATIVE_CALL funcRepairVehicle ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcRepairVehicle()" );
	CHECK_PARAMS( 1 );
	_VehicleID vehicleID = ( _VehicleID ) a_Params[ 1 ];

	if( __NetGame->vehiclePool->GetSlotState( vehicleID ) )
	{
		CVehicle* vehicle = __NetGame->vehiclePool->GetVehicle( vehicleID );
		if( vehicle == 0 ) return 0;
		vehicle->Repair();
		return 1;
	}
	return 0;
	//return _funcRepairVehicle ( a_AmxInterface, a_Params );
}
cell AMX_NATIVE_CALL funcAddVehicleComponent ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcAddVehicleComponent()" );
	CHECK_PARAMS( 2 );

	_VehicleID vehicleID = ( _VehicleID ) a_Params[ 1 ];

	if( __NetGame->vehiclePool->GetSlotState( vehicleID ) )
	{
		CVehicle* vehicle = __NetGame->vehiclePool->GetVehicle( vehicleID );
		if( vehicle == 0 ) return 0;
		vehicle->AddVehicleComponent( 0xFFFF, ( uint16_t )a_Params[ 2 ] );
		return 1;
	}
	return 0;	

	//return _funcAddVehicleComponent ( a_AmxInterface, a_Params );
}
cell AMX_NATIVE_CALL funcRemoveVehicleComponent ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcRemoveVehicleComponent()" );
	CHECK_PARAMS( 2 );

	_VehicleID vehicleID = ( _VehicleID ) a_Params[ 1 ];

	if( __NetGame->vehiclePool->GetSlotState( vehicleID ) )
	{
		CVehicle* vehicle = __NetGame->vehiclePool->GetVehicle( vehicleID );
		if( vehicle == 0 ) return 0;
		vehicle->RemoveVehicleComponent( 0xFFFF, ( uint16_t )a_Params[ 2 ] );
		return 1;
	}
	return 0;	
	
	//	return _funcRemoveVehicleComponent ( a_AmxInterface, a_Params );
}
cell AMX_NATIVE_CALL funcChangeVehicleColor ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcChangeVehicleColor()" );
	CHECK_PARAMS( 3 );

	_VehicleID vehicleID = ( _VehicleID ) a_Params[ 1 ];

	if( __NetGame->vehiclePool->GetSlotState( vehicleID ) )
	{
		CVehicle* vehicle = __NetGame->vehiclePool->GetVehicle( vehicleID );
		if( vehicle == 0 ) return 0;
		vehicle->ChangeColor( 0xFFFF, a_Params[ 2 ], a_Params[ 3 ] );
		return 1;
	}
	return 0;	
//	return _funcChangeVehicleColor ( a_AmxInterface, a_Params );
}
cell AMX_NATIVE_CALL funcChangeVehiclePaintjob ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcChangeVehiclePaintjob()" );

	CHECK_PARAMS( 2 );

	_VehicleID vehicleID = ( _VehicleID ) a_Params[ 1 ];

	if( __NetGame->vehiclePool->GetSlotState( vehicleID ) )
	{
		CVehicle* vehicle = __NetGame->vehiclePool->GetVehicle( vehicleID );
		if( vehicle == 0 ) return 0;
		vehicle->SetPaintjob( 0xFFFF, ( uint8_t )a_Params[ 2 ] );
		return 1;
	}
	return 0;		
	
	//	return _funcChangeVehiclePaintjob ( a_AmxInterface, a_Params );
}
cell AMX_NATIVE_CALL funcLinkVehicleToInterior ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcLinkVehicleToInterior()" );
	CHECK_PARAMS( 2 );

	_VehicleID vehicleID = ( _VehicleID ) a_Params[ 1 ];

	if( __NetGame->vehiclePool->GetSlotState( vehicleID ) )
	{
		CVehicle* vehicle = __NetGame->vehiclePool->GetVehicle( vehicleID );
		if( vehicle == 0 ) return 0;
		vehicle->GetSpawnInfo()->vehInterior = a_Params[ 2 ];
		RakNet::BitStream bStream;
		bStream.Write( vehicleID );
		bStream.Write( ( uint8_t ) a_Params[ 2 ] );
		__NetGame->AddedVehicleRPC( RPC_LinkVehicleToInterior, &bStream, vehicleID );
		return 1;
	}
	return 0;	
//	return _funcLinkVehicleToInterior ( a_AmxInterface, a_Params );
}
cell AMX_NATIVE_CALL funcSetVehicleHealth ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcSetVehicleHealth()" );

	CHECK_PARAMS( 2 );

	_VehicleID vehicleID = ( _VehicleID ) a_Params[ 1 ];

	if( __NetGame->vehiclePool->GetSlotState( vehicleID ) )
	{
		CVehicle* vehicle = __NetGame->vehiclePool->GetVehicle( vehicleID );
		if( vehicle == 0 ) return 0;
		vehicle->SetHealth( amx_ctof( a_Params[ 2 ] ) );
		return 1;
	}	
	
	return 0;
	//	return _funcSetVehicleHealth ( a_AmxInterface, a_Params );
}
cell AMX_NATIVE_CALL funcGetVehicleHealth ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcGetVehicleHealth()" );
	CHECK_PARAMS( 2 );

	_VehicleID vehicleID = ( _VehicleID ) a_Params[ 1 ];

	if( __NetGame->vehiclePool->GetSlotState( vehicleID ) )
	{
		CVehicle* vehicle = __NetGame->vehiclePool->GetVehicle( vehicleID );
		if( vehicle == 0 ) return 0;
		cell* ptr = 0;
		amx_GetAddr( a_AmxInterface, a_Params[ 2 ], &ptr );
		*ptr = amx_ftoc( vehicle->vehHealth );
		return 1;
	}	
	
	return 0;
	
	//	return _funcGetVehicleHealth ( a_AmxInterface, a_Params );
}
cell AMX_NATIVE_CALL funcAttachTrailerToVehicle ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcAttachTrailerToVehicle()" );

	CHECK_PARAMS( 2 );

	_VehicleID vehicleID = ( _VehicleID ) a_Params[ 1 ];
	_VehicleID trailerID = ( _VehicleID ) a_Params[ 2 ];

	if( __NetGame->vehiclePool->GetSlotState( vehicleID ) && __NetGame->vehiclePool->GetSlotState( trailerID ) )
	{

		RakNet::BitStream bStream;
		bStream.Write( vehicleID );
		bStream.Write( trailerID );
		__NetGame->AddedVehicleRPC( RPC_AttachTrailerToVehicle, &bStream, vehicleID );
		return 1;
	}	
	
	return 0;	
	
	//	return _funcAttachTrailerToVehicle ( a_AmxInterface, a_Params );
}
cell AMX_NATIVE_CALL funcDetachTrailerFromVehicle ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcDetachTrailerFromVehicle()" );

	CHECK_PARAMS( 1 );

	_VehicleID vehicleID = ( _VehicleID ) a_Params[ 1 ];

	if( __NetGame->vehiclePool->GetSlotState( vehicleID ) )
	{
	
		RakNet::BitStream bStream;
		bStream.Write( vehicleID );
		__NetGame->AddedVehicleRPC( RPC_DetachTrailerFromVehicle, &bStream, vehicleID );
		return 1;
	}	
	
	return 0;	
	
	//	return _funcDetachTrailerFromVehicle ( a_AmxInterface, a_Params );
}
cell AMX_NATIVE_CALL funcIsTrailerAttachedToVehicle ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcIsTrailerAttachedToVehicle()" );
	CHECK_PARAMS( 1 );

	_VehicleID vehicleID = ( _VehicleID ) a_Params[ 1 ];

	if( __NetGame->vehiclePool->GetSlotState( vehicleID ) )
	{
		CVehicle* vehicle = __NetGame->vehiclePool->GetVehicle( vehicleID );
		if( vehicle == 0 ) return 0;
		return (cell)(vehicle->vehTrailerID != 0);
	}	
	return 0;	
	
	//	return _funcIsTrailerAttachedToVehicle ( a_AmxInterface, a_Params );
}
cell AMX_NATIVE_CALL funcGetVehicleTrailer ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcGetVehicleTrailer()" );
	CHECK_PARAMS( 1 );

	_VehicleID vehicleID = ( _VehicleID ) a_Params[ 1 ];

	if( __NetGame->vehiclePool->GetSlotState( vehicleID ) )
	{
		CVehicle* vehicle = __NetGame->vehiclePool->GetVehicle( vehicleID );
		if( vehicle == 0 ) return 0;
		if( vehicle->vehTrailerID != 0 && vehicle->vehTrailerID != INVALID_VEHICLE_ID )
		{
			return (cell)vehicle->vehTrailerID;
		}
	}	
	return 0;	
//	return _funcGetVehicleTrailer ( a_AmxInterface, a_Params );
}
cell AMX_NATIVE_CALL funcSetVehicleNumberPlate ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcSetVehicleNumberPlate()" );

	CHECK_PARAMS( 2 );

	_VehicleID vehicleID = ( _VehicleID ) a_Params[ 1 ];

	if( __NetGame->vehiclePool->GetSlotState( vehicleID ) )
	{
		CVehicle* vehicle = __NetGame->vehiclePool->GetVehicle( vehicleID );
		if( vehicle == 0 ) return 0;
		char* str = 0;
		amx_StrParam( a_AmxInterface, a_Params[ 2 ], str );
		vehicle->SetNumberPlate( str );
		return 1;
	}	
	
	return 0;

//	return _funcSetVehicleNumberPlate ( a_AmxInterface, a_Params );
}
cell AMX_NATIVE_CALL funcGetVehicleModel ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcGetVehicleModel()" );
	CHECK_PARAMS( 1 );

	_VehicleID vehicleID = ( _VehicleID ) a_Params[ 1 ];

	if( __NetGame->vehiclePool->GetSlotState( vehicleID ) )
	{
		CVehicle* vehicle = __NetGame->vehiclePool->GetVehicle( vehicleID );
		if( vehicle == 0 ) return 0;
		return vehicle->GetSpawnInfo()->vehType;
	}	
	
	return 0;

//	return _funcGetVehicleModel ( a_AmxInterface, a_Params );
}
cell AMX_NATIVE_CALL funcGetVehicleComponentInSlot ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcGetVehicleComponentInSlot()" );
	CHECK_PARAMS( 2 );

	_VehicleID vehicleID = ( _VehicleID ) a_Params[ 1 ];

	if( __NetGame->vehiclePool->GetSlotState( vehicleID ) )
	{
		CVehicle* vehicle = __NetGame->vehiclePool->GetVehicle( vehicleID );
		if( vehicle == 0 ) return 0;

		if( a_Params[ 2 ] < 0xE )
		{
			return (cell)( vehicle->vehModInfo.vehModSlots[ a_Params[ 2 ] ] + 999 );
		}
		return 0;
	}	
	
	return 0;
	
	//	return _funcGetVehicleComponentInSlot ( a_AmxInterface, a_Params );
}
cell AMX_NATIVE_CALL funcGetVehicleComponentType ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcGetVehicleComponentType()" );
	CHECK_PARAMS( 1 );

	if( a_Params[ 1 ] < 0x03E8 || a_Params[ 1 ] > 0x04E9 ) return -1;

	return (cell)vehicleComponentTypes[ a_Params[ 1 ] + 0x18 ];
	//return _funcGetVehicleComponentType ( a_AmxInterface, a_Params );
}
cell AMX_NATIVE_CALL funcGetVehicleVelocity ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcGetVehicleVelocity()" );
	CHECK_PARAMS( 4 );

	_VehicleID vehicleID = ( _VehicleID ) a_Params[ 1 ];

	if( __NetGame->vehiclePool->GetSlotState( vehicleID ) )
	{
		CVehicle* vehicle = __NetGame->vehiclePool->GetVehicle( vehicleID );
		if( vehicle == 0 ) return 0;

		cell* ptr = 0;
		amx_GetAddr( a_AmxInterface, a_Params[ 2 ], &ptr );
		*ptr = amx_ftoc( vehicle->vehMoveSpeed.X );
		amx_GetAddr( a_AmxInterface, a_Params[ 3 ], &ptr );
		*ptr = amx_ftoc( vehicle->vehMoveSpeed.Y );
		amx_GetAddr( a_AmxInterface, a_Params[ 4 ], &ptr );
		*ptr = amx_ftoc( vehicle->vehMoveSpeed.Z );
		return 1;
	}
	return 0;
	//return _funcGetVehicleVelocity ( a_AmxInterface, a_Params );
}
cell AMX_NATIVE_CALL funcSetVehicleVelocity ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcSetVehicleVelocity()" );
	CHECK_PARAMS( 4 );

	_VehicleID vehicleID = ( _VehicleID ) a_Params[ 1 ];

	if( __NetGame->vehiclePool->GetSlotState( vehicleID ) )
	{
		CVehicle* vehicle = __NetGame->vehiclePool->GetVehicle( vehicleID );
		if( vehicle == 0 ) return 0;
		

		RakNet::BitStream bStream;
		bStream.Write( ( uint8_t ) 0 );
		bStream.Write( amx_ftoc( a_Params[ 2 ] ) );
		bStream.Write( amx_ftoc( a_Params[ 3 ] ) );
		bStream.Write( amx_ftoc( a_Params[ 4 ] ) );
		__NetGame->PlayerRPC( RPC_SetVehicleVelocity, &bStream, vehicle->vehDriverID, 2 );
		return 1;
	}
	return 0;
//	return _funcSetVehicleVelocity ( a_AmxInterface, a_Params );
}
cell AMX_NATIVE_CALL funcSetVehicleAngularVelocity ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcSetVehicleAngularVelocity()" );
	CHECK_PARAMS( 5 );

	_VehicleID vehicleID = ( _VehicleID ) a_Params[ 1 ];

	if( __NetGame->vehiclePool->GetSlotState( vehicleID ) )
	{
		CVehicle* vehicle = __NetGame->vehiclePool->GetVehicle( vehicleID );
		if( vehicle == 0 ) return 0;

		RakNet::BitStream bStream;
		bStream.Write( ( uint8_t ) 1 );
		bStream.Write( amx_ftoc( a_Params[ 2 ] ) );
		bStream.Write( amx_ftoc( a_Params[ 3 ] ) );
		bStream.Write( amx_ftoc( a_Params[ 4 ] ) );
		__NetGame->PlayerRPC( RPC_SetVehicleVelocity, &bStream, vehicle->vehDriverID, 2 );
		return 1;
	}
	return 0;
//	return _funcSetVehicleAngularVelocity ( a_AmxInterface, a_Params );
}
cell AMX_NATIVE_CALL funcGetVehicleDamageStatus ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcGetVehicleDamageStatus()" );
	CHECK_PARAMS( 5 );

	_VehicleID vehicleID = ( _VehicleID ) a_Params[ 1 ];

	if( __NetGame->vehiclePool->GetSlotState( vehicleID ) )
	{
		CVehicle* vehicle = __NetGame->vehiclePool->GetVehicle( vehicleID );
		if( vehicle == 0 ) return 0;
		cell* ptr = 0;

		amx_GetAddr( a_AmxInterface, a_Params[ 2 ], &ptr );
		*ptr = vehicle->vehPanelStatus;
		amx_GetAddr( a_AmxInterface, a_Params[ 3 ], &ptr );
		*ptr = vehicle->vehDoorStatus;
		amx_GetAddr( a_AmxInterface, a_Params[ 4 ], &ptr );
		*ptr = vehicle->vehLightStatus;
		amx_GetAddr( a_AmxInterface, a_Params[ 5 ], &ptr );
		*ptr = vehicle->vehTireStatus;
		return 1;
	}
	return 0;
//	return _funcGetVehicleDamageStatus ( a_AmxInterface, a_Params );
}
cell AMX_NATIVE_CALL funcUpdateVehicleDamageStatus ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcUpdateVehicleDamageStatus()" );

	CHECK_PARAMS( 5 );

	_VehicleID vehicleID = ( _VehicleID ) a_Params[ 1 ];

	if( __NetGame->vehiclePool->GetSlotState( vehicleID ) )
	{
		CVehicle* vehicle = __NetGame->vehiclePool->GetVehicle( vehicleID );
		if( vehicle == 0 ) return 0;
		vehicle->UpdateDamage( 0xFFFF, a_Params[2], a_Params[3], (uint8_t)a_Params[4], (uint8_t)a_Params[5] );
		return 1;
	}
	return 0;	
	
	
	//	return _funcUpdateVehicleDamageStatus ( a_AmxInterface, a_Params );
}
cell AMX_NATIVE_CALL funcSetVehicleVirtualWorld ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcSetVehicleVirtualWorld()" );
	CHECK_PARAMS( 2 );
	_VehicleID vehicleID = (_VehicleID)a_Params[ 1 ];

	if( __NetGame->vehiclePool->GetSlotState( vehicleID ) )
	{
		__NetGame->vehiclePool->vehiclesVirtualWorlds[ vehicleID ] = a_Params[ 2 ];
		return 1;
	}
	return 0;
	//return _funcSetVehicleVirtualWorld ( a_AmxInterface, a_Params );
}
cell AMX_NATIVE_CALL funcGetVehicleVirtualWorld ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcGetVehicleVirtualWorld()" );
	CHECK_PARAMS( 2 );
	_VehicleID vehicleID = (_VehicleID)a_Params[ 1 ];

	if( __NetGame->vehiclePool->GetSlotState( vehicleID ) )
	{
		return __NetGame->vehiclePool->vehiclesVirtualWorlds[ vehicleID ];
	}
	return 0;
//	return _funcGetVehicleVirtualWorld ( a_AmxInterface, a_Params );
}
cell AMX_NATIVE_CALL funcSendClientMessage ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcSendClientMessage()" );
	CHECK_PARAMS( 3 );
	_PlayerID playerID = ( _PlayerID ) a_Params[ 1 ];

	if( __NetGame->playerPool->GetSlotState( playerID ) )
	{
		char* str = 0;
		amx_StrParam( a_AmxInterface, a_Params[ 3 ], str );
		__NetGame->SendClientMessage( playerID, a_Params[ 2 ], str );
		return 1;
	}
	return 0;
	//return _funcSendClientMessage ( a_AmxInterface, a_Params );
}
cell AMX_NATIVE_CALL funcSendClientMessageToAll ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcSendClientMessageToAll()" );
	CHECK_PARAMS( 2 );

	char* str = 0;
	amx_StrParam( a_AmxInterface, a_Params[ 2 ], str );
	__NetGame->SendClientMessageToAll( a_Params[ 1 ], str );
	return 1;
//	return _funcSendClientMessageToAll ( a_AmxInterface, a_Params );
}
cell AMX_NATIVE_CALL funcSendDeathMessage ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcSendDeathMessage()" );
	CHECK_PARAMS( 3 );
	RakNet::BitStream bStream;
		
	bStream.Write( ( _PlayerID ) a_Params[ 1 ] );
	bStream.Write( ( _PlayerID ) a_Params[ 2 ] );
	bStream.Write( ( uint8_t ) a_Params[ 3 ] );

	__NetGame->GlobalRPC( RPC_SendDeathMessage, &bStream, 0xFFFF, 3 );
	return 1;
//	return _funcSendDeathMessage ( a_AmxInterface, a_Params );
}
cell AMX_NATIVE_CALL funcGameTextForAll ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcGameTextForAll()" );
//	return _funcGameTextForAll ( a_AmxInterface, a_Params );
	if( __NetGame->playerPool == 0 ) return 0;
	if( a_Params[ 1 ] >= MAX_PLAYERS ) return 0;

	uint32_t text_len = 0, style = a_Params[3], duration = a_Params[4];
	char* text = 0;

	amx_StrParam( a_AmxInterface, a_Params[2], text );

	if( ( text_len = strlen( text ) ) == 0 ) text = 0;

	RakNet::BitStream bStream;
	bStream.Write( (uint32_t)style );
	bStream.Write( (uint32_t)duration );
	bStream.Write( (uint32_t)text_len );
	bStream.Write( (char*)text, text_len );

	__NetGame->GlobalRPC( RPC_ShowGameTextForPlayer, &bStream, 0xFFFF, 3 );
	return 1;
}
cell AMX_NATIVE_CALL funcGameTextForPlayer ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcGameTextForPlayer()" );
	//return _funcGameTextForPlayer ( a_AmxInterface, a_Params );

	if( __NetGame->playerPool == 0 ) return 0;
	if( a_Params[ 1 ] >= MAX_PLAYERS ) return 0;

	uint32_t text_len = 0, style = a_Params[3], duration = a_Params[4];
	char* text = 0;

	amx_StrParam( a_AmxInterface, a_Params[2], text );

	if( ( text_len = strlen( text ) ) == 0 ) text = 0;

	RakNet::BitStream bStream;
	bStream.Write( (uint32_t)style );
	bStream.Write( (uint32_t)duration );
	bStream.Write( (uint32_t)text_len );
	bStream.Write( (char*)text, text_len );

	__NetGame->PlayerRPC( RPC_ShowGameTextForPlayer, &bStream, (_PlayerID)a_Params[1], 3 );

	return 1;
}


cell AMX_NATIVE_CALL funcSendPlayerMessageToPlayer ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcSendPlayerMessageToPlayer()" );
	CHECK_PARAMS( 3 );

	_PlayerID playerID = ( _PlayerID ) a_Params[ 1 ], senderID = ( _PlayerID ) a_Params[ 2 ];
	char* str = 0;
	amx_StrParam( a_AmxInterface, a_Params[ 3 ], str );

	if( str == 0 ) return 0;

	RakNet::BitStream bStream;
	bStream.Write( ( _PlayerID ) senderID );
	bStream.Write( ( uint8_t ) strlen( str ) );
	bStream.Write( ( char*) str, strlen( str ) );
	__NetGame->PlayerRPC( RPC_AddPlayerMessage, &bStream, playerID, 3 );
	return 0;
//	return _funcSendPlayerMessageToPlayer ( a_AmxInterface, a_Params );
}
cell AMX_NATIVE_CALL funcSendPlayerMessageToAll ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcSendPlayerMessageToAll()" );


	CHECK_PARAMS( 2 );

	_PlayerID senderID = ( _PlayerID ) a_Params[ 1 ];
	char* str = 0;
	amx_StrParam( a_AmxInterface, a_Params[ 2 ], str );

	if( str == 0 ) return 0;


	if( __NetGame->playerPool->GetSlotState( senderID ) )
	{
		CPlayer* player = __NetGame->playerPool->GetPlayer( senderID );
		if( player == 0 ) return 0;

		player->Say( str, strlen( str ) );

		return 1;
	}
	return 0;

	//return _funcSendPlayerMessageToAll ( a_AmxInterface, a_Params );
}



cell AMX_NATIVE_CALL funcSetPlayerTime ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcSetPlayerTime()" );
	CHECK_PARAMS( 3 );
	_PlayerID playerID = ( _PlayerID ) a_Params[ 1 ];

	if( __NetGame->playerPool->GetSlotState( playerID ) )
	{
		CPlayer* player = __NetGame->playerPool->GetPlayer( playerID );
		if( player == 0 ) return 0;
		player->SetTime( ( uint8_t ) a_Params[ 2 ], ( uint8_t ) a_Params[ 3 ] );
		return 1;
	}
	return 0;
//	return _funcSetPlayerTime ( a_AmxInterface, a_Params );
}
cell AMX_NATIVE_CALL funcTogglePlayerClock ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcTogglePlayerClock()" );
	CHECK_PARAMS( 2 );
	_PlayerID playerID = ( _PlayerID ) a_Params[ 1 ];
	if( __NetGame->playerPool->GetSlotState( playerID ) )
	{
		CPlayer* player = __NetGame->playerPool->GetPlayer( playerID );
		if( player == 0 ) return 0;

		player->ToggleClock( ( bool )( a_Params[ 2 ] ? true : false ) );

		return 1;
	}
	return 0;
//	return _funcTogglePlayerClock ( a_AmxInterface, a_Params );
}
cell AMX_NATIVE_CALL funcGetPlayerTime ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcGetPlayerTime()" );
	CHECK_PARAMS( 3 );

	if(!__NetGame || __NetGame->playerPool) return 0;

	CPlayer* player = __NetGame->playerPool->GetPlayer((_PlayerID)a_Params[1]);

	if (player) 
	{
		int iTime;
		if (player->hasTime())
		{
			iTime = (int)player->getTime();
		}
		else
		{
			iTime = ((int)__NetGame->worldTime) * 60;
		}

		cell* cptr;
			
		amx_GetAddr(a_AmxInterface, a_Params[2], &cptr);
		*cptr = (cell)(iTime / 60);
		amx_GetAddr(a_AmxInterface, a_Params[3], &cptr);
		*cptr = (cell)(iTime % 60);

		return 1;
	} 

	return 0;
}
cell AMX_NATIVE_CALL funcHTTP ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcHTTP()" );
	CHECK_PARAMS( 5 );

	uint32_t index = a_Params[1];
	uint32_t type = a_Params[2];

	char* url, *data, *callback;
	amx_StrParam( a_AmxInterface, a_Params[3], url );
	amx_StrParam( a_AmxInterface, a_Params[4], data );
	amx_StrParam( a_AmxInterface, a_Params[5], callback );

	if(callback[0] == NULL) 
		__NetGame->scriptHttpManager->Start(index, type, url, data, 0, 1, a_AmxInterface, callback);
	else
		if(strlen(callback)) __NetGame->scriptHttpManager->Start(index, type, url, data, 0, 0, a_AmxInterface, callback);

	return 1;
}

cell AMX_NATIVE_CALL funcGetPlayerDistanceFromPoint ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcGetPlayerDistanceFromPoint()" );
	CHECK_PARAMS( 4 );

	float distance = -1.0f;
	_PlayerID playerID = ( _PlayerID ) a_Params[ 1 ];

	if(__NetGame->playerPool->GetSlotState( playerID ) )
	{
		CPlayer* player = __NetGame->playerPool->GetPlayer( playerID );
		if( player ) return amx_ftoc( distance );
		distance = player->GetDistanceFrom3DPoint( amx_ctof( a_Params[ 2 ] ), amx_ctof( a_Params[ 3 ] ), amx_ctof( a_Params[ 4 ] ) );
	}
	return amx_ftoc( distance );
}

cell AMX_NATIVE_CALL funcGetVehicleDistanceFromPoint ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcGetVehicleDistanceFromPoint()" );
	CHECK_PARAMS( 4 );
	float distance = -1.0f;
	_VehicleID vehicleID = ( _VehicleID )a_Params[ 1 ];

	if( __NetGame->vehiclePool->GetSlotState( vehicleID ) )
	{
		CVehicle* vehicle = __NetGame->vehiclePool->GetVehicle( vehicleID );
		if( vehicle == 0 ) return amx_ftoc( distance );
		distance = vehicle->GetDistanceFromPoint( amx_ctof( a_Params[ 2 ] ), amx_ctof( a_Params[ 3 ] ), amx_ctof( a_Params[ 4 ] ) );
	}
	return amx_ftoc( distance );
}

cell AMX_NATIVE_CALL funcGetPlayerSurfingObjectID( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcGetPlayerSurfingObjectID()" );
	CHECK_PARAMS( 1 );

	_PlayerID playerID = ( _PlayerID ) a_Params[ 1 ];

	if( __NetGame->playerPool->GetSlotState( playerID ) )
	{
		CPlayer* player = __NetGame->playerPool->GetPlayer( playerID );
		if( player == 0 ) return 0;

		if( player->getState( ) == PLAYER_STATE_ONFOOT )
		{
			if( player->getOnFootSyncData()->surfingVehicleID >= MAX_VEHICLES && player->getOnFootSyncData()->surfingVehicleID < MAX_VEHICLES + MAX_OBJECTS )
			{
				return ( cell )( player->getOnFootSyncData()->surfingVehicleID - MAX_VEHICLES );
			}
		}
	}
	return 0xFFFF;
}

cell AMX_NATIVE_CALL funcGetPlayerCameraMode( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcGetPlayerCameraMode()" );
	CHECK_PARAMS( 1 );
	_PlayerID playerID = ( _PlayerID ) a_Params[ 1 ];

	if( __NetGame->playerPool->GetSlotState( playerID ) )
	{
		CPlayer* player = __NetGame->playerPool->GetPlayer( playerID );
		if( player == 0 ) return 0;
		return ( cell )( player->getAimSyncData()->cameraMode );
	}
	return -1;
}

cell AMX_NATIVE_CALL funcGetNetworkStats ( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcGetNetworkStats()" );
	CHECK_PARAMS( 2 );

	if(!__NetGame) return 0;

	char* szSvrDispBuf = new char[a_Params[ 3 ] + 1];
	RakNetStatisticsStruct* RakServerStats;

	RakServerStats = __NetGame->rakServerInterface->GetStatistics(__NetGame->rakServerInterface->GetInternalID());

	StatisticsToString(RakServerStats, szSvrDispBuf, 1);

	cell* dest = 0;
	amx_GetAddr( a_AmxInterface, a_Params[2], &dest );
	amx_SetString( dest, szSvrDispBuf, 0, 0, a_Params[ 3 ] );

	delete [] szSvrDispBuf;

	return 1;
}

cell AMX_NATIVE_CALL funcAddRule( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcAddRule()" );
	CHECK_PARAMS( 2 );

	char *ruleName = 0, *ruleValue = 0;
	amx_StrParam( a_AmxInterface, a_Params[ 1 ], ruleName );
	amx_StrParam( a_AmxInterface, a_Params[ 2 ], ruleValue );

	if( __Console->FindVar( ruleName ) == 0 )
	{
		__Console->AddStringVar( ruleName, CONSOLE_VARFLAG_RULE, ruleValue );
		return 1;
	}
	return 0;
}


cell AMX_NATIVE_CALL funcEditRule( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcEditRule()" );
	CHECK_PARAMS( 2 );

	char *ruleName = 0, *ruleValue = 0;
	amx_StrParam( a_AmxInterface, a_Params[ 1 ], ruleName );
	amx_StrParam( a_AmxInterface, a_Params[ 2 ], ruleValue );
	consoleVarStruct* var = __Console->FindVar( ruleName );
	if( var != 0 )
	{
		if( ( var->varFlags & CONSOLE_VARFLAG_RULE ) == CONSOLE_VARFLAG_RULE )
		{
			__Console->SetStringVar( ruleName, ruleValue );
			return 1;
		}
	}
	return 0;
}

cell AMX_NATIVE_CALL funcRemoveRule( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcRemoveRule()" );
	CHECK_PARAMS( 1 );
	char* ruleName = 0;
	amx_StrParam( a_AmxInterface, a_Params[ 1 ], ruleName );
	consoleVarStruct* var = __Console->FindVar( ruleName );
	if( var != 0 )
	{
		if( ( var->varFlags & CONSOLE_VARFLAG_UNREMOVABLE ) != CONSOLE_VARFLAG_UNREMOVABLE )
		{
			__Console->RemoveVar( ruleName );
			return 1;
		}
	}
	return 0;
}

//cell AMX_NATIVE_CALL funcTogglePlayerWideScreen( AMX* a_AmxInterface, cell* a_Params )
//{
//	if(bScriptDebug) logprintf ( "[Call]-> funcTogglePlayerWideScreen()" );
//	CHECK_PARAMS( 2 );
//	_PlayerID playerID = ( _PlayerID ) a_Params[ 1 ];
//
//	if( __NetGame->playerPool->GetSlotState( playerID ) )
//	{
//		RakNet::BitStream bStream;
//		bStream.Write( ( uint8_t )( a_Params[ 2 ] == 1 ) );
//		__NetGame->PlayerRPC( RPC_ToggleWidescreen, &bStream, playerID );
//
//	}
//	return 0;
//}

cell AMX_NATIVE_CALL funcCreatePlayerExplosion( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcCreatePlayerExplosion()" );
	CHECK_PARAMS( 6 );
	_PlayerID playerID = ( _PlayerID )a_Params[ 1 ];

	if( __NetGame->playerPool->GetSlotState( playerID ) )
	{
		RakNet::BitStream bStream;

		bStream.Write( amx_ctof( a_Params[2] )); // x
		bStream.Write( amx_ctof( a_Params[3] )); // y
		bStream.Write( amx_ctof( a_Params[4] )); // z
		bStream.Write( a_Params[5] ); // type
		bStream.Write( amx_ctof( a_Params[6] )); // radius
		__NetGame->PlayerRPC( RPC_CreateExplosion, &bStream, playerID );
		return 1;
	}
	return 0;
}
cell AMX_NATIVE_CALL funcPlayAudioStreamForPlayer( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcPlayAudioStreamForPlayer()" );
	CHECK_PARAMS( 7 );
	_PlayerID playerID = ( _PlayerID ) a_Params[ 1 ];

	if( __NetGame->playerPool->GetSlotState( playerID ) )
	{
		char* streamURL = 0;
		amx_StrParam( a_AmxInterface, a_Params[ 2 ], streamURL );
		
		if( streamURL )
		{
			RakNet::BitStream bStream;
			bStream.Write( ( uint8_t )strlen( streamURL ) );		// url length
			bStream.Write( streamURL, strlen( streamURL ) );		// url
			bStream.Write( ( float ) amx_ctof( a_Params[ 3 ] ) );	// posX
			bStream.Write( ( float ) amx_ctof( a_Params[ 4 ] ) );	// posY
			bStream.Write( ( float ) amx_ctof( a_Params[ 5 ] ) );	// posZ
			bStream.Write( ( float ) amx_ctof( a_Params[ 6 ] ) );	// distance
			bStream.Write( ( uint8_t ) a_Params[ 7 ] );				// usePos

			__NetGame->PlayerRPC( RPC_PlayAudioStream, &bStream, playerID );
			return 1;
		}
	}
	return 0;
}
cell AMX_NATIVE_CALL funcStopAudioStreamForPlayer( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcStopAudioStreamForPlayer()" );
	CHECK_PARAMS( 1 );

	_PlayerID playerID = ( _PlayerID ) a_Params[ 1 ];

	if( __NetGame->playerPool->GetSlotState( playerID ) )
	{
		RakNet::BitStream bStream;
		__NetGame->PlayerRPC( RPC_StopAudioStream, &bStream, playerID );
		return 1;
	}
	return 0;
}


cell AMX_NATIVE_CALL funcPlayAudioStreamForAll( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcPlayAudioStreamForAll()" );
	CHECK_PARAMS( 6 );

	char* streamURL = 0;
	amx_StrParam( a_AmxInterface, a_Params[ 1 ], streamURL );
		
	if( streamURL )
	{
		RakNet::BitStream bStream;
		bStream.Write( ( uint8_t )strlen( streamURL ) );		// url length
		bStream.Write( streamURL, strlen( streamURL ) );		// url
		bStream.Write( ( float ) amx_ctof( a_Params[ 2 ] ) );	// posX
		bStream.Write( ( float ) amx_ctof( a_Params[ 3 ] ) );	// posY
		bStream.Write( ( float ) amx_ctof( a_Params[ 4 ] ) );	// posZ
		bStream.Write( ( float ) amx_ctof( a_Params[ 5 ] ) );	// distance
		bStream.Write( ( uint8_t ) a_Params[ 6 ] );				// usePos
		__NetGame->GlobalRPC( RPC_PlayAudioStream, &bStream );
		return 1;
	}
	return 0;
}
cell AMX_NATIVE_CALL funcStopAudioStreamForAll( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcStopAudioStreamForAll()" );
	CHECK_PARAMS( 0 );

	RakNet::BitStream bStream;
	__NetGame->GlobalRPC( RPC_StopAudioStream, &bStream );
	return 1;
}

cell AMX_NATIVE_CALL funcRemoveBuildingForPlayer( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcRemoveBuildingForPlayer()" );
	CHECK_PARAMS( 6 );

	_PlayerID playerID = ( _PlayerID ) a_Params[ 1 ];

	if( __NetGame->playerPool->GetSlotState( playerID ) )
	{
		RakNet::BitStream bStream;

		bStream.Write( ( uint32_t ) a_Params[ 2 ] ); // modelid
		bStream.Write( ( float ) amx_ctof( a_Params[ 3 ]  ) ); // posX
		bStream.Write( ( float ) amx_ctof( a_Params[ 4 ]  ) ); // posY
		bStream.Write( ( float ) amx_ctof( a_Params[ 5 ]  ) ); // posZ
		bStream.Write( ( float ) amx_ctof( a_Params[ 6 ]  ) ); // radius

		__NetGame->PlayerRPC( RPC_RemoveBuilding, &bStream, playerID );

		return 1;
	}
	return 0;
}


cell AMX_NATIVE_CALL funcGetVehicleColor( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcGetVehicleColor()" );
	CHECK_PARAMS( 3 );

	_VehicleID vehicleID = ( _VehicleID ) a_Params[ 1 ];
	CVehicle* vehicle = __NetGame->vehiclePool->GetVehicle( vehicleID );
	
	if( vehicle )
	{
		uint32_t color1 = 0, color2 = 0;
		vehicle->GetVehicleColor( color1, color2 );
		cell* ptr = 0;
		amx_GetAddr( a_AmxInterface, a_Params[ 2 ], &ptr );
		*ptr = color1;
		amx_GetAddr( a_AmxInterface, a_Params[ 3 ], &ptr );
		*ptr = color2;
		return 1;
	}
	return 0;
}

cell AMX_NATIVE_CALL funcTogglePlayerCollision( AMX* a_AmxInterface, cell* a_Params )
{
	/* forplayerID, playerID, toggle */
	if(bScriptDebug) logprintf ( "[Call]-> funcTogglePlayerCollision()" );
	CHECK_PARAMS( 3 );
	// INVALID_PLAYER_ID for playerCollisionID will toggle collision for all players
	_PlayerID playerID = ( _PlayerID )( a_Params[ 1 ] ), playerCollisionID = ( _PlayerID )a_Params[ 2 ];

	if( playerCollisionID >= MAX_PLAYERS ) playerCollisionID = INVALID_PLAYER_ID; 

	if( __NetGame->playerPool->GetSlotState( playerID ) )
	{
		RakNet::BitStream bStream;
		bStream.Write( playerID );
		bStream.Write( playerCollisionID );
		bStream.Write( (bool)( a_Params[3] != 0 ) );
		__NetGame->GlobalRPC( RPC_TogglePlayerCollision, &bStream );
		return 1;
	}
	return 0;
}

cell AMX_NATIVE_CALL funcToggleVehicleCollision( AMX* a_AmxInterface, cell* a_Params )
{
	/* vehicleid, toggle */

	if(bScriptDebug) logprintf ( "[Call]-> funcToggleVehicleCollision()" );
	CHECK_PARAMS( 2 );

	_VehicleID vehicleID = ( _VehicleID )( a_Params[ 1 ] );

	if( __NetGame->vehiclePool->GetSlotState( vehicleID ) )
	{
		RakNet::BitStream bStream;
		bStream.Write( vehicleID );
		bStream.Write( (bool)( a_Params[2] != 0 ) );
	
		__NetGame->GlobalRPC( RPC_ToggleVehicleCollision, &bStream );
		return 1;
	}
	return 0;
}

cell AMX_NATIVE_CALL funcToggleVehicleCollisionForPlayer( AMX* a_AmxInterface, cell* a_Params )
{
	/* playerid, vehicleid, toggle */

	if(bScriptDebug) logprintf ( "[Call]-> funcToggleVehicleCollisionForPlayer()" );
	CHECK_PARAMS( 2 );

	_PlayerID playerID = ( _VehicleID )( a_Params[ 1 ] );
	_VehicleID vehicleID = ( _VehicleID )( a_Params[ 2 ] );

	if( __NetGame->vehiclePool->GetSlotState( vehicleID ) && __NetGame->playerPool->GetSlotState( playerID ) )
	{
		RakNet::BitStream bStream;
		bStream.Write( vehicleID );
		bStream.Write( (bool)( a_Params[3] != 0 ) );
	
		__NetGame->PlayerRPC( RPC_ToggleVehicleCollision, &bStream, playerID );
		return 1;
	}
	return 0;
}

cell AMX_NATIVE_CALL funcGetPlayerTargetPlayer( AMX* a_AmxInterface, cell* a_Params )
{
	if(bScriptDebug) logprintf ( "[Call]-> funcGetPlayerTargetPlayer()" );
	CHECK_PARAMS( 1 );
	_PlayerID playerID = ( _PlayerID ) a_Params[ 1 ];
	if( __NetGame->playerPool->GetSlotState( playerID ) )
	{
		CPlayer* player = __NetGame->playerPool->GetPlayer( playerID );
		if( player == 0 ) return INVALID_PLAYER_ID;

		return player->GetTargettedPlayerID( );
	}
	return INVALID_PLAYER_ID;
}




/**
 *	Reference:
 *		[CALL] 0x004807B7
 *		[CALL] 0x00480902
 *		[CALL] 0x00483E7F
 *
.text:0048A7D0 amx_CustomInit  proc near               ; CODE XREF: CFilterScripts__LoadFilterScript+117p
.text:0048A7D0                                         ; CFilterScripts__LoadFilterScriptFromMemory+C2p ...
.text:0048A7D0
.text:0048A7D0 a_AmxInstance   = uint32_t ptr  4
.text:0048A7D0
.text:0048A7D0                 mov     eax, [esp+a_AmxInstance]
.text:0048A7D4                 push    0FFFFFFFFh      ; a_Number
.text:0048A7D6                 push    offset __ArrayCustomNative ; a_NativeInfoList
.text:0048A7DB                 push    eax             ; a_AmxInstance
.text:0048A7DC                 call    amx_Register
.text:0048A7E1                 add     esp, 0Ch
.text:0048A7E4                 retn
.text:0048A7E4 amx_CustomInit  endp
**/

AMX_NATIVE_INFO __AmxScriptNative[] =
{
	{ "print",									funcPrint },								// 0
	{ "printf",									funcPrintf },								// 1
	{ "format",									funcFormat },								// 2
	{ "SetTimer",								funcSetTimer },								// 3
	{ "KillTimer",								funcKillTimer },							// 4
	{ "GetTickCount",							funcGetTickCount },							// 5
	{ "GetMaxPlayers",							funcGetMaxPlayers },						// 6
	{ "SetTimerEx",								funcSetTimerEx },							// 7
	{ "LimitGlobalChatRadius",					funcLimitGlobalChatRadius },				// 8
	{ "LimitPlayerMarkerRadius",				funcLimitPlayerMarkerRadius },				// 9
	{ "SetWeather",								funcSetWeather },							// 10
	{ "SetPlayerWeather",						funcSetPlayerWeather },						// 11
	{ "CallRemoteFunction",						funcCallRemoteFunction },					// 12
	{ "CallLocalFunction",						funcCallLocalFunction },					// 13
	{ "asin",									funcAsin },									// 14
	{ "acos",									funcAcos },									// 15
	{ "atan2",									funcAtan2 },								// 16
	{ "atan",									funcAtan },									// 17
	{ "gpci",									funcGpci },									// 18
	{ "GameModeExit",							funcGameModeExit },							// 19
	{ "SetGameModeText",						funcSetGameModeText },						// 20
	{ "SetTeamCount",							funcSetTeamCount },							// 21
	{ "AddPlayerClass",							funcAddPlayerClass },						// 22
	{ "AddPlayerClassEx",						funcAddPlayerClassEx },						// 23
	{ "AddStaticVehicle",						funcAddStaticVehicle },						// 24
	{ "AddStaticVehicleEx",						funcAddStaticVehicleEx },					// 25
	{ "AddStaticPickup",						funcAddStaticPickup },						// 26
	{ "CreatePickup",							funcCreatePickup },							// 27
	{ "DestroyPickup",							funcDestroyPickup },						// 28
	{ "SetPlayerWorldBounds",					funcSetPlayerWorldBounds },					// 29
	{ "ShowNameTags",							funcShowNameTags },							// 30
	{ "ShowPlayerMarkers",						funcShowPlayerMarkers },					// 31
	{ "SetWorldTime",							funcSetWorldTime },							// 32
	{ "GetWeaponName",							funcGetWeaponName },						// 33
	{ "EnableTirePopping",						funcEnableTirePopping },					// 34
	{ "AllowInteriorWeapons",					funcAllowInteriorWeapons },					// 35
	{ "SetGravity",								funcSetGravity },							// 36
	{ "GetGravity",								funcGetGravity },							// 37
	{ "AllowAdminTeleport",						funcAllowAdminTeleport },					// 38
	{ "SetDeathDropAmount",						funcSetDeathDropAmount },					// 39
	{ "CreateExplosion",						funcCreateExplosion },						// 40
	{ "SetDisabledWeapons",						funcSetDisabledWeapons },					// 41
	{ "UsePlayerPedAnims",						funcUsePlayerPedAnims },					// 42
	{ "DisableInteriorEnterExits",				funcDisableInteriorEnterExits },			// 43
	{ "SetNameTagDrawDistance",					funcSetNameTagDrawDistance },				// 44
	{ "DisableNameTagLOS",						funcDisableNameTagLOS },					// 45
	{ "ConnectNPC",								funcConnectNPC },							// 46
	{ "IsPlayerNPC",							funcIsPlayerNPC },							// 47
	{ "Create3DTextLabel",						funcCreate3DTextLabel },					// 48
	{ "Delete3DTextLabel",						funcDelete3DTextLabel },					// 49
	{ "Attach3DTextLabelToPlayer",				funcAttach3DTextLabelToPlayer },			// 50
	{ "Attach3DTextLabelToVehicle",				funcAttach3DTextLabelToVehicle },			// 51
	{ "Update3DTextLabelText",					funcUpdate3DTextLabelText },				// 52
	{ "CreatePlayer3DTextLabel",				funcCreatePlayer3DTextLabel },				// 53
	{ "DeletePlayer3DTextLabel",				funcDeletePlayer3DTextLabel },				// 54
	{ "UpdatePlayer3DTextLabelText",			funcUpdatePlayer3DTextLabelText },			// 55
	{ "ShowPlayerDialog",						funcShowPlayerDialog },					// 56
	{ "EnableZoneNames",						funcEnableZoneNames },					// 57

	{ "GangZoneCreate",							funcGangZoneCreate },					// 58
	{ "GangZoneDestroy",						funcGangZoneDestroy },					// 59
	{ "GangZoneShowForPlayer",					funcGangZoneShowForPlayer },					// 60
	{ "GangZoneShowForAll",						funcGangZoneShowForAll },					// 61
	{ "GangZoneHideForPlayer",					funcGangZoneHideForPlayer },					// 62
	{ "GangZoneHideForAll",						funcGangZoneHideForAll },					// 63
	{ "GangZoneFlashForPlayer",					funcGangZoneFlashForPlayer },					// 64
	{ "GangZoneFlashForAll",					funcGangZoneFlashForAll },					// 65
	{ "GangZoneStopFlashForPlayer",				funcGangZoneStopFlashForPlayer },					// 66
	{ "GangZoneStopFlashForAll",				funcGangZoneStopFlashForAll },					// 67
	
	{ "IsPlayerAdmin",							funcIsPlayerAdmin },					// 68
	{ "Kick",									funcKick },					// 69
	{ "Ban",									funcBan },					// 70
	{ "BanEx",									funcBanEx },					// 71
	{ "SendRconCommand",						funcSendRconCommand },					// 72
	{ "GetServerVarAsString",					funcGetServerVarAsString },					// 73
	{ "GetServerVarAsInt",						funcGetServerVarAsInt },					// 74
	{ "GetServerVarAsBool",						funcGetServerVarAsBool },					// 75
	{ "SetSpawnInfo",							funcSetSpawnInfo },					// 76
	{ "SpawnPlayer",							funcSpawnPlayer },					// 77
	{ "SetPlayerTeam",							funcSetPlayerTeam },					// 78
	{ "GetPlayerTeam",					funcGetPlayerTeam },					// 79
	{ "SetPlayerName",					funcSetPlayerName },					// 80
	{ "SetPlayerSkin",					funcSetPlayerSkin },					// 81
	{ "GetPlayerSkin",					funcGetPlayerSkin },					// 82
	{ "GetPlayerPos",					funcGetPlayerPos },					// 83
	{ "SetPlayerPos",					funcSetPlayerPos },					// 84
	{ "SetPlayerPosFindZ",					funcSetPlayerPosFindZ },					// 85
	{ "GetPlayerHealth",					funcGetPlayerHealth },					// 86
	{ "SetPlayerHealth",					funcSetPlayerHealth },					// 87
	{ "SetPlayerColor",					funcSetPlayerColor },					// 88
	{ "GetPlayerColor",					funcGetPlayerColor },					// 89
	{ "GetPlayerVehicleID",					funcGetPlayerVehicleID },					// 90
	{ "GetPlayerVehicleSeat",					funcGetPlayerVehicleSeat },					// 91
	{ "GetPlayerSurfingVehicleID",					funcGetPlayerSurfingVehicleID },					// 92
	{ "PutPlayerInVehicle",					funcPutPlayerInVehicle },					// 93
	{ "RemovePlayerFromVehicle",					funcRemovePlayerFromVehicle },					// 94
	{ "IsPlayerInVehicle",					funcIsPlayerInVehicle },					// 95
	{ "IsPlayerInAnyVehicle",					funcIsPlayerInAnyVehicle },					// 96
	{ "GetPlayerName",					funcGetPlayerName },					// 97
	{ "SetPlayerCheckpoint",					funcSetPlayerCheckpoint },					// 98
	{ "DisablePlayerCheckpoint",					funcDisablePlayerCheckpoint },					// 99
	{ "IsPlayerInCheckpoint",					funcIsPlayerInCheckpoint },					// 100
	{ "SetPlayerRaceCheckpoint",					funcSetPlayerRaceCheckpoint },					// 101
	{ "DisablePlayerRaceCheckpoint",					funcDisablePlayerRaceCheckpoint },					// 102
	{ "IsPlayerInRaceCheckpoint",					funcIsPlayerInRaceCheckpoint },					// 103
	{ "SetPlayerInterior",					funcSetPlayerInterior },					// 104
	{ "GetPlayerInterior",					funcGetPlayerInterior },					// 105
	{ "SetPlayerAttachedObject",					funcSetPlayerAttachedObject },					// 106
	{ "RemovePlayerAttachedObject",					funcRemovePlayerAttachedObject },					// 107
	{ "SetPlayerAttachedObject",					funcSetPlayerAttachedObject },					// 108
	{ "IsPlayerAttachedObjectSlotUsed",					funcIsPlayerAttachedObjectSlotUsed },					// 109
	{ "SetPlayerCameraLookAt",					funcSetPlayerCameraLookAt },					// 110
	{ "SetPlayerCameraPos",					funcSetPlayerCameraPos },					// 111
	{ "SetCameraBehindPlayer",					funcSetCameraBehindPlayer },					// 112
	{ "GetPlayerCameraPos",					funcGetPlayerCameraPos },					// 113
	{ "GetPlayerCameraFrontVector",					funcGetPlayerCameraFrontVector },					// 114
	{ "TogglePlayerControllable",					funcTogglePlayerControllable },					// 115
	{ "PlayerPlaySound",					funcPlayerPlaySound },					// 116
	{ "SetPlayerScore",					funcSetPlayerScore },					// 117
	{ "GetPlayerScore",					funcGetPlayerScore },					// 118
	{ "SetPlayerFacingAngle",					funcSetPlayerFacingAngle },					// 119
	{ "GetPlayerFacingAngle",					funcGetPlayerFacingAngle },					// 120
	{ "GivePlayerMoney",					funcGivePlayerMoney },					// 121
	{ "GetPlayerMoney",					funcGetPlayerMoney },					// 122
	{ "ResetPlayerMoney",					funcResetPlayerMoney },					// 123
	{ "IsPlayerConnected",					funcIsPlayerConnected },					// 124
	{ "GetPlayerState",					funcGetPlayerState },					// 125
	{ "ResetPlayerWeapons",					funcResetPlayerWeapons },					// 126
	{ "GivePlayerWeapon",					funcGivePlayerWeapon },					// 127
	{ "SetPlayerArmedWeapon",					funcSetPlayerArmedWeapon },					// 128
	{ "GetPlayerIp",					funcGetPlayerIp },					// 129
	{ "GetPlayerPing",					funcGetPlayerPing },					// 130
	{ "GetPlayerWeapon",					funcGetPlayerWeapon },					// 131
	{ "SetPlayerArmour",					funcSetPlayerArmour },					// 132
	{ "GetPlayerArmour",					funcGetPlayerArmour },					// 133
	{ "SetPlayerMapIcon",					funcSetPlayerMapIcon },					// 134
	{ "RemovePlayerMapIcon",					funcRemovePlayerMapIcon },					// 135
	{ "GetPlayerKeys",					funcGetPlayerKeys },					// 136
	{ "SetPlayerMarkerForPlayer",					funcSetPlayerMarkerForPlayer },					// 137
	{ "GetPlayerAmmo",					funcGetPlayerAmmo },					// 138
	{ "SetPlayerAmmo",					funcSetPlayerAmmo },					// 139
	{ "GetPlayerWeaponData",					funcGetPlayerWeaponData },					// 140
	{ "GetPlayerWeaponState",					funcGetPlayerWeaponState },					// 141
	{ "AllowPlayerTeleport",					funcAllowPlayerTeleport },					// 142
	{ "ForceClassSelection",					funcForceClassSelection },					// 143
	{ "SetPlayerWantedLevel",					funcSetPlayerWantedLevel },					// 144
	{ "GetPlayerWantedLevel",					funcGetPlayerWantedLevel },					// 145
	{ "GetPlayerFightingStyle",					funcGetPlayerFightingStyle },					// 146
	{ "SetPlayerFightingStyle",					funcSetPlayerFightingStyle },					// 147
	{ "GetPlayerVelocity",					funcGetPlayerVelocity },					// 148
	{ "SetPlayerVelocity",					funcSetPlayerVelocity },					// 149
	{ "IsPlayerInRangeOfPoint",					funcIsPlayerInRangeOfPoint },					// 150
	{ "IsPlayerStreamedIn",					funcIsPlayerStreamedIn },					// 151
	{ "IsVehicleStreamedIn",					funcIsVehicleStreamedIn },					// 152
	{ "SetPlayerVirtualWorld",					funcSetPlayerVirtualWorld },					// 153
	{ "GetPlayerVirtualWorld",					funcGetPlayerVirtualWorld },					// 154
	{ "ShowPlayerNameTagForPlayer",					funcShowPlayerNameTagForPlayer },					// 155
	{ "EnableStuntBonusForAll",					funcEnableStuntBonusForAll },					// 156
	{ "EnableStuntBonusForPlayer",					funcEnableStuntBonusForPlayer },					// 157
	{ "TogglePlayerSpectating",					funcTogglePlayerSpectating },					// 158
	{ "GetPlayerDrunkLevel",					funcGetPlayerDrunkLevel },					// 159
	{ "SetPlayerDrunkLevel",					funcSetPlayerDrunkLevel },					// 160
	{ "PlayerSpectateVehicle",					funcPlayerSpectateVehicle },					// 161
	{ "PlayerSpectatePlayer",					funcPlayerSpectatePlayer },					// 162
	{ "ApplyAnimation",					funcApplyAnimation },					// 163
	{ "ClearAnimations",					funcClearAnimations },					// 164
	{ "GetPlayerAnimationIndex",					funcGetPlayerAnimationIndex },					// 165
	{ "GetAnimationName",					funcGetAnimationName },					// 166
	{ "SetPlayerSpecialAction",					funcSetPlayerSpecialAction },					// 167
	{ "GetPlayerSpecialAction",					funcGetPlayerSpecialAction },					// 168
	{ "StartRecordingPlayerData",					funcStartRecordingPlayerData },					// 169
	{ "StopRecordingPlayerData",					funcStopRecordingPlayerData },					// 170
	{ "PlayCrimeReportForPlayer",					funcPlayCrimeReportForPlayer },					// 171
	{ "SetPlayerShopName",					funcSetPlayerShopName },					// 172
	{ "SetPlayerSkillLevel",					funcSetPlayerSkillLevel },					// 173
	{ "SetPlayerChatBubble",					funcSetPlayerChatBubble },					// 174
	{ "SetPVarInt",					funcSetPVarInt },					// 175
	{ "SetPVarString",					funcSetPVarString },					// 176
	{ "SetPVarFloat",					funcSetPVarFloat },					// 177
	{ "GetPVarInt",					funcGetPVarInt },					// 178
	{ "GetPVarString",					funcGetPVarString },					// 179
	{ "GetPVarFloat",					funcGetPVarFloat },					// 180
	{ "DeletePVar",					funcDeletePVar },					// 181
	{ "GetPVarType",					funcGetPVarType },					// 182
	{ "GetPVarNameAtIndex",					funcGetPVarNameAtIndex },					// 183
	{ "GetPVarsUpperIndex",					funcGetPVarsUpperIndex },					// 184
	{ "IsValidVehicle",					funcIsValidVehicle },					// 185
	{ "CreateVehicle",					funcCreateVehicle },					// 186
	{ "DestroyVehicle",					funcDestroyVehicle },					// 187
	{ "GetVehiclePos",					funcGetVehiclePos },					// 188
	{ "SetVehiclePos",					funcSetVehiclePos },					// 189
	{ "GetVehicleZAngle",					funcGetVehicleZAngle },					// 190
	{ "GetVehicleRotationQuat",					funcGetVehicleRotationQuat },					// 191
	{ "SetVehicleZAngle",					funcSetVehicleZAngle },					// 192
	{ "SetVehicleParamsForPlayer",					funcSetVehicleParamsForPlayer },					// 193
	{ "ManualVehicleEngineAndLights",					funcManualVehicleEngineAndLights },					// 194
	{ "SetVehicleParamsEx",					funcSetVehicleParamsEx },					// 195
	{ "GetVehicleParamsEx",					funcGetVehicleParamsEx },					// 196
	{ "SetVehicleToRespawn",					funcSetVehicleToRespawn },					// 197
	{ "RepairVehicle",					funcRepairVehicle },					// 198
	{ "AddVehicleComponent",					funcAddVehicleComponent },					// 199
	{ "RemoveVehicleComponent",					funcRemoveVehicleComponent },					// 200
	{ "ChangeVehicleColor",					funcChangeVehicleColor },					// 201
	{ "ChangeVehiclePaintjob",					funcChangeVehiclePaintjob },					// 202
	{ "LinkVehicleToInterior",					funcLinkVehicleToInterior },					// 203
	{ "SetVehicleHealth",					funcSetVehicleHealth },					// 204
	{ "GetVehicleHealth",					funcGetVehicleHealth },					// 205
	{ "AttachTrailerToVehicle",					funcAttachTrailerToVehicle },					// 206
	{ "DetachTrailerFromVehicle",					funcDetachTrailerFromVehicle },					// 207
	{ "IsTrailerAttachedToVehicle",					funcIsTrailerAttachedToVehicle },					// 208
	{ "GetVehicleTrailer",					funcGetVehicleTrailer },					// 209
	{ "SetVehicleNumberPlate",					funcSetVehicleNumberPlate },					// 210
	{ "GetVehicleModel",					funcGetVehicleModel },					// 211
	{ "GetVehicleComponentInSlot",					funcGetVehicleComponentInSlot },					// 212
	{ "GetVehicleComponentType",					funcGetVehicleComponentType },					// 213
	{ "GetVehicleVelocity",					funcGetVehicleVelocity },					// 214
	{ "SetVehicleVelocity",					funcSetVehicleVelocity },					// 215
	{ "SetVehicleAngularVelocity",					funcSetVehicleAngularVelocity },					// 216
	{ "GetVehicleDamageStatus",					funcGetVehicleDamageStatus },					// 217
	{ "UpdateVehicleDamageStatus",					funcUpdateVehicleDamageStatus },					// 218
	{ "SetVehicleVirtualWorld",					funcSetVehicleVirtualWorld },					// 219
	{ "GetVehicleVirtualWorld",					funcGetVehicleVirtualWorld },					// 220
	{ "SendClientMessage",					funcSendClientMessage },					// 221
	{ "SendClientMessageToAll",					funcSendClientMessageToAll },					// 222
	{ "SendDeathMessage",					funcSendDeathMessage },					// 223
	{ "GameTextForAll",					funcGameTextForAll },					// 224
	{ "GameTextForPlayer",					funcGameTextForPlayer },					// 225
	{ "SendPlayerMessageToPlayer",					funcSendPlayerMessageToPlayer },					// 226
	{ "SendPlayerMessageToAll",					funcSendPlayerMessageToAll },					// 227
	{ "TextDrawCreate",					funcTextDrawCreate },					// 228
	{ "TextDrawSetString",					funcTextDrawSetString },					// 229
	{ "TextDrawLetterSize",					funcTextDrawLetterSize },					// 230
	{ "TextDrawTextSize",					funcTextDrawTextSize },					// 231
	{ "TextDrawAlignment",					funcTextDrawAlignment },					// 232
	{ "TextDrawColor",					funcTextDrawColor },					// 233
	{ "TextDrawUseBox",					funcTextDrawUseBox },					// 234
	{ "TextDrawBoxColor",					funcTextDrawBoxColor },					// 235
	{ "TextDrawSetShadow",					funcTextDrawSetShadow },					// 236
	{ "TextDrawSetOutline",					funcTextDrawSetOutline },					// 237
	{ "TextDrawBackgroundColor",					funcTextDrawBackgroundColor },					// 238
	{ "TextDrawFont",					funcTextDrawFont },					// 239
	{ "TextDrawSetProportional",					funcTextDrawSetProportional },					// 240
	{ "TextDrawShowForPlayer",					funcTextDrawShowForPlayer },					// 241
	{ "TextDrawShowForAll",					funcTextDrawShowForAll },					// 242
	{ "TextDrawHideForPlayer",					funcTextDrawHideForPlayer },					// 243
	{ "TextDrawHideForAll",					funcTextDrawHideForAll },					// 244
	{ "TextDrawDestroy",					funcTextDrawDestroy },					// 245
	{ "CreateObject",					funcCreateObject },					// 246
	{ "AttachObjectToVehicle",					funcAttachObjectToVehicle },					// 247
	{ "SetObjectPos",					funcSetObjectPos },					// 248
	{ "SetObjectRot",					funcSetObjectRot },					// 249
	{ "GetObjectPos",					funcGetObjectPos },					// 250
	{ "GetObjectRot",					funcGetObjectRot },					// 251
	{ "IsValidObject",					funcIsValidObject },					// 252
	{ "DestroyObject",					funcDestroyObject },					// 253
	{ "MoveObject",					funcMoveObject },					// 254
	{ "StopObject",					funcStopObject },					// 255
	{ "CreatePlayerObject",					funcCreatePlayerObject },					// 256
	{ "SetPlayerObjectPos",					funcSetPlayerObjectPos },					// 257
	{ "SetPlayerObjectRot",					funcSetPlayerObjectRot },					// 258
	{ "GetPlayerObjectPos",					funcGetPlayerObjectPos },					// 259
	{ "GetPlayerObjectRot",					funcGetPlayerObjectRot },					// 260
	{ "IsValidPlayerObject",					funcIsValidPlayerObject },					// 261
	{ "DestroyPlayerObject",					funcDestroyPlayerObject },					// 262
	{ "MovePlayerObject",					funcMovePlayerObject },					// 263
	{ "StopPlayerObject",					funcStopPlayerObject },					// 264
	{ "AttachObjectToPlayer",					funcAttachObjectToPlayer },					// 265
//	{ "AttachPlayerObjectToPlayer",					NULL },					// 266 LOLOLOL
	{ "CreateMenu",					funcCreateMenu },					// 267
	{ "DestroyMenu",					funcDestroyMenu },					// 268
	{ "AddMenuItem",					funcAddMenuItem },					// 269
	{ "SetMenuColumnHeader",					funcSetMenuColumnHeader },					// 270
	{ "ShowMenuForPlayer",					funcShowMenuForPlayer },					// 271
	{ "HideMenuForPlayer",					funcHideMenuForPlayer },					// 272
	{ "IsValidMenu",					funcIsValidMenu },					// 273
	{ "DisableMenu",					funcDisableMenu },					// 274
	{ "DisableMenuRow",					funcDisableMenuRow },					// 275
	{ "GetPlayerMenu",					funcGetPlayerMenu },					// 276
	{ "SetPlayerTime",					funcSetPlayerTime },					// 277
	{ "TogglePlayerClock",					funcTogglePlayerClock },					// 278
	{ "GetPlayerTime",					funcGetPlayerTime },					// 279
	{ "HTTP",					funcHTTP },
	{ "GetPlayerDistanceFromPoint", funcGetPlayerDistanceFromPoint },
	{ "GetVehicleDistanceFromPoint", funcGetVehicleDistanceFromPoint },
	{ "GetPlayerSurfingObjectID", funcGetPlayerSurfingObjectID },
	{ "GetPlayerCameraMode", funcGetPlayerCameraMode },
	{ "GetNetworkStats",			funcGetNetworkStats },
	{ "GetPlayerNetworkStats",		funcGetPlayerNetworkStats },
// Open SA-MP

	{ "SetPlayerGravity",			funcSetPlayerGravity },
	{ "GetPlayerGravity",			funcGetPlayerGravity },
	{ "GetPlayerSkillLevel",		funcGetPlayerSkillLevel },
	
	{ "ElementCreate",						funcElementCreate },
	{ "ElementClone",						funcElementClone },
	{ "ElementDestroy",						funcElementDestroy },

	{ "ElementSetModel",					funcElementSetModel },
	{ "ElementGetModel",					funcElementGetModel },
	
	{ "ElementIsValid",						funcElementIsValid },
	{ "ElementGetType",						funcElementGetType },
	
	{ "ElementSetParent",					funcElementSetParent },
	{ "ElementGetParent",					funcElementGetParent },
	{ "ElementSetOwner",					funcElementSetOwner },
	{ "ElementGetOwner",					funcElementGetOwner },

	{ "ElementSetPosition",					funcElementSetPosition },
	{ "ElementGetPosition",					funcElementGetPosition },
	{ "ElementSetAbsolutePosition",			funcElementSetAbsolutePosition },
	{ "ElementGetAbsolutePosition",			funcElementGetAbsolutePosition },

	{ "ElementGetDistanceFromPoint",		funcElementGetDistanceFromPoint },
	{ "ElementGetDistanceFromElement",		funcElementGetDistanceFromElement },
	
	{ "ElementMove",						funcElementMove },
	
	{ "ObjectCreate",						funcObjectCreate },
	{ "ObjectDestroy",						funcObjectDestroy },
	{ "ObjectIsValid",						funcObjectIsValid },
	
	{ "RadarZoneCreate",					funcRadarZoneCreate },
	{ "RadarZoneDestroy",					funcRadarZoneDestroy },
	{ "RadarZoneIsValid",					funcRadarZoneIsValid },
	{ "RadarZoneSetSize",					funcRadarZoneSetSize },
	{ "RadarZoneGetSize",					funcRadarZoneGetSize },
	{ "RadarZoneGetCenter",					funcRadarZoneGetCenter },
	{ "RadarZoneSetColor",					funcRadarZoneSetColor },
	{ "RadarZoneGetColor",					funcRadarZoneGetColor },
	{ "RadarZoneSetFlashingColor",			funcRadarZoneSetFlashingColor },
	{ "RadarZoneGetFlashingColor",			funcRadarZoneGetFlashingColor },
	{ "RadarZoneSetFlashing",				funcRadarZoneSetFlashing },
	{ "RadarZoneIsFlashing",				funcRadarZoneIsFlashing },
	
	{ "RadarIconCreate",					funcRadarIconCreate },
	{ "RadarIconDestroy",					funcRadarZoneDestroy },
	{ "RadarIconIsValid",					funcRadarZoneIsValid },
	{ "RadarIconSetColor",					funcRadarIconSetColor },
	{ "RadarIconGetColor",					funcRadarIconGetColor },

	{ "PickupCreate",						funcPickupCreate },

	{ "AddRule",							funcAddRule },
	{ "EditRule",							funcEditRule },
	{ "RemoveRule",							funcRemoveRule},

	//{ "TogglePlayerWideScreen",				funcTogglePlayerWideScreen },

	{ "IsAnOpenPlayer",						funcIsAnOpenPlayer }, 
	{ "CreatePlayerExplosion",				funcCreatePlayerExplosion },
	{ "PlayAudioStreamForPlayer",			funcPlayAudioStreamForPlayer },
	{ "StopAudioStreamForPlayer",			funcStopAudioStreamForPlayer },
	{ "PlayAudioStreamForAll",				funcPlayAudioStreamForAll },
	{ "StopAudioStreamForAll",				funcStopAudioStreamForAll },
	{ "RemoveBuildingForPlayer",			funcRemoveBuildingForPlayer },
	{ "GetVehicleColor",					funcGetVehicleColor },
	{ "TogglePlayerCollision",				funcTogglePlayerCollision },
	{ "ToggleVehicleCollision",				funcToggleVehicleCollision },
	{ "ToggleVehicleCollisionForPlayer",	funcToggleVehicleCollisionForPlayer },
	{ "GetPlayerTargetPlayer",				funcGetPlayerTargetPlayer },
	{ "BindKey",							funcBindKey },
	{ "UnbindKey",							funcUnbindKey },
	{ "IsValidBindKey",						funcIsValidBindKey },

	{ NULL, NULL }
};

int amx_CustomInit ( AMX* a_AmxInterface )
{
	if(bScriptDebug) logprintf ( "[Call]-> amx_CustomInit()" );
	return amx_Register ( a_AmxInterface, __AmxScriptNative, -1 );
}
