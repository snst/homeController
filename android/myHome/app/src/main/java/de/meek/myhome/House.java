/**
 * Copyright (c) 2017 by Stefan Schmidt
 */

package de.meek.myhome;

import java.lang.reflect.Array;
import java.util.ArrayList;

public class House {
    protected ArrayList<Room> rooms = new ArrayList<Room>();

    protected ArrayList<String> mqttTopics = new ArrayList<String>();

    public House() {}

    public Room getRoom(int i) {
        return rooms.get(i);
    }

    public int getSize() {
        return rooms.size();
    }

    public void addRoom(Room room) {
        rooms.add(room);
        if(!mqttTopics.contains(room.mqttTopic)) {
            mqttTopics.add(room.mqttTopic);
        }
    }

    public Room findRoom(BTAddr addr) {
        for (Room r: rooms) {
            if(r.btAddress.isSameAddr(addr)) {
                return r;
            }
        }
        return null;
    }

    public ArrayList<Room> getRoomList() {
        return rooms;
    }

    public ArrayList<String> getMqttTopicList() { return mqttTopics; }

    public void clear() {
        rooms.clear();
    }
}
