package de.meek.myhome;

import android.content.Context;
import android.util.Log;
import android.widget.Toast;

import org.eclipse.paho.android.service.MqttAndroidClient;
import org.eclipse.paho.client.mqttv3.DisconnectedBufferOptions;
import org.eclipse.paho.client.mqttv3.IMqttActionListener;
import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.IMqttToken;
import org.eclipse.paho.client.mqttv3.MqttCallbackExtended;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;

/**
 * Created by stefan on 02.12.2017.
 */

public class MqttHelper {
    public MqttAndroidClient mqttAndroidClient;

    final String serverUri = AccountConfig.MQTT_SERVER_URI;

    final String clientId = AccountConfig.MQTT_CLIENT_ID;
    final String subscriptionTopic = AccountConfig.MQTT_TOPIC_SUBSCRIPTION;

    final String username = AccountConfig.MQTT_USERNAME;
    final String password = AccountConfig.MQTT_PASSWORD;
    Context _context;

    protected Logger getLogger() {
        MyApplication app = (MyApplication)_context;
        return app.getLogger();
    }

    public MqttHelper(Context context){
        _context = context;
        mqttAndroidClient = new MqttAndroidClient(context, serverUri, clientId);
        mqttAndroidClient.setCallback(new MqttCallbackExtended() {
            @Override
            public void connectComplete(boolean b, String s) {
                Log.w("mqtt", s);
            }

            @Override
            public void connectionLost(Throwable throwable) {
            }

            @Override
            public void messageArrived(String topic, MqttMessage mqttMessage) throws Exception {
                Log.w("Mqtt", mqttMessage.toString());
            }

            @Override
            public void deliveryComplete(IMqttDeliveryToken iMqttDeliveryToken) {

            }
        });
        connect();
    }

    public void sendCmd(Cmd cmd) {

        try {
            if(cmd != null && isConnected()) {
                getLogger().add("<" + AccountConfig.MQTT_TOPIC_REQUEST + ": " + cmd.cmd.toString(), cmd.addr);
               mqttAndroidClient.publish(AccountConfig.MQTT_TOPIC_REQUEST, cmd.getBuffer(), 0, false);
            }
        } catch (MqttException e) {
            e.printStackTrace();
        }
    }

    public void setCallback(MqttCallbackExtended callback) {
        mqttAndroidClient.setCallback(callback);
    }

    public boolean isConnected() {
        return mqttAndroidClient!=null ? mqttAndroidClient.isConnected() : false;
    }

    public void disconnect() {
        try {
            getLogger().add("Mqtt disconnecting");
            mqttAndroidClient.disconnect();
        } catch (MqttException e) {
            e.printStackTrace();
        }
    }

    private void connect(){
        getLogger().add("Mqtt connecting");
        MqttConnectOptions mqttConnectOptions = new MqttConnectOptions();
        mqttConnectOptions.setAutomaticReconnect(true);
        mqttConnectOptions.setCleanSession(false);
        mqttConnectOptions.setUserName(username);
        mqttConnectOptions.setPassword(password.toCharArray());

        try {

            mqttAndroidClient.connect(mqttConnectOptions, null, new IMqttActionListener() {
                @Override
                public void onSuccess(IMqttToken asyncActionToken) {

                    DisconnectedBufferOptions disconnectedBufferOptions = new DisconnectedBufferOptions();
                    disconnectedBufferOptions.setBufferEnabled(true);
                    disconnectedBufferOptions.setBufferSize(100);
                    disconnectedBufferOptions.setPersistBuffer(false);
                    disconnectedBufferOptions.setDeleteOldestMessages(false);
                    mqttAndroidClient.setBufferOpts(disconnectedBufferOptions);
                    subscribeToTopic();
                }

                @Override
                public void onFailure(IMqttToken asyncActionToken, Throwable exception) {
                    String str = "Failed to connect to: " + serverUri + exception.toString();
                    getLogger().add(str);
                    Log.w("Mqtt",str);
                    Toast.makeText(_context, str,
                            Toast.LENGTH_LONG).show();
                }
            });


        } catch (MqttException ex){
            ex.printStackTrace();
        }
    }


    private void subscribeToTopic() {
        try {
            mqttAndroidClient.subscribe(subscriptionTopic, 0, null, new IMqttActionListener() {
                @Override
                public void onSuccess(IMqttToken asyncActionToken) {
                    String str = "Mqtt subscribe ok: " + subscriptionTopic;
                    getLogger().add(str);
                    Log.w("Mqtt",str);
                }

                @Override
                public void onFailure(IMqttToken asyncActionToken, Throwable exception) {
                    String str = "Mqtt subscribe failed: " + subscriptionTopic;
                    getLogger().add(str);
                    Log.w("Mqtt",str);
                }
            });

        } catch (MqttException ex) {
            System.err.println("Exception whilst subscribing");
            ex.printStackTrace();
        }
    }
}
