package com.example.hcbled;

import androidx.appcompat.app.AppCompatActivity;

import android.annotation.SuppressLint;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.ImageButton;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

import org.eclipse.paho.android.service.MqttAndroidClient;
import org.eclipse.paho.client.mqttv3.IMqttActionListener;
import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.IMqttToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {

    static String MQTTHOST = "tcp://io.adafruit.com:1883";
    static String USERNAME = "iigenio";
    static String PASSWORD = "aio_PEDW50tYTMrEWDidOoDpMvFlZEVN";

    // CANAIS
    String feedsPWM = "iigenio/feeds/tempo-real.canais-pwm";

    String tmpFeed = "";

    MqttAndroidClient client;
    MqttConnectOptions options;
    TextView PercBranco;
    TextView PercAzul;
    TextView PercAzulRoyal;
    TextView PercVioleta;
    TextView PercTempLuminaria;
    TextView PercTempAquario;
    TextView valorPH;

    ProgressBar progressBarBranco;
    ProgressBar progressBarAzul;
    ProgressBar progressBarAzulRoyal;
    ProgressBar progressBarVioleta;
    ProgressBar progressBarTempLuminaria;
    ProgressBar progressBarTempAquario;

    ImageButton btnConfig, btnTeste;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        PercBranco = (TextView) findViewById(R.id.PercBranco);
        PercAzul = (TextView) findViewById(R.id.PercAzul);
        PercAzulRoyal = (TextView) findViewById(R.id.PercAzulRoyal);
        PercVioleta = (TextView) findViewById(R.id.PercVioleta);
        PercTempLuminaria = (TextView) findViewById(R.id.PercTemperaturaLumaria);
        PercTempAquario = (TextView) findViewById(R.id.PercTemperaturaAquario);

        progressBarBranco = (ProgressBar) findViewById(R.id.progressBarBranco);
        progressBarAzul = (ProgressBar) findViewById(R.id.progressBarAzul);
        progressBarAzulRoyal = (ProgressBar) findViewById(R.id.progressBarAzulRoyal);
        progressBarVioleta = (ProgressBar) findViewById(R.id.progressBarVioleta);
        progressBarTempLuminaria = (ProgressBar) findViewById(R.id.progressBarLuminaria);
        progressBarTempAquario = (ProgressBar) findViewById(R.id.progressBarAquario);

        valorPH = (TextView) findViewById(R.id.valorPH);
        btnConfig = (ImageButton)findViewById(R.id.imageButtonConfig);
        btnTeste = (ImageButton)findViewById(R.id.imageButtonTeste);

        ImageButton imageButtonConfig = (ImageButton) findViewById(R.id.imageButtonConfig);
        imageButtonConfig.setOnClickListener(this);

        ImageButton imageButtonTeste = (ImageButton) findViewById(R.id.imageButtonTeste);
        imageButtonTeste.setOnClickListener(this);

        String clientId = MqttClient.generateClientId();
        client = new MqttAndroidClient(this.getApplicationContext(), MQTTHOST, clientId);
        options = new MqttConnectOptions();
        options.setUserName(USERNAME);
        options.setPassword(PASSWORD.toCharArray());

        try {
            IMqttToken token = client.connect(options);
            token.setActionCallback(new IMqttActionListener() {
                @Override
                public void onSuccess(IMqttToken asyncActionToken) {
                    Toast.makeText(MainActivity.this, "Conectado ao MQTT", Toast.LENGTH_LONG).show();
                    setSubscription();
                }

                @Override
                public void onFailure(IMqttToken asyncActionToken, Throwable exception) {
                    Toast.makeText(MainActivity.this, "Não conectado ao MQTT", Toast.LENGTH_LONG).show();

                }
            });
        } catch (MqttException e) {
            e.printStackTrace();
        }

        client.setCallback(new MqttCallback() {
            @Override
            public void connectionLost(Throwable cause) {

            }

            @SuppressLint("SetTextI18n")
            @Override
            public void messageArrived(String topic, MqttMessage message) throws Exception {
                Toast.makeText(MainActivity.this, "Chegou mensagem do MQTT", Toast.LENGTH_LONG).show();

                tmpFeed = new String(message.getPayload());
                String[] strFeed = tmpFeed.split(";");

                PercBranco.setText("Branco: " + strFeed[0] + "%");
                progressBarBranco.setProgress(Integer.parseInt(strFeed[0]));

                PercAzul.setText("Azul: " + strFeed[1] + "%");
                progressBarAzul.setProgress(Integer.parseInt(strFeed[1]));

                PercAzulRoyal.setText("Azul Royal: " + strFeed[2] + "%");
                progressBarAzulRoyal.setProgress(Integer.parseInt(strFeed[2]));

                PercVioleta.setText("Violeta: " + strFeed[3] + "%");
                progressBarVioleta.setProgress(Integer.parseInt(strFeed[3]));

                PercTempLuminaria.setText("Luminária: " + strFeed[4] + "º");
                progressBarTempLuminaria.setProgress(Integer.parseInt(strFeed[4]));

                PercTempAquario.setText("Aquario: " + strFeed[5] + "º");
                progressBarTempAquario.setProgress(Integer.parseInt(strFeed[5]));

                valorPH.setText("PH: "  + strFeed[6] + "ppm");
            }

            @Override
            public void deliveryComplete(IMqttDeliveryToken token) {

            }
        });

    }
    @Override
    public void onClick(View v) {

        switch (v.getId()) {

            case R.id.imageButtonConfig:
                btnConfig.setImageResource(R.drawable.ic_baseline_settings_a48);

                Intent itConfig = new Intent(this, MainActivityConfig.class);
                startActivity(itConfig);
                break;

            case R.id.imageButtonTeste:
                btnTeste.setImageResource(R.drawable.ic_baseline_luz_pink_48);

                Intent itTestar = new Intent(this, MainActivityTestar.class);
                startActivity(itTestar);
                break;

            default:
                break;
        }
    }

    private void setSubscription() {
        try {
            // OUTROS
            client.subscribe(feedsPWM, 0);

        } catch (MqttException e) {
            e.printStackTrace();
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        btnConfig.setImageResource(R.drawable.ic_baseline_settings_48);
        btnTeste.setImageResource(R.drawable.ic_baseline_luz_azulroyal_48);
    }
}