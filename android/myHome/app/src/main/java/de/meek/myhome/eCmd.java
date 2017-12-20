/**
 * Copyright (c) 2017 by Stefan Schmidt
 */

package de.meek.myhome;


public enum eCmd {
    NONE(0, "none"),
    PING(1, "ping"),
    BOOST_ON(2, "boost on"),
    BOOST_OFF(3, "boost off"),
    ON(4, "on"),
    OFF(5, "off"),
    ECO(6, "eco"),
    COMFORT(7, "comfort"),
    AUTO(8, "auto"),
    MANUAL(9, "manual"),
    REBOOT(10, "reboot"),
    SETTEMP(11, "settemp"),
    GETSTATUS(12, "getstatus"),
    ABORT(13, "abort");

    private byte value;
    private String name;

    private eCmd(int value, String name) {
        this.value = (byte)value;
        this.name = name;
    }
    public byte getValue() {
        return value;
    }
    public String toString() { return name; }
}

