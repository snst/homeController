/**
 * Copyright (c) 2017 by Stefan Schmidt
 */

package de.meek.myhome;

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
}
