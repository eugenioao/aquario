package com.example.hcbled;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.view.View;
import android.widget.SeekBar;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;

import org.eclipse.paho.android.service.MqttAndroidClient;
import org.eclipse.paho.client.mqttv3.IMqttActionListener;
import org.eclipse.paho.client.mqttv3.IMqttToken;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;

public class MainActivityTestar extends AppCompatActivity {

    TextView ConfiguracaoGeralBranco;
    TextView ConfiguracaoGeralAzul;
    TextView ConfiguracaoGeralAzulRoyal;
    TextView ConfiguracaoGeralVioleta;

    private String valorSeekBarBranco;
    private String valorSeekBarAzul;
    private String valorSeekBarAzulRoyal;
    private String valorSeekBarVioleta;

    Switch CanaisLigarTodos;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main_testar);

        ConfiguracaoGeralBranco = (TextView) findViewById(R.id.PercBranco);
        SeekBar Branco = (SeekBar) findViewById(R.id.seekBarBranco);
        valorSeekBarBranco = String.valueOf(Branco.getProgress());

        ConfiguracaoGeralAzul = (TextView) findViewById(R.id.PercAzul);
        SeekBar Azul = (SeekBar) findViewById(R.id.seekBarAzul);
        valorSeekBarAzul = String.valueOf(Azul.getProgress());

        ConfiguracaoGeralAzulRoyal = (TextView) findViewById(R.id.PercAzulRoyal);
        SeekBar AzulRoyal = (SeekBar) findViewById(R.id.seekBarAzulRoyal);
        valorSeekBarAzulRoyal = String.valueOf(AzulRoyal.getProgress());

        ConfiguracaoGeralVioleta = (TextView) findViewById(R.id.PercVioleta);
        SeekBar Violeta = (SeekBar) findViewById(R.id.seekBarVioleta);
        valorSeekBarVioleta = String.valueOf(Violeta.getProgress());

        CanaisLigarTodos = (Switch) findViewById(R.id.switchLigarTudo);

        CanaisLigarTodos.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Switch TestarOn = (Switch) view;
                if (TestarOn.isChecked()) {
                    ConfiguracaoGeralBranco.setText("  Percentual: 100%");
                    Branco.setProgress(100);
                    ConfiguracaoGeralAzul.setText("  Percentual: 100%");
                    Azul.setProgress(100);
                    ConfiguracaoGeralAzulRoyal.setText("  Percentual: 100%");
                    AzulRoyal.setProgress(100);
                    ConfiguracaoGeralVioleta.setText("  Percentual: 100%");
                    Violeta.setProgress(100);

                    valorSeekBarBranco = "100";
                    valorSeekBarAzul = "100";
                    valorSeekBarAzulRoyal = "100";
                    valorSeekBarVioleta = "100";

                } else {
                    ConfiguracaoGeralBranco.setText("  Percentual: 0%");
                    Branco.setProgress(0);
                    ConfiguracaoGeralAzul.setText("  Percentual: 0%");
                    Azul.setProgress(0);
                    ConfiguracaoGeralAzulRoyal.setText("  Percentual: 0%");
                    AzulRoyal.setProgress(0);
                    ConfiguracaoGeralVioleta.setText("  Percentual: 0%");
                    Violeta.setProgress(0);

                    valorSeekBarBranco = "0";
                    valorSeekBarAzul = "0";
                    valorSeekBarAzulRoyal = "0";
                    valorSeekBarVioleta = "0";

                }
                Publicar_Canais_Teste();
            }
        });

        Branco.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
                ConfiguracaoGeralBranco.setText("  Percentual: " + i + "%");
                valorSeekBarBranco = String.valueOf(i);

                Publicar_Canais_Teste();
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });
        Azul.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
                ConfiguracaoGeralAzul.setText("  Percentual: " + i + "%");
                valorSeekBarAzul = String.valueOf(i);

                Publicar_Canais_Teste();
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });
        AzulRoyal.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
                ConfiguracaoGeralAzulRoyal.setText("  Percentual: " + i + "%");
                valorSeekBarAzulRoyal = String.valueOf(i);

                Publicar_Canais_Teste();
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });
        Violeta.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
                ConfiguracaoGeralVioleta.setText("  Percentual: " + i + "%");
                valorSeekBarVioleta = String.valueOf(i);

                Publicar_Canais_Teste();
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });

    }

    public void Publicar_Canais_Teste() {

        String MQTTHOST = "tcp://io.adafruit.com:1883";
        String USERNAME = "USERNAME";
        String PASSWORD = "aio_CODE";

        // CANAIS
        String feedsTestarCanais = USERNAME + "/feeds/testes.canais";

        MqttAndroidClient client;
        MqttConnectOptions options;

        String clientId = MqttClient.generateClientId();
        client = new MqttAndroidClient(this.getApplicationContext(), MQTTHOST, clientId);
        options = new MqttConnectOptions();

        options.setCleanSession(true);

        options.setUserName(USERNAME);
        options.setPassword(PASSWORD.toCharArray());

        try {
            IMqttToken token = client.connect(options);
            token.setActionCallback(new IMqttActionListener() {
                @Override
                public void onSuccess(IMqttToken asyncActionToken) {

                    try {

                        MqttMessage message = new MqttMessage();
                        message.setRetained(false);

                        String tmpString = valorSeekBarBranco + ";" + valorSeekBarAzul + ";" + valorSeekBarAzulRoyal + ";" + valorSeekBarVioleta;
                        message.setPayload(tmpString.getBytes());
                        try {
                            client.publish(feedsTestarCanais, message);
                        } catch (MqttException e) {
                            Toast.makeText(MainActivityTestar.this, "Falhou ao publicar:" + tmpString, Toast.LENGTH_LONG).show();
                            e.printStackTrace();
                        }

                        client.disconnect();

                    } catch (MqttException e) {
                        Toast.makeText(MainActivityTestar.this, "Conexão falhou com o MQTT", Toast.LENGTH_LONG).show();
                        e.printStackTrace();
                    }

                }

                @Override
                public void onFailure(IMqttToken asyncActionToken, Throwable exception) {
                    Toast.makeText(MainActivityTestar.this, "Não conectado ao MQTT", Toast.LENGTH_LONG).show();

                }
            });

        } catch (MqttException e) {
            Toast.makeText(MainActivityTestar.this, "Conexão falhou com o MQTT", Toast.LENGTH_LONG).show();
            e.printStackTrace();
        }

    }

}
