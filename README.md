# mas_pickups
Include for work with pickups


define PICKUP_SYNC_STANDART_ANTIFLOOD - default antiflood for synced pickups



PickupCreate(pick_Function[23], pick_Model, pick_Type, Float:pick_X, Float:pick_Y, Float:pick_Z, pick_VirtualWorld = 0, pick_Interior = 0, pick_Playerid = 0, Float:streamdistance = STREAMER_PICKUP_SD)

SyncPickup(syncid, withid)

PickupSetAntiFlood(id, interval)

GetPickupModelEx(id)

GetPickupTypeEx(id)

GetPickupVirtualWorldEx(id)

GetPickupPlayerid(id)

ReplacePickup(&id, Float:new_x, Float:new_y, Float:new_z, new_vw = -1, new_int = -1)

SetPickupModel(&id, model)

SetPickupVirtualWorld(&id, world)

SetPickupType(&id, type)

PickupSetOnKey(id)

PickupEmitate(playerid, id)

forward OnPlayerUseSyncPickup(playerid, pickupid, syncpickupid);



