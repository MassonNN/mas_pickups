



//			Masson`s pickup System
//			This include will make it easier to work with pickups, 
//			add functionality, and fix some known issues

//			Version: 0.6
//			Last Release: 02.06.2020

//			Dependencies:
//			- Streamer

//			Compatibility:
//			- YSF


#if defined _MASSON_PICKUP
	#endinput
#else 
	#define _MASSON_PICKUP
#endif


#if !defined _streamer_included
	#error Include streamer before this include
#endif


#if !defined MAX_PICKUPS
	#error Please define MAX_PICKUPS
#endif


#define INVALID_PICKUP	MAX_PICKUPS+1


#if !defined PICKUP_SYNC_STANDART_ANTIFLOOD
	#define PICKUP_SYNC_STANDART_ANTIFLOOD 3
#endif


// Maximum number of characters in the pickup function name
#if !defined MAX_PICK_FUNC_NAME
	#define MAX_PICK_FUNC_NAME 23	// 32 - 9 (_mas_pck_)
#endif

#if !defined MAS_PCK_MAX_FUNC
	#define MAS_PCK_MAX_FUNC 32	// for pawn functions
#endif


#define Pickup:%0(%1) \
	forward _mas_pck_%0(%1);\
	public _mas_pck_%0(%1)


#define PickupFunction%0(%1) \
	forward%0(%1);\
	public%0(%1)


#define PICKUP_STANDART			0	// Standart pickup
#define PICKUP_ON_KEY			1	// Pickup on key
// #define PICKUP_ONLY_CAR			2	// Only car pickup


#define PICKUP_MODE_DEFAULT 	0	// 
#define PICKUP_MODE_NONE 		1	// 
// #define PICKUP_MODE_TEMP 		2	// Temporary pickup
// #define PICKUP_MODE_SYNC 		3	// Synchronized pickup


#if !defined MAS_PCK_TEXT_DRAWDIST
	#define MAS_PCK_TEXT_DRAWDIST 20.0
#endif


#if !defined MAS_PCK_KEY
	#define MAS_PCK_KEY 1024 // L.ALT
#endif

#if !defined MAS_PCK_AREA_SIZE
	#define MAS_PCK_AREA_SIZE 5.0
#endif


forward OnPlayerUseSyncPickup(playerid, pickupid, syncpickupid);


// Optionall
static const _mas_KEY_PRESS_TEXT[] = "Press alt";


enum MAS_PICKUP {
		pickID,
  Float:pickX,
  Float:pickY,
  Float:pickZ,
		pickModel,
		pickType,
		pickInterior,
		pickPlayerid,
		pickVirtualWorld,
  Float:pickStreamDistance,
		pickFunction[MAS_PCK_MAX_FUNC],
		pickMode,
		pickClass,
		pickSynced,
		pickAntiFlood,
 Text3D:pickText[sizeof _mas_KEY_PRESS_TEXT],
		pickArea
};


static getPickup[MAX_PICKUPS][MAS_PICKUP];


// Antiflood system
static pickupAntiFlood[MAX_PLAYERS];

// 
static pickupNow[MAX_PLAYERS];

// 
static pickupTotalPickup = 0;


PickupCreate(pick_Function[MAX_PICK_FUNC_NAME], 
					pick_Model, 
					pick_Type, 
					Float:pick_X, 
					Float:pick_Y, 
					Float:pick_Z, 
					pick_VirtualWorld = 0, 
					pick_Interior = 0, 
					pick_Playerid = 0, 
					Float:streamdistance = STREAMER_PICKUP_SD) {
	if(pickupTotalPickup == MAX_PICKUPS) return 1;

	if(strlen(pick_Function) > MAX_PICK_FUNC_NAME) return 1;
	
	new mas_tmp_id = CreateDynamicPickup(pick_Model, pick_Type, pick_X, pick_Y, pick_Z, pick_VirtualWorld, pick_Interior, 
	pick_Playerid, streamdistance); 

	getPickup[mas_tmp_id][pickX] = pick_X;
	getPickup[mas_tmp_id][pickY] = pick_Y;
	getPickup[mas_tmp_id][pickZ] = pick_Z;

	getPickup[mas_tmp_id][pickModel] = pick_Model;
	getPickup[mas_tmp_id][pickType] = pick_Type;
	getPickup[mas_tmp_id][pickVirtualWorld] = pick_VirtualWorld;
	getPickup[mas_tmp_id][pickInterior] = pick_Interior;

	getPickup[mas_tmp_id][pickSynced] = INVALID_PICKUP;

	
	getPickup[mas_tmp_id][pickPlayerid] = pick_Playerid;
	getPickup[mas_tmp_id][pickClass] = PICKUP_STANDART;
	getPickup[mas_tmp_id][pickMode] = PICKUP_MODE_NONE;
	getPickup[mas_tmp_id][pickStreamDistance] = streamdistance;


	if(strlen(pick_Function)) {
		new mas_temp_func[MAS_PCK_MAX_FUNC];
		strcat(mas_temp_func, "_mas_pck_");
		strcat(mas_temp_func, pick_Function);
		getPickup[mas_tmp_id][pickFunction] = mas_temp_func;
	}

	pickupTotalPickup++;
	return mas_tmp_id;	
}


ReplacePickup(&id, Float:new_x, Float:new_y, Float:new_z, new_vw = -1, new_int = -1) {
	if(getPickup[id][!pickModel]) return 0;

	DestroyDynamicPickup(id);

	getPickup[id][pickX] = new_x;
	getPickup[id][pickY] = new_y;
	getPickup[id][pickZ] = new_z;

	if(new_int != -1) getPickup[id][pickInterior] = new_int;
	if(new_vw != -1) getPickup[id][pickVirtualWorld] = new_vw;

	id = CreateDynamicPickup(
		getPickup[id][pickModel], 
		getPickup[id][pickType], 
		getPickup[id][pickX], 
		getPickup[id][pickY], 
		getPickup[id][pickZ], 
		getPickup[id][pickVirtualWorld], 
		getPickup[id][pickInterior], 
		getPickup[id][pickPlayerid], 
		getPickup[id][pickStreamDistance] 
	);

	return 1;
}


SetPickupModel(&id, model) {
	if(!getPickup[id][pickModel]) return 0;
	
	DestroyDynamicPickup(id);
	
	getPickup[id][pickModel] = model;

	id = CreateDynamicPickup(
		getPickup[id][pickModel], 
		getPickup[id][pickType], 
		getPickup[id][pickX], 
		getPickup[id][pickY], 
		getPickup[id][pickZ], 
		getPickup[id][pickVirtualWorld], 
		getPickup[id][pickInterior], 
		getPickup[id][pickPlayerid], 
		getPickup[id][pickStreamDistance] 
	);

	return 1;
}


SetPickupVirtualWorld(&id, world) {
	if(getPickup[id][!pickModel]) return 0;
	
	DestroyDynamicPickup(id);
	
	getPickup[id][pickVirtualWorld] = world;

	id = CreateDynamicPickup(
		getPickup[id][pickModel], 
		getPickup[id][pickType], 
		getPickup[id][pickX], 
		getPickup[id][pickY], 
		getPickup[id][pickZ], 
		getPickup[id][pickVirtualWorld], 
		getPickup[id][pickInterior], 
		getPickup[id][pickPlayerid], 
		getPickup[id][pickStreamDistance] 
	);

	return 1;
}


SetPickupType(&id, type) {
	if(!getPickup[id][pickModel]) return 0;
	
	DestroyDynamicPickup(id);
	
	getPickup[id][pickType] = type;

	id = CreateDynamicPickup(
		getPickup[id][pickModel], 
		getPickup[id][pickType], 
		getPickup[id][pickX], 
		getPickup[id][pickY], 
		getPickup[id][pickZ], 
		getPickup[id][pickVirtualWorld], 
		getPickup[id][pickInterior], 
		getPickup[id][pickPlayerid], 
		getPickup[id][pickStreamDistance] 
	);

	return 1;
}


PickupSetOnKey(id) {
	#if defined _YSF_included
		if(!IsValidPickup(id)) return 0;
	#else 
		if(id >= MAX_PICKUPS) return 0;
	#endif

	getPickup[id][pickClass] = PICKUP_ON_KEY;
	getPickup[id][pickText] = Create3DTextLabel(
		_mas_KEY_PRESS_TEXT, 
		-1,
		getPickup[id][pickX], 
		getPickup[id][pickY], 
		getPickup[id][pickZ]+0.5, 
		MAS_PCK_TEXT_DRAWDIST, 
		getPickup[id][pickVirtualWorld]
	);

	getPickup[id][pickArea] = CreateDynamicSphere(
		getPickup[id][pickX], 
		getPickup[id][pickY], 
		getPickup[id][pickZ], 
		MAS_PCK_AREA_SIZE, 
		getPickup[id][pickVirtualWorld], 
		getPickup[id][pickInterior], 
		getPickup[id][pickPlayerid]
	);

	return 1;
}



SyncPickup(syncid, withid) {
	getPickup[syncid][pickSynced] = withid;
	getPickup[withid][pickSynced] = syncid;
	return 1;
}

/* // Coming soon
SyncLastPickups() {
	getPickup[pickupTotalPickup][pickSynced] = pickupTotalPickup--;
	getPickup[pickupTotalPickup--][pickSynced] = pickupTotalPickup;
	return 1;
}
*/

PickupSetAntiFlood(id, interval) {
	getPickup[id][pickAntiFlood] = interval;
	return 1;
}

/* // Coming soon
PickupSetAntiFloodLast(interval) {
	getPickup[pickupTotalPickup--][pickAntiFlood] = interval;
	return 1;
}
*/

public OnPlayerPickUpDynamicPickup(playerid, pickupid) {

	if(getPickup[pickupid][pickClass] != PICKUP_ON_KEY) {
		if(pickupAntiFlood[playerid] == 0) {
			if(getPickup[pickupid][pickFunction]) 
				CallLocalFunction(getPickup[pickupid][pickFunction], "dd", playerid, pickupid);
			if(getPickup[pickupid][pickSynced] != INVALID_PICKUP) {

				new syncedid = getPickup[pickupid][pickSynced];
				CallLocalFunction("OnPlayerUseSyncPickup", "ddd", playerid, pickupid, syncedid);

				if(
					!getPickup[syncedid][pickX] &&
					!getPickup[syncedid][pickY] &&
					!getPickup[syncedid][pickZ] 
				)	return 1;

				else {
					SetPlayerPos(playerid, getPickup[syncedid][pickX], getPickup[syncedid][pickY], getPickup[syncedid][pickZ]+0.1);
					SetPlayerInterior(playerid, getPickup[syncedid][pickInterior]);
					SetPlayerVirtualWorld(playerid, getPickup[syncedid][pickVirtualWorld]);
					pickupAntiFlood[playerid] += PICKUP_SYNC_STANDART_ANTIFLOOD;
				}
			}
			pickupAntiFlood[playerid] += getPickup[pickupid][pickAntiFlood];
		}
	}

	

    #if defined _mas_pck_OnPlayerPickUpDynamicP  
        _mas_pck_OnPlayerPickUpDynamicP(playerid, pickupid);
    #endif
    return 1;
}
#if    defined    _ALS_OnPlayerPickUpDynamicPicku    
    #undef    OnPlayerPickUpDynamicPickup
#else
    #define    _ALS_OnPlayerPickUpDynamicPicku
#endif
#define    OnPlayerPickUpDynamicPickup    _mas_pck_OnPlayerPickUpDynamicP
#if    defined    _mas_pck_OnPlayerPickUpDynamicP
forward _mas_pck_OnPlayerPickUpDynamicP(playerid, pickupid);
#endif 


public OnGameModeInit() {

	SetTimer("pickupAntiFloodUpdate", 1000, true);

    #if defined _mas_pck_OnGameModeInit  
        _mas_pck_OnGameModeInit();
    #endif
    return 1;
}
#if    defined    _ALS_OnGameModeInit   
    #undef    OnGameModeInit
#else
    #define    _ALS_OnGameModeInit
#endif
#define    OnGameModeInit    _mas_pck_OnGameModeInit
#if    defined    _mas_pck_OnGameModeInit
forward _mas_pck_OnGameModeInit();
#endif 




public OnPlayerKeyStateChange(playerid, newkeys, oldkeys) {

	if(newkeys == MAS_PCK_KEY && IsPlayerInAnyDynamicArea(playerid)) {
		for(new i; i < MAX_PICKUPS; i++) {
			if(IsPlayerInDynamicArea(playerid, getPickup[i][pickArea])) {
			 	PickupEmitate(playerid, i);
			} else continue;
		}
	}

    #if defined _mas_pck_OnPlayerKeyStateChange
        _mas_pck_OnPlayerKeyStateChange(playerid, newkeys, oldkeys);
    #endif
    return 1;
}
#if    defined    _ALS_OnPlayerKeyStateChange  
    #undef    OnPlayerKeyStateChange
#else
    #define    _ALS_OnPlayerKeyStateChange
#endif
#define    OnPlayerKeyStateChange    _mas_pck_OnPlayerKeyStateChange
#if    defined    _mas_pck_OnPlayerKeyStateChange
forward _mas_pck_OnPlayerKeyStateChange(playerid, newkeys, oldkeys);
#endif 


//
PickupFunction pickupAntiFloodUpdate() {
	for(new i; i < MAX_PLAYERS; i++) {
		if(pickupAntiFlood[i] != 0) pickupAntiFlood[i]--;
	}
	return 1;
}

//
PickupFunction GetPickupModelEx(id) return getPickup[id][pickModel];

//
PickupFunction GetPickupTypeEx(id) return getPickup[id][pickType];

//
PickupFunction GetPickupVirtualWorldEx(id) return getPickup[id][pickVirtualWorld];

//
PickupFunction GetPickupPlayerid(id) return getPickup[id][pickPlayerid];

//
PickupFunction PickupEmitate(playerid, id) {
	if(!getPickup[id][pickFunction]) return 0;
	return CallLocalFunction(getPickup[id][pickFunction], "dd", playerid, id);
}

// GetPlayerPickup(playerid) return pickupNow[playerid];
