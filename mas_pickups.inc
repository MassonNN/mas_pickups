



//			Masson`s pickup System
//			This include will make it easier to work with pickups, 
//			add functionality, and fix some known issues

//			Version: 0.5
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
	#error Connect the streamer before this include
#endif


#if !defined MAX_PICKUPS
	#error Please define MAX_PICKUPS
#endif


#define INVALID_PICKUP	MAX_PICKUPS+1

// Key for triggering pickups of the PICKUP_KEY type (optionally)
#if !defined PICKUP_KEY
	#define PICKUP_KEY 1024	// L.Alt
#endif


#if !defined PICKUP_SYNC_STANDART_ANTIFLOOD
	#define PICKUP_SYNC_STANDART_ANTIFLOOD 3
#endif


// Maximum number of characters in the pickup function name
#if !defined MAX_PICK_FUNC_NAME
	#define MAX_PICK_FUNC_NAME 23	// 32 - 9 (_mas_pck_)
#endif


#define Pickup:%0(%1) \
	forward _mas_pck_%0(%1);\
	public _mas_pck_%0(%1)


#define PickupFunction%0(%1) \
	forward%0(%1);\
	public%0(%1)


#define PICKUP_STANDART			0	// Standart pickup
#define PICKUP_ON_KEY			1	// Pickup on key
#define PICKUP_ONLY_CAR			2	// Only car pickup


#define PICKUP_MODE_DEFAULT 	0	// 
#define PICKUP_MODE_NONE 		2	// 
#define PICKUP_MODE_TEMP 		3	// Temporary pickup
#define PICKUP_MODE_SYNC 		4	// Synchronized pickup


forward OnPlayerUseSyncPickup(playerid, pickupid, syncpickupid);


enum PICKUP {
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
	pickFunction[32],
	pickMode,
	pickClass,
	pickSynced,
	pickAntiFlood
};

static getPickup[MAX_PICKUPS][PICKUP];


// Antiflood system
static pickupAntiFlood[MAX_PLAYERS];

// 
static pickupNow[MAX_PLAYERS];

// 
static pickupTotalPickup = 0;



//		PickupCreate(...)
//		Arguments: 
//		- pick_Function - function to call when pickup is pickedup
//		- the other arguments are from standard pickups
//		
//		returns: id of created pickup
//
PickupCreate(pick_Function[23], 
					pick_Model, 
					pick_Type, 
					Float:pick_X, 
					Float:pick_Y, 
					Float:pick_Z, 
					pick_VirtualWorld = 0, 
					pickup_Interior = 0, 
					pickup_Playerid = 0, 
					Float:streamdistance = STREAMER_PICKUP_SD) {
	if(pickupTotalPickup == MAX_PICKUPS) return 1;

	if(strlen(pick_Function) > 23) return 1;
	
	new mas_tmp_id = CreateDynamicPickup(pick_Model, pick_Type, pick_X, pick_Y, pick_Z, pick_VirtualWorld, pickup_Interior, 
	pickup_Playerid, streamdistance); 

	getPickup[mas_tmp_id][pickX] = pick_X;
	getPickup[mas_tmp_id][pickY] = pick_Y;
	getPickup[mas_tmp_id][pickZ] = pick_Z;

	getPickup[mas_tmp_id][pickModel] = pick_Model;
	getPickup[mas_tmp_id][pickType] = pick_Type;
	getPickup[mas_tmp_id][pickVirtualWorld] = pick_VirtualWorld;
	getPickup[mas_tmp_id][pickInterior] = pickup_Interior;
	getPickup[mas_tmp_id][pickPlayerid] = pickup_Playerid;
	getPickup[mas_tmp_id][pickClass] = PICKUP_STANDART;
	getPickup[mas_tmp_id][pickMode] = PICKUP_MODE_NONE;
	getPickup[mas_tmp_id][pickStreamDistance] = streamdistance;

	new mas_temp_func[32];
	strcat(mas_temp_func, "_mas_pck_");
	strcat(mas_temp_func, pick_Function);
	getPickup[mas_tmp_id][pickFunction] = mas_temp_func;

	pickupTotalPickup++;
	return mas_tmp_id;	
}


SyncPickup(syncid, withid) {
	getPickup[syncid][pickSynced] = withid;
	getPickup[withid][pickSynced] = syncid;
	return 1;
}


SyncLastPickups() {
	getPickup[pickupTotalPickup][pickSynced] = pickupTotalPickup--;
	getPickup[pickupTotalPickup--][pickSynced] = pickupTotalPickup;
	return 1;
}


PickupSetAntiFlood(id, interval) {
	getPickup[id][pickAntiFlood] = interval;
	return 1;
}


PickupSetAntiFloodLast(interval) {
	getPickup[pickupTotalPickup--][pickAntiFlood] = interval;
	return 1;
}


public OnPlayerPickUpDynamicPickup(playerid, pickupid) {

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



PickupFunction pickupAntiFloodUpdate() {
	for(new i; i < MAX_PLAYERS; i++) {
		if(pickupAntiFlood[i] != 0) pickupAntiFlood[i]--;
	}
	return 1;
}


GetPickupModelEx(id) return getPickup[id][pickModel];

GetPickupTypeEx(id) return getPickup[id][pickType];

GetPickupVirtualWorldEx(id) return getPickup[id][pickVirtualWorld];

GetPickupPlayerid(id) return getPickup[id][pickupPlayerid];

PickupEmitate(playerid, id) {
	if(!getPickup[id][pickFunction]) return 0;
	return CallLocalFunction(getPickup[id][pickFunction], "dd", playerid, id);
}

// GetPlayerPickup(playerid) return pickupNow[playerid];
