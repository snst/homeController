/**
 * Copyright (c) 2017 by Stefan Schmidt
 */

package de.meek.myhome;


public enum eConnectionState {
    UNKOWN(0),
    CONNECTING(1),
    CONNECTED(2),
    DISCONNECTED(3),
    NORESPONSE(4);

    private byte value;

    private eConnectionState(int value) {
        this.value = (byte)value;
    }
    public byte getValue() {
        return value;
    }
    }
