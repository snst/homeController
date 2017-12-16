/**
 * Copyright (c) 2017 by Stefan Schmidt
 */

package de.meek.myhome;

import android.app.Activity;
import android.content.SharedPreferences;

import java.time.LocalTime;
import java.util.ArrayList;
import java.util.Date;

public class Room {
    public String name;
    public BTAddr btAddress = new BTAddr();
    public int id;
    public boolean autoActive = false;
    public boolean boostActive = false;
    public int percent = 0;
    public int temp = 0;
    public int msgCount = 0;
    public boolean valid = false;
    public Date lastUpdate;
    public IRoom viewRoom;
    public ArrayList<Integer> presetTemp = new ArrayList<Integer>();
    public Room(int _roomId, IRoom _viewRoom) {
        id = _roomId;
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
