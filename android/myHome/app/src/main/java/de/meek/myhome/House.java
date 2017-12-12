/**
 * Copyright (c) 2017 by Stefan Schmidt
 */

package de.meek.myhome;

import java.util.ArrayList;

public class House {
    private static House mInstance = null;

    protected ArrayList<Room> rooms = new ArrayList<Room>();
    public static Room getRoom(int i) {
        return getInstance().rooms.get(i);
    }

    public static int getSize() {
        return getInstance().rooms.size();
    }

    public static void addRoom(Room room) {
        getInstance().rooms.add(room);
    }

    protected House(){}

    public static synchronized House getInstance(){
        if(null == mInstance){
            mInstance = new House();
        }
        return mInstance;
    }
}
