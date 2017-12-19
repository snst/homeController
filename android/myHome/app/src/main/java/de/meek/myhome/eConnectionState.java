/**
 * Copyright (c) 2017 by Stefan Schmidt
 */

package de.meek.myhome;


public enum eConnectionState {
    UNKOWN(0, "?"),
    CONNECTING(1, "connecting"),
    CONNECTED(2, "connected"),
    DISCONNECTED(3, "disconnected"),
    NORESPONSE(4, "noresponse");

    private byte value;
    private String name;

    private eConnectionState(int value, String name) {
        this.value = (byte) value;
        this.name = name;
    }

    public byte getValue() {
        return value;
    }

    public String toString() {
        return name;
    }
}
