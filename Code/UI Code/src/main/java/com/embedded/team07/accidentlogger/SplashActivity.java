package com.embedded.team07.accidentlogger;

import android.content.Intent;
import android.os.Bundle;
import android.os.SystemClock;
import android.support.v7.app.AppCompatActivity;

import java.util.concurrent.TimeUnit;

public class SplashActivity extends AppCompatActivity {
    /*
    * Function Name: onCreate(Bundle)
    *
    * Input: savedInstanceState
    *
    * Output: void
    *
    * Logic: display the splash with a delay of 5 seconds.
    *        Automatically shift to main activity
    */
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        SystemClock.sleep(TimeUnit.SECONDS.toMillis(5));
        Intent intent = new Intent(this, MainActivity.class);
        startActivity(intent);
        finish();
    }
}
