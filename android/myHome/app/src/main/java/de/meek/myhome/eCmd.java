/**
 * Copyright (c) 2017 by Stefan Schmidt
 */

package de.meek.myhome;


public enum eCmd {
    NONE(0),
    PING(1),
    BOOST_ON(2),
    BOOST_OFF(3),
    ON(4),
    OFF(5),
    ECO(6),
    COMFORT(7),
    AUTO(8),
    MANUAL(9),
    REBOOT(10),
    SETTEMP(11),
    GETSTATUS(12);

    private byte value;

    private eCmd(int value) {
        this.value = (byte)value;
    }
    public byte getValue() {
        return value;
    }
}

