/**
 * Copyright (c) 2017 by Stefan Schmidt
 */

package de.meek.myhome;

import android.app.Activity;
import android.content.SharedPreferences;

public class Room {
    public String name;
    public String btAddress;
    public int id;
    public boolean autoActive = false;
    public boolean boostActive = false;
    public int percent = 0;
    public int temp = 0;
    public int buttonId;
    public boolean valid = false;
    public Room(Activity activity, int _roomId, int _buttonId) {
        id = _roomId;
        buttonId = _buttonId;
        SharedPreferences settings = activity.getSharedPreferences(getId(), 0);
        name = settings.getString(Const.INTENT_X_ROOM_NAME, "Name");
        btAddress = settings.getString(Const.INTENT_X_BT_ADDRESS, "112233445566");
    }
    public String getId() {
        return "" + id;
    }
}
