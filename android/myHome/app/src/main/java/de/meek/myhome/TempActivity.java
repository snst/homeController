/**
 * Copyright (c) 2017 by Stefan Schmidt
 */

package de.meek.myhome;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.SeekBar;
import android.widget.TextView;

import java.text.DateFormat;
import java.text.SimpleDateFormat;

public class TempActivity extends AppCompatActivity {

    TextView txtViewTempNew;
    TextView txtViewTempCurrent;
    TextView txtLastUpdate;
    SeekBar seekBarTemp;
    SeekBar seekBarTempComfort;
    Room room = null;
    int temp;

    public void runCmd(eCmd cmd) {

        Intent data = new Intent();
        data.putExtra(Const.INTENT_NEW_CMD, (int) cmd.getValue());
        data.putExtra(Const.INTENT_ROOM_ID, room.id);
        setResult(RESULT_OK,data);
        finish();
    }

    public void onBtnAuto(View view) {
        runCmd(eCmd.AUTO);
    }

    public void onBtnManual(View view) {
        runCmd(eCmd.MANUAL);
    }

    public void onBtnOff(View view) {
        runCmd(eCmd.OFF);
    }

    public void onBtnOn(View view) {
        runCmd(eCmd.ON);
    }

    public void onBtnBoostOn(View view) {
        runCmd(eCmd.BOOST_ON);
    }

    public void onBtnBoostOff(View view) {
        runCmd(eCmd.BOOST_OFF);
    }

    public void onBtnEco(View view) {
        runCmd(eCmd.ECO);
    }

    public void onBtnComfort(View view) {
        runCmd(eCmd.COMFORT);
    }

    public void onBtnTemp(View view) {
        Button btn = (Button) view;
        int presetIndex = Integer.parseInt((String)btn.getTag());
        temp = room.presetTemp.get(presetIndex);
        requestNewTempAndClose(temp);
    }

    public void onBtnDec(View view) {
        if(temp>Const.TEMP_MIN) {
            temp -= Const.TEMP_STEP;
        }
        showTemp();
    }

    public void onBtnInc(View view) {
        if(temp<Const.TEMP_MAX) {
            temp += Const.TEMP_STEP;
        }
        showTemp();
    }

    public void requestNewTempAndClose(int t) {

        Intent data = new Intent();
        data.putExtra(Const.INTENT_NEW_TEMP, temp);
        data.putExtra(Const.INTENT_ROOM_ID, room.id);
        setResult(RESULT_OK,data);
        finish();
    }

    public void onBtnSetTemp(View view) {

        requestNewTempAndClose(temp);
    }

    void showTemp() {
        String str = Format.tempToString(temp);
        txtViewTempNew.setText(str);
        seekBarTemp.setProgress((temp-Const.TEMP_MIN)/Const.TEMP_STEP);

        int tempComfort = Math.min(Math.max(temp, Const.TEMP_MIN_COMFORT), Const.TEMP_MAX_COMFORT);
        tempComfort = (tempComfort-Const.TEMP_MIN_COMFORT) / Const.TEMP_STEP;
        seekBarTempComfort.setProgress(tempComfort);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_temp, menu);
        return true;
    }

    void updateButtonPresetTemp(int btnId, int value) {
        Button btn = (Button) findViewById(btnId);
        btn.setText(Format.tempToString(value));
    }

    void updateAllButtonPresetTemps() {

        updateButtonPresetTemp(R.id.btnTemp0, room.presetTemp.get(0));
        updateButtonPresetTemp(R.id.btnTemp1, room.presetTemp.get(1));
        updateButtonPresetTemp(R.id.btnTemp2, room.presetTemp.get(2));
        updateButtonPresetTemp(R.id.btnTemp3, room.presetTemp.get(3));
    }

    void setNewButtonPresetTemp(int presetId, int value) {
        room.presetTemp.set(presetId, value);
        updateAllButtonPresetTemps();
        Settings settings = new Settings(this);
        settings.saveRoom(room);
    }

    void showRoomSettingsActivity()
    {
        Intent intent = new Intent(this, RoomSettingsActivity.class);
        intent.putExtra(Const.INTENT_ROOM_ID, room.id);
        startActivityForResult(intent,Const.ACTIVITY_ROOM_SETTINGS);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        int id = item.getItemId();
        switch(id) {
            case R.id.action_save1:
                setNewButtonPresetTemp(0, temp);
                break;
            case R.id.action_save2:
                setNewButtonPresetTemp(1, temp);
                break;
            case R.id.action_save3:
                setNewButtonPresetTemp(2, temp);
                break;
            case R.id.action_save4:
                setNewButtonPresetTemp(3, temp);
                break;
            case R.id.action_room_setting:
                showRoomSettingsActivity();
                break;
        }
        return super.onOptionsItemSelected(item);
    }


    void showCurrentState() {
        String lastUpdate = "#";
        String str = "?";
        if(room != null) {
            str = room.autoActive ? "Auto" : "Manual";
            if (room.boostActive) str += " + Boost";
            str += "    " + Format.tempAndPercentToString(temp, room.percent);
            lastUpdate += room.msgCount;
            lastUpdate += ": ";
            if(room.lastUpdate != null) {
                DateFormat dateFormat = new SimpleDateFormat("HH:mm:ss  dd.MM.yyyy");
                lastUpdate += dateFormat.format(room.lastUpdate);
            } else {
                lastUpdate += "-";
            }
        }
        txtViewTempCurrent.setText(str);
        txtViewTempNew.setText(Format.tempToString(temp));
        txtLastUpdate.setText(lastUpdate);
    }


    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (requestCode == Const.ACTIVITY_ROOM_SETTINGS) {
            if (resultCode == RESULT_OK) {
                setTitle(room.name);
            }
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_temp);
        txtViewTempNew = (TextView) findViewById(R.id.txtViewTempNew);
        txtViewTempCurrent = (TextView) findViewById(R.id.txtViewTempCurrent);
        txtLastUpdate = (TextView) findViewById(R.id.txtLastUpdate);
        seekBarTemp = (SeekBar) findViewById(R.id.seekBarTemp);
        seekBarTempComfort = (SeekBar) findViewById(R.id.seekBarTempComfort);
        seekBarTemp.setMax((Const.TEMP_MAX - Const.TEMP_MIN) / Const.TEMP_STEP);
        seekBarTempComfort.setMax((Const.TEMP_MAX_COMFORT - Const.TEMP_MIN_COMFORT) / Const.TEMP_STEP);

        Intent intent = getIntent();

        int id = intent.getIntExtra(Const.INTENT_ROOM_ID, 0);
        room = ((MyApplication)getApplicationContext()).getHouse().getRoom(id);
        setTitle(room.name);
        temp = room.temp;

        if(temp<Const.TEMP_MIN) {
            temp = Const.TEMP_DEFAULT;
        }

        showCurrentState();
        updateAllButtonPresetTemps();

        seekBarTemp.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                if(fromUser) {
                    temp = (progress * Const.TEMP_STEP) + Const.TEMP_MIN;
                    showTemp();
                }
            }
            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {}
            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {}
        });

        seekBarTempComfort.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                if(fromUser) {
                    temp = (progress * Const.TEMP_STEP) + Const.TEMP_MIN_COMFORT;
                    showTemp();
                }
            }
            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {}
            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {}
        });

        showTemp();
    }
}
