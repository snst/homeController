/**
 * Copyright (c) 2017 by Stefan Schmidt
 */

package de.meek.myhome;


public enum eResponse {
    NONE(0),
    PONG(1),
    STATE(2),
    CONNECTION(3);

    private byte value;

    private eResponse(int value) {
        this.value = (byte)value;
    }
    public byte getValue() {
        return value;
    }
}
