/**
 * Copyright (c) 2017 by Stefan Schmidt
 */
package de.meek.myhome;


public class SensorData {
    public SensorData(int source) {
        this.source = source;
    }

    public static final int SOURCE_INSIDE = 1;
    public static final int SOURCE_OUTSIDE = 2;
    public static final int SOURCE_DWD = 3;

    public int source = 0;
    public String stationId = "";
    public String date = "";
    public float pressure = 0.0f;
    public boolean pressureValid = false;
    public float temperature = 0.0f;
    public boolean temperatureValid = false;
    public float humidity = 0.0f;
    public boolean humidityValid = false;
    public float dewPoint = 0.0f;
    public boolean dewPointValid = false;

    public void setTemperature(float val) {
        temperature = val;
        temperatureValid = true;
    }

    public void setHumidity(float val) {
        humidity = val;
        humidityValid = true;
    }

    public void setPressure(float val) {
        pressure = val;
        pressureValid = true;
    }

    public void setDewPoint(float val) {
        dewPoint = val;
        dewPointValid = true;
    }
}
