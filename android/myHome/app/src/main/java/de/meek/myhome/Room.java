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
    public int msgCount = 0;
    public boolean valid = false;
    public IRoom viewRoom;
    public Room(Activity activity, int _roomId, IRoom _viewRoom) {
        id = _roomId;
        SharedPreferences settings = activity.getSharedPreferences(getId(), 0);
        name = settings.getString(Const.INTENT_X_ROOM_NAME, "Name");
        btAddress = settings.getString(Const.INTENT_X_BT_ADDRESS, "112233445566");
        viewRoom = _viewRoom;
    }
    public String getId() {
        return "" + id;
    }
    public void update() {
        if(viewRoom != null) {
            viewRoom.update(this);
        }
    }
}
