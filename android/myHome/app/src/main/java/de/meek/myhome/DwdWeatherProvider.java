/**
 * Copyright (c) 2017 by Stefan Schmidt
 */

package de.meek.myhome;

public class DwdWeatherProvider {

    public String server = "ftp-cdc.dwd.de";
    public String file = "/pub/CDC/observations_germany/climate/10_minutes/air_temperature/now/10minutenwerte_TU_03668_now.zip";

    public DwdWeatherProvider() {
    }
    FtpTask ftp = null;

    public void update() {
        ftp = new FtpTask(server, file) {
            @Override
            protected void onPostExecute(String str) {
                super.onPostExecute(str);
                parseString(str);
            }
        };
        ftp.execute();
    }

    void parseString(String str) {
//        Log.d("data", str);
        if (str != null) {
            String[] lines = str.split("\n");
            if (lines != null && lines.length > 0) {
                parseEntry(lines[lines.length - 1]);
            }
        }
    }

    boolean parseEntry(String str) {
//            STATIONS_ID;MESS_DATUM;  QN   ;PP_10  ;TT_10 ;TM5_10;RF_10 ;TD_10 ;eor
//            3668;201802240000;    2;  981.7;  -4.2;  -6.4;  65.5;  -9.7;eor
        try {
            String[] parts = str.split(";");
            int i = 0;
            String stationId = parts[i++];
            String date = parts[i++];
            i++;
            String pressure = parts[i++];
            String temperature2m = parts[i++];
            String temperature5cm = parts[i++];
            String humidity = parts[i++];
            String dewPoint = parts[i++];
            SensorData data = new SensorData(SensorData.SOURCE_DWD);
            data.setTemperature(Float.parseFloat(temperature2m));
            data.setHumidity(Float.parseFloat(humidity));
            data.setDewPoint(Float.parseFloat(dewPoint));
            data.setPressure(Float.parseFloat(pressure));
            onNewData(data);
        } catch (Exception e) {
            return false;
        }
        return true;
    }

    public void onNewData(SensorData data) {
    }
}
