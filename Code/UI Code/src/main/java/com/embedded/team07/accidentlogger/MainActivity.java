package com.embedded.team07.accidentlogger;
/*
* Team Id: 07
* Author List: Deepanjan Kundu, Karan Ganju, Sanket Kanjalkar, Roshan R
* Filename: MainActivity.java
* Theme: Accident logger
* Functions: onCreate(bundle), onCreateOptionsMenu(Menu), onOptionsItemSelected(MenuItem),run()
* Global Variables: Server, Rate, rating, filename, filename1, alertDialog, hasAcc, index
*
*/
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.graphics.Color;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.ImageView;
import android.widget.TextView;
import java.io.BufferedInputStream;
import java.io.BufferedWriter;
import java.io.File;
import java.io.BufferedReader;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.Writer;
import java.net.HttpURLConnection;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.net.URL;
import java.util.Enumeration;
public class MainActivity extends AppCompatActivity {
    //Server : used to display server address
    TextView Server;
    //Rate : used to display Ratings in text
    TextView Rate;
    //rating : used to show the star images for rating
    ImageView rating;
    //
    String filename = "/storage/emulated/0/myfile.txt";
    String filename1 = "/storage/emulated/0/info.txt";
    //alertDialog : Alert box to tell about the occurence of accident
    AlertDialog alertDialog;
    //hasAcc : boolean tells has the accident happened
    Boolean hasAcc=false;
    //index : used for parsing the server responses
    int index;
    //response : used for collecting server responses
    String response;
    /*
    * Function Name: onCreate(Bundle)
    *
    * Input: savedInstanceState
    *
    * Output: void
    *
    * Logic: set the textviews, image views, open the required files and start the client thread
    *        Automatically called on a new Android app instance
    */
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        //Initialize the AlertDialog Box and set its icon
        alertDialog = new AlertDialog.Builder(this).create();
        alertDialog.setIcon(R.mipmap.accident);
        //Select the layout to be used
        setContentView(R.layout.activity_main);
        // Initialize the File variables with
        File file = new File(filename);
        File file1 = new File(filename1);
        //create them if they don't already exist
        try {
            if (!file.exists() && !file.isDirectory()) {
                file.createNewFile();
            }
            if (!file1.exists() && !file1.isDirectory()) {
                file1.createNewFile();
                try {
                    Writer out = new BufferedWriter(new FileWriter(filename1, true));
                    out.append("Unknown");
                    out.close();
                } catch (Exception e) {
                    e.printStackTrace();
                }

            }
            //initialize the views and set their properties
            rating = (ImageView) findViewById(R.id.Rating);
            rating.setImageResource(R.drawable.star0);
            Server = (TextView) findViewById(R.id.Server);
            Server.setText("Server : http://embedded-roshanroshan.rhcloud.com/");
            Server.setTextColor(Color.BLACK);
            Server.setBackgroundColor(Color.GRAY);
            Rate = (TextView) findViewById(R.id.Rate);
            Rate.setTextColor(Color.BLACK);
            //display the previous stored driver rating
            try {
                BufferedReader in1 = new BufferedReader(new FileReader(filename1));
                String currate=in1.readLine();
                in1.close();
                if(currate.equals("Unknown")) {
                    Rate.setText("Driver Rating : Unknown");
                }
                else{
                    Rate.setText("Driver Rating :"+currate+"\n");
                    if(currate.contains("0"))rating.setImageResource(R.drawable.star0);
                    else if(currate.contains("1"))rating.setImageResource(R.drawable.star1);
                    else if(currate.contains("2"))rating.setImageResource(R.drawable.star2);
                    else if(currate.contains("3"))rating.setImageResource(R.drawable.star3);
                    else if(currate.contains("4"))rating.setImageResource(R.drawable.star4);
                    else if(currate.contains("5"))rating.setImageResource(R.drawable.star5);
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
            //start the client thread
            SampleClient myHTTPClient= new SampleClient();
            myHTTPClient.start();
        } catch (IOException e1) {
            e1.printStackTrace();
        }
    }
    /*
    * Function Name: onCreateOptionsMenu(Menu)
    *
    * Input: menu -> Interface for direct access to a previously created menu item. eg: options menu
    *
    * Output: boolean
    *
    * Logic: this adds items to the action bar if it is present
    */
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    /*
    * Function Name: onOptionsItemSelected(MenuItem item)
    *
    * Input: item -> Interface for direct access to a previously created menu item.
    *
    * Output: boolean :
    *
    * Logic: Handle action bar item clicks here. The action bar will
    *        automatically handle clicks on the Home/Up button, so long
    *        as you specify a parent activity in AndroidManifest.xml.
    */
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    private class SampleClient extends Thread {
        /*
        * Function Name: run()
        *
        * Logic: Open a HTTP connection with the server and send it a get command
        *       Read the input stream on the connection and parse the required data
        *       for reading the update in driver's rating occurence of accident and
        *       the location and also log the required data
        */

        @Override
        public void run() {
                //loop to poll the server to see if some info has been added
                while(true) {
                    try {
                        //Declare the url and start the http connection
                        URL url = new URL("http://embedded-roshanroshan.rhcloud.com/get/");
                        HttpURLConnection urlConnection = (HttpURLConnection) url.openConnection();
                        //declare input stream
                        InputStream is;
                        try {
                            //initialize the input stream for the url connection
                            is = new BufferedInputStream(urlConnection.getInputStream());
                            //response variable stores the string we get from the input stream that is the response from the server
                            while (true) {
                                response = "";
                                //read the stream byte by byte and line by line

                                while (true) {
                                    int temp = is.read();
                                    if (temp == -1 || (char) temp == '\n') break;
                                    response += (char) temp;
                                }
                                //continue to the next iteration if there is no response
                                if (response == "") break;
                                //check for driver rating message
                                index = response.indexOf("Driver_rating");
                                //if driver rating message then update the rating display by running a main activity UI thread
                                if (index != -1) {
                                    MainActivity.this.runOnUiThread(new Runnable() {
                                        @Override
                                        public void run() {
                                            String currat = response.substring(index + 14, index + 15);
                                            Rate.setText("Driver Rating : " + currat + "\n");
                                            if (currat.equals("0"))
                                                rating.setImageResource(R.drawable.star0);
                                            else if (currat.equals("1"))
                                                rating.setImageResource(R.drawable.star1);
                                            else if (currat.equals("2"))
                                                rating.setImageResource(R.drawable.star2);
                                            else if (currat.equals("3"))
                                                rating.setImageResource(R.drawable.star3);
                                            else if (currat.equals("4"))
                                                rating.setImageResource(R.drawable.star4);
                                            else if (currat.equals("5"))
                                                rating.setImageResource(R.drawable.star5);
                                        }
                                    });
                                    //log the latest rating
                                    try {
                                        Writer out1 = new FileWriter(filename1, false);
                                        out1.write(response.substring(index + 14, index + 15));
                                        out1.close();
                                    } catch (Exception e) {
                                        e.printStackTrace();
                                    }
                                    response = response.substring(0, index);
                                }
                                //look for accident message
                                index = -1;
                                index = response.indexOf("accident");
                                //if accident has occured  make the hasAcc variable true
                                if (index != -1) {
                                    hasAcc = true;
                                }
                                //look for the location message
                                index = -1;
                                index = response.indexOf("location");
                                //if found display the alert box for accident with location
                                if (index != -1) {
                                    MainActivity.this.runOnUiThread(new Runnable() {

                                        @Override
                                        public void run() {
                                            alertDialog.setTitle("Accident");
                                            alertDialog.setMessage(response);
                                            alertDialog.setButton("OK", new DialogInterface.OnClickListener() {
                                                public void onClick(DialogInterface dialog, int which) {
                                                }
                                            });
                                            alertDialog.show();
                                        }
                                    });
                                    hasAcc = false;
                                }
                                //log all the responses recieved
                                try {
                                    Writer out = new BufferedWriter(new FileWriter(filename, true));
                                    out.append(response);
                                    out.close();
                                } catch (Exception e) {
                                    e.printStackTrace();
                                }
                            }
                        }catch(Exception e){

                        }finally{
                            //disconnect the http connection
                            urlConnection.disconnect();
                        }
                    } catch (Exception e) {

                    }
                }
        }


    }
}


