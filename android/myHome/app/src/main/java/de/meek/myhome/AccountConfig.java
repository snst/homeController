/**
 * Copyright (c) 2017 by Stefan Schmidt
 */

package de.meek.myhome;

import android.app.Activity;
import android.content.SharedPreferences;

public class AccountConfig {
    public static String MQTT_SERVER_URI = "";

    public static String MQTT_CLIENT_ID = "";
    public static String MQTT_TOPIC_SUBSCRIPTION = "";
    public static String MQTT_TOPIC_REQUEST = "";
    public static String MQTT_TOPIC_STATUS = "";
    public static String MQTT_TOPIC = "";
    public static String MQTT_USERNAME = "";
    public static String MQTT_PASSWORD = "";
    public static int NUMBER_OF_ROOMS = 4;

    private static final String ID_SETTINGS = "roomSettings";
    private static final String ID_URI = "uri";
    private static final String ID_CLIENTID = "clientid";
    private static final String ID_USERNAME = "user";
    private static final String ID_PASSWORD = "pw";
    private static final String ID_NUMBEROFROOMS = "rooms";
    private static final String ID_TOPIC = "topic";

    public static void load(Activity activity) {
        SharedPreferences settings = activity.getSharedPreferences(ID_SETTINGS, 0);
        MQTT_SERVER_URI = settings.getString(ID_URI, "tcp://m11.cloudmqtt.com:12345");
        MQTT_CLIENT_ID = settings.getString(ID_CLIENTID, "myHomeAndroidClient");
        MQTT_USERNAME = settings.getString(ID_USERNAME, "username");
        MQTT_PASSWORD = settings.getString(ID_PASSWORD, "password");
        MQTT_TOPIC = settings.getString(ID_TOPIC, "eq3");
        MQTT_TOPIC_REQUEST = MQTT_TOPIC + "/req";
        MQTT_TOPIC_SUBSCRIPTION = MQTT_TOPIC + "/+";
        MQTT_TOPIC_STATUS = MQTT_TOPIC + "/stat";
        NUMBER_OF_ROOMS = settings.getInt(ID_NUMBEROFROOMS, 4);
    }

    public static void save(Activity activity) {

        SharedPreferences settings = activity.getSharedPreferences(ID_SETTINGS, 0);
        SharedPreferences.Editor editor = settings.edit();
        editor.putString(ID_URI, MQTT_SERVER_URI);
        editor.putString(ID_CLIENTID, MQTT_CLIENT_ID);
        editor.putString(ID_USERNAME, MQTT_USERNAME);
        editor.putString(ID_PASSWORD, MQTT_PASSWORD);
        editor.putString(ID_TOPIC, MQTT_TOPIC);
        editor.putInt(ID_NUMBEROFROOMS, NUMBER_OF_ROOMS);
        editor.commit();
    }
}
