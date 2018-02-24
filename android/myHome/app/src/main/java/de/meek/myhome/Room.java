/**
 * Copyright (c) 2017 by Stefan Schmidt
 */

package de.meek.myhome;

import java.util.ArrayList;
import java.util.Date;

public class Room {
    public String name;
    public BTAddr btAddress = new BTAddr();
    public String mqttTopic;
    public int id;
    public boolean autoActive = false;
    public boolean boostActive = false;
    public boolean lowBattery = false;
    public boolean showControls = false;
    public int lastRequestedTemp = Const.TEMP_DEFAULT;
    public boolean updateOnStart = true;
    public int percent = 0;
    public int temp = 0;
    public int msgCount = 0;
    public boolean valid = false;
    public boolean refreshing = false;
    //public boolean isSensorTemp = false;
    public eConnectionState connectionState = eConnectionState.DISCONNECTED;
    public Date lastUpdate;
    public ArrayList<Integer> presetTemp = new ArrayList<Integer>();
    public Room(int _roomId) {
        id = _roomId;
    }
    public String getId() {
        return "" + id;
    }
    public boolean isConnected() { return connectionState == eConnectionState.CONNECTED; }
}
