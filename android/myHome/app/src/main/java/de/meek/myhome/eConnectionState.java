/**
 * Copyright (c) 2017 by Stefan Schmidt
 */

package de.meek.myhome;


public enum eConnectionState {
    QUEUED(0, "QUEUED"),
    DISCONNECTED(1, "disconnected"),
    CONNECTING(2, "connecting"),
    DISCONNECTING(3, "disconnecting"),
    FAILED(4, "failed"),
    CONNECTED(5, "connected");

    private byte value;
    private String name;

    eConnectionState(int value, String name) {
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
