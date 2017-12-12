/**
 * Copyright (c) 2017 by Stefan Schmidt
 */

package de.meek.myhome;

import android.app.Activity;
import android.content.SharedPreferences;

public class AccountConfig {
    public static String MQTT_SERVER_URI = "";

    public static String MQTT_CLIENT_ID = "";
    public static String MQTT_SUBSCRIPTION_TOPIC = "eq3/+";
    public static String MQTT_USERNAME = "";
    public static String MQTT_PASSWORD = "";

    private static final String SETTINGS = "settings";
    private static final String URI = "uri";
    private static final String CLIENTID = "clientid";
    private static final String USERNAME = "user";
    private static final String PASSWORD = "pw";

    public static void load(Activity activity) {
        SharedPreferences settings = activity.getSharedPreferences(SETTINGS, 0);
        MQTT_SERVER_URI = settings.getString(URI, "tcp://m11.cloudmqtt.com:12345");
        MQTT_CLIENT_ID = settings.getString(CLIENTID, "myHomeAndroidClient");
        MQTT_USERNAME = settings.getString(USERNAME, "username");
        MQTT_PASSWORD = settings.getString(PASSWORD, "password");
    }

    public static void save(Activity activity) {

        SharedPreferences settings = activity.getSharedPreferences(SETTINGS, 0);
        SharedPreferences.Editor editor = settings.edit();
        editor.putString(URI, MQTT_SERVER_URI);
        editor.putString(CLIENTID, MQTT_CLIENT_ID);
        editor.putString(USERNAME, MQTT_USERNAME);
        editor.putString(PASSWORD, MQTT_PASSWORD);
        editor.commit();
    }
}
