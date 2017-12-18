/**
 * Copyright (c) 2017 by Stefan Schmidt
 */

package de.meek.myhome;

import android.app.Activity;
import android.content.SharedPreferences;

public class RoomSettings {
    Activity activity;

    private static final String ROOM_NAME = "room_name";
    private static final String ROOM_BT_ADDR = "room_bt_addr";
    private static final String ROOM_PRESET_TEMP = "room_ptemp";


    public RoomSettings(Activity _activity) {
        activity = _activity;
    }

    public boolean loadRoom(Room room) {
            SharedPreferences settings = activity.getSharedPreferences(room.getId(), 0);
            room.name = settings.getString(ROOM_NAME, "Name");
        try {
            room.btAddress.convertFromLong(settings.getLong(ROOM_BT_ADDR, 0x0l));
        } catch (Exception e) {
            room.btAddress.convertFromLong(0);
        }
        room.presetTemp.clear();
        for (int i=0; i<Const.MAX_ROOM_PRESET_TEMP; i++) {
            int temp = settings.getInt(ROOM_PRESET_TEMP+i, Const.TEMP_DEFAULT);
            room.presetTemp.add(temp);
        }

        return true;
    }

    public boolean saveRoom(Room room) {
        SharedPreferences settings = activity.getSharedPreferences(room.getId(), 0);
        SharedPreferences.Editor editor = settings.edit();
        editor.putString(ROOM_NAME, room.name);
        editor.putLong(ROOM_BT_ADDR, room.btAddress.toLong());
        int i=0;
        for (Integer temp: room.presetTemp
             ) {
            editor.putInt(ROOM_PRESET_TEMP+i, temp);
            i++;
        }
        editor.commit();
        return true;
    }
}
