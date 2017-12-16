/**
 * Copyright (c) 2017 by Stefan Schmidt
 */

package de.meek.myhome;

import android.app.Application;

public class MyApplication extends Application {

    protected House house = new House();
    public House getHouse() { return house; }
}