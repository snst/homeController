package de.meek.myhome;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

import helpers.Format;

public class TempActivity extends AppCompatActivity {

    public void runCmd(String cmd) {

        Intent data = new Intent();
        data.putExtra("cmd",cmd);
        setResult(RESULT_OK,data);
        finish();
    }

    public void onBtnAuto(View view) {
        runCmd("auto");
    }

    public void onBtnManual(View view) {
        runCmd("manual");
    }

    public void onBtnOff(View view) {
        runCmd("off");
    }

    public void onBtnOn(View view) {
        runCmd("on");
    }

    public void onBtnBoostOn(View view) {
        runCmd("booston");
    }

    public void onBtnBoostOff(View view) {
        runCmd("boostoff");
    }

    public void onBtnEco(View view) {
        runCmd("eco");
    }

    public void onBtnComfort(View view) {
        runCmd("comfort");
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

    public void onBtnSetTemp(View view) {
        Intent data = new Intent();
        data.putExtra("newTemp",temp);
        setResult(RESULT_OK,data);
        finish();
    }

        TextView txtViewTemp;
    int temp = 0;
    int percent = 0;

    void showTemp() {
        String str = Format.tempToString(temp, percent);
        txtViewTemp.setText(str);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_temp);

        Intent intent = getIntent();
        String roomName = intent.getStringExtra("room");
        temp = intent.getIntExtra("temp", 0);
        percent = intent.getIntExtra("percent", 0);
        setTitle(roomName);

        if(temp<Const.TEMP_MIN) {
            temp = Const.TEMP_DEFAULT;
        }

        txtViewTemp = (TextView) findViewById(R.id.txtViewTemp2);
        showTemp();
    }
}
