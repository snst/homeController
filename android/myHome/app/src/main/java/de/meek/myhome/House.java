/**
 * Copyright (c) 2017 by Stefan Schmidt
 */

package de.meek.myhome;

import java.lang.reflect.Array;
import java.util.ArrayList;

public class House {
    protected ArrayList<Room> rooms = new ArrayList<Room>();

    public House() {

    }

    public Room getRoom(int i) {
        return rooms.get(i);
    }

    public int getSize() {
        return rooms.size();
    }

    public void addRoom(Room room) {
        rooms.add(room);
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
}
