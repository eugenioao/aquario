package com.example.hcbled;

import androidx.appcompat.app.AppCompatActivity;

import android.app.TimePickerDialog;
import android.os.Bundle;
import android.view.View;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.SeekBar;
import android.widget.TextView;
import android.widget.TimePicker;
import android.widget.Toast;

import com.melnykov.fab.FloatingActionButton;

import org.eclipse.paho.android.service.MqttAndroidClient;
import org.eclipse.paho.client.mqttv3.IMqttActionListener;
import org.eclipse.paho.client.mqttv3.IMqttToken;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.Calendar;

public class MainActivityConfig extends AppCompatActivity {

    TextView ConfiguracaoGeralBranco;
    TextView ConfiguracaoEfeitoBranco;
    TextView ConfiguracaoGeralAzul;
    TextView ConfiguracaoEfeitoAzul;
    TextView ConfiguracaoGeralAzulRoyal;
    TextView ConfiguracaoEfeitoAzulRoyal;
    TextView ConfiguracaoGeralVioleta;
    TextView ConfiguracaoEfeitoVioleta;
    TextView ConfiguracaoGeralTemperaturaAquario;
    TextView ConfiguracaoGeralTemperaturaLumaria;
    TextView HoraLigarBranco;
    TextView HoraDesligarBranco;
    TextView HoraLigarAzul;
    TextView HoraDesligarAzul;
    TextView HoraLigarAzulRoyal;
    TextView HoraDesligarAzulRoyal;
    TextView HoraLigarVioleta;
    TextView HoraDesligarVioleta;

    private String valorSeekBarBranco;
    private String valorSeekBarBrancoEfeito;

    private String valorSeekBarAzul;
    private String valorSeekBarAzulEfeito;

    private String valorSeekBarAzulRoyal;
    private String valorSeekBarAzulRoyalEfeito;

    private String valorSeekBarVioleta;
    private String valorSeekBarVioletaEfeito;

    private String valorSeekBarTempAquario;
    private String valorSeekBarTempLuminaria;

    String ARQUIVO_PROPRIEDADES = "propriedades.config";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main_config);

        ConfiguracaoGeralBranco = (TextView) findViewById(R.id.PercBranco);
        SeekBar Branco = (SeekBar) findViewById(R.id.seekBarBranco);
        valorSeekBarBranco = String.valueOf(Branco.getProgress());

        ConfiguracaoEfeitoBranco = (TextView) findViewById(R.id.PercEfeitoBranco);
        SeekBar BrancoEfeito = (SeekBar) findViewById(R.id.seekBarEfeitoBranco);
        valorSeekBarBrancoEfeito = String.valueOf(BrancoEfeito.getProgress());

        ConfiguracaoGeralAzul = (TextView) findViewById(R.id.PercAzul);
        SeekBar Azul = (SeekBar) findViewById(R.id.seekBarAzul);
        valorSeekBarAzul = String.valueOf(Azul.getProgress());

        ConfiguracaoEfeitoAzul = (TextView) findViewById(R.id.PercEfeitoAzul);
        SeekBar AzulEfeito = (SeekBar) findViewById(R.id.seekBarEfeitoAzul);
        valorSeekBarAzulEfeito = String.valueOf(AzulEfeito.getProgress());

        ConfiguracaoGeralAzulRoyal = (TextView) findViewById(R.id.PercAzulRoyal);
        SeekBar AzulRoyal = (SeekBar) findViewById(R.id.seekBarAzulRoyal);
        valorSeekBarAzulRoyal = String.valueOf(AzulRoyal.getProgress());

        ConfiguracaoEfeitoAzulRoyal = (TextView) findViewById(R.id.PercEfeitoAzulRoyal);
        SeekBar AzulRoyalEfeito = (SeekBar) findViewById(R.id.seekBarEfeitoAzulRoyal);
        valorSeekBarAzulRoyalEfeito = String.valueOf(AzulRoyalEfeito.getProgress());

        ConfiguracaoGeralVioleta = (TextView) findViewById(R.id.PercVioleta);
        SeekBar Violeta = (SeekBar) findViewById(R.id.seekBarVioleta);
        valorSeekBarVioleta = String.valueOf(Violeta.getProgress());

        ConfiguracaoEfeitoVioleta = (TextView) findViewById(R.id.PercEfeitoVioleta);
        SeekBar VioletaEfeito = (SeekBar) findViewById(R.id.seekBarEfeitoVioleta);
        valorSeekBarVioletaEfeito = String.valueOf(VioletaEfeito.getProgress());

        ConfiguracaoGeralTemperaturaLumaria = (TextView) findViewById(R.id.tvTempLumaria);
        SeekBar TemperaturaLumaria = (SeekBar) findViewById(R.id.seekBarTempLumaria);
        valorSeekBarTempLuminaria = String.valueOf(TemperaturaLumaria.getProgress());

        ConfiguracaoGeralTemperaturaAquario = (TextView) findViewById(R.id.tvTempAquario);
        SeekBar TemperaturaAquario = (SeekBar) findViewById(R.id.seekBarTempAquario);
        valorSeekBarTempAquario = String.valueOf(TemperaturaAquario.getProgress());

        HoraLigarBranco = (EditText) findViewById(R.id.HoraLigarBranco);
        HoraDesligarBranco = (EditText) findViewById(R.id.HoraDesligarBranco);

        HoraLigarAzul = (EditText) findViewById(R.id.HoraLigarAzul);
        HoraDesligarAzul = (EditText) findViewById(R.id.HoraDesligarAzul);

        HoraLigarAzulRoyal = (EditText) findViewById(R.id.HoraLigarAzulRoyal);
        HoraDesligarAzulRoyal = (EditText) findViewById(R.id.HoraDesligarAzulRoyal);

        HoraLigarVioleta = (EditText) findViewById(R.id.HoraLigarVioleta);
        HoraDesligarVioleta = (EditText) findViewById(R.id.HoraDesligarVioleta);

        try {

            String tmpLinha = null;
            File tmpCaminho = getFileStreamPath(ARQUIVO_PROPRIEDADES);
            if (tmpCaminho.exists()) {
                //Toast.makeText(this, "Lendo arquivo: " + ARQUIVO_PROPRIEDADES, Toast.LENGTH_LONG).show();

                FileInputStream tmpArquivo = openFileInput(ARQUIVO_PROPRIEDADES);

                InputStreamReader tmpISR = new InputStreamReader(tmpArquivo);
                BufferedReader tmpConteudo = new BufferedReader(tmpISR);

                tmpLinha = tmpConteudo.readLine();

                while (tmpLinha != null) {
                    String[] tmpColunas = tmpLinha.split(";");

                    if (tmpColunas[0].equals("1")) {
                        HoraLigarBranco.setText(tmpColunas[1]);
                        HoraDesligarBranco.setText(tmpColunas[2]);
                        ConfiguracaoGeralBranco.setText("  Percentual: " + tmpColunas[4] + "%");
                        Branco.setProgress(Integer.parseInt(tmpColunas[4]));
                        ConfiguracaoEfeitoBranco.setText("  Tempo para Efeito: " + tmpColunas[3] + "m");
                        BrancoEfeito.setProgress(Integer.parseInt(tmpColunas[3]));
                        valorSeekBarBranco = tmpColunas[4];
                        valorSeekBarBrancoEfeito = tmpColunas[3];

                    } else if (tmpColunas[0].equals("2")) {
                        HoraLigarAzul.setText(tmpColunas[1]);
                        HoraDesligarAzul.setText(tmpColunas[2]);
                        ConfiguracaoGeralAzul.setText("  Percentual: " + tmpColunas[4] + "%");
                        AzulEfeito.setProgress(Integer.parseInt(tmpColunas[3]));
                        ConfiguracaoEfeitoAzul.setText("  Tempo para Efeito: " + tmpColunas[3] + "m");
                        Azul.setProgress(Integer.parseInt(tmpColunas[4]));
                        valorSeekBarAzul = tmpColunas[4];
                        valorSeekBarAzulEfeito = tmpColunas[3];

                    } else if (tmpColunas[0].equals("3")) {
                        HoraLigarAzulRoyal.setText(tmpColunas[1]);
                        HoraDesligarAzulRoyal.setText(tmpColunas[2]);
                        ConfiguracaoGeralAzulRoyal.setText("  Percentual: " + tmpColunas[4] + "%");
                        AzulRoyalEfeito.setProgress(Integer.parseInt(tmpColunas[3]));
                        ConfiguracaoEfeitoAzulRoyal.setText("  Tempo para Efeito: " + tmpColunas[3] + "m");
                        AzulRoyal.setProgress(Integer.parseInt(tmpColunas[4]));
                        valorSeekBarAzulRoyal = tmpColunas[4];
                        valorSeekBarAzulRoyalEfeito = tmpColunas[3];

                    } else if (tmpColunas[0].equals("4")) {
                        HoraLigarVioleta.setText(tmpColunas[1]);
                        HoraDesligarVioleta.setText(tmpColunas[2]);
                        ConfiguracaoGeralVioleta.setText("  Percentual: " + tmpColunas[4] + "%");
                        VioletaEfeito.setProgress(Integer.parseInt(tmpColunas[3]));
                        ConfiguracaoEfeitoVioleta.setText("  Tempo para Efeito: " + tmpColunas[3] + "m");
                        Violeta.setProgress(Integer.parseInt(tmpColunas[4]));
                        valorSeekBarVioleta = tmpColunas[4];
                        valorSeekBarVioletaEfeito = tmpColunas[3];

                    } else if (tmpColunas[0].equals("5")) {
                        ConfiguracaoGeralTemperaturaLumaria.setText("  Luminária - Ligar Coolers em: " + tmpColunas[1] + "°");
                        TemperaturaLumaria.setProgress(Integer.parseInt(tmpColunas[1]));
                        valorSeekBarTempLuminaria = tmpColunas[1];

                    } else if (tmpColunas[0].equals("6")) {
                        ConfiguracaoGeralTemperaturaAquario.setText("  Aquario - Ligar Coolers em: " + tmpColunas[1] + "°");
                        TemperaturaAquario.setProgress(Integer.parseInt(tmpColunas[1]));
                        valorSeekBarTempAquario = tmpColunas[1];

                    }
                    tmpLinha = tmpConteudo.readLine();
                }
                tmpISR.close();
            } else {
                tmpLinha = "1;13:00;20:00;50;95\n";
                tmpLinha += "2;9:30;21:00;40;95\n";
                tmpLinha += "3;10:00;20:30;30;95\n";
                tmpLinha += "4;9:00;20:00;30;85\n";
                tmpLinha += "5;43\n";
                tmpLinha += "6;26\n";

                FileOutputStream tmpArquivo = openFileOutput(ARQUIVO_PROPRIEDADES, MODE_PRIVATE);
                tmpArquivo.write(tmpLinha.getBytes());
                tmpArquivo.close();

                Toast.makeText(this, "Gravando arquivo: " + ARQUIVO_PROPRIEDADES, Toast.LENGTH_LONG).show();

            }

        } catch (Exception error) {
            // Exception
        }

        FloatingActionButton fabSalvarEnviar = findViewById(R.id.fabSalvarEnviar);
        fabSalvarEnviar.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                /*Snackbar.make(view, "Here's a Snackbar", Snackbar.LENGTH_LONG)
                        .setAction("Action", null).show(); */
                Publicar_Salvar();
            }
        });

        Branco.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
                ConfiguracaoGeralBranco.setText("  Percentual: " + i + "%");
                valorSeekBarBranco = String.valueOf(i);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });
        BrancoEfeito.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
                ConfiguracaoEfeitoBranco.setText("  Tempo para Efeito: " + i + "m");
                valorSeekBarBrancoEfeito = String.valueOf(i);
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
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });
        AzulEfeito.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
                ConfiguracaoEfeitoAzul.setText("  Tempo para Efeito: " + i + "m");
                valorSeekBarAzulEfeito = String.valueOf(i);
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
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });
        AzulRoyalEfeito.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
                ConfiguracaoEfeitoAzulRoyal.setText("  Tempo para Efeito: " + i + "m");
                valorSeekBarAzulRoyalEfeito = String.valueOf(i);
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
                valorSeekBarVioletaEfeito = String.valueOf(i);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });
        VioletaEfeito.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
                ConfiguracaoEfeitoVioleta.setText("  Tempo para Efeito: " + i + "m");
                valorSeekBarVioletaEfeito = String.valueOf(i);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });
        TemperaturaLumaria.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
                ConfiguracaoGeralTemperaturaLumaria.setText("  Luminaria - Ligar Coolers em: " + i + "°");
                valorSeekBarTempLuminaria = String.valueOf(i);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });
        TemperaturaAquario.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int i, boolean b) {
                ConfiguracaoGeralTemperaturaAquario.setText("  Aquario - Ligar Coolers em: " + i + "°");
                valorSeekBarTempAquario = String.valueOf(i);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });

    }

    public void PegarHoraLigarBranco(View v) {

        final Calendar MinhaHora = Calendar.getInstance();

        String tmpHora = HoraLigarBranco.getText().toString();
        String[] strHora = tmpHora.split(":");
        int hora = Integer.parseInt(strHora[0]);
        int minuto = Integer.parseInt(strHora[1]);

        MinhaHora.set(Calendar.HOUR_OF_DAY, hora);
        MinhaHora.set(Calendar.MINUTE, minuto);

        TimePickerDialog.OnTimeSetListener meuTimeListener = new TimePickerDialog.OnTimeSetListener() {
            @Override
            public void onTimeSet(TimePicker view, int HoraSelecionada, int MinutoSelecionado) {
                if (view.isShown()) {
                    HoraLigarBranco.setText(HoraSelecionada + ":" + MinutoSelecionado);
                }
            }
        };
        TimePickerDialog timePickerDialog = new TimePickerDialog(this, android.R.style.Theme_Holo_Light_Dialog_NoActionBar, meuTimeListener, hora, minuto, true);
        timePickerDialog.setTitle("Selecione a Hora:");
        timePickerDialog.getWindow().setBackgroundDrawableResource(android.R.color.transparent);
        timePickerDialog.show();
    }

    public void PegarHoraDesligarBranco(View v) {

        final Calendar MinhaHora = Calendar.getInstance();

        String tmpHora = HoraDesligarBranco.getText().toString();
        String[] strHora = tmpHora.split(":");
        int hora = Integer.parseInt(strHora[0]);
        int minuto = Integer.parseInt(strHora[1]);

        MinhaHora.set(Calendar.HOUR_OF_DAY, hora);
        MinhaHora.set(Calendar.MINUTE, minuto);

        TimePickerDialog.OnTimeSetListener meuTimeListener = new TimePickerDialog.OnTimeSetListener() {
            @Override
            public void onTimeSet(TimePicker view, int HoraSelecionada, int MinutoSelecionado) {
                if (view.isShown()) {
                    HoraDesligarBranco.setText(HoraSelecionada + ":" + MinutoSelecionado);
                }
            }
        };
        TimePickerDialog timePickerDialog = new TimePickerDialog(this, android.R.style.Theme_Holo_Light_Dialog_NoActionBar, meuTimeListener, hora, minuto, true);
        timePickerDialog.setTitle("Selecione a Hora:");
        timePickerDialog.getWindow().setBackgroundDrawableResource(android.R.color.transparent);
        timePickerDialog.show();
    }

    public void PegarHoraLigarAzul(View v) {

        final Calendar MinhaHora = Calendar.getInstance();

        String tmpHora = HoraLigarAzul.getText().toString();
        String[] strHora = tmpHora.split(":");
        int hora = Integer.parseInt(strHora[0]);
        int minuto = Integer.parseInt(strHora[1]);

        MinhaHora.set(Calendar.HOUR_OF_DAY, hora);
        MinhaHora.set(Calendar.MINUTE, minuto);

        TimePickerDialog.OnTimeSetListener meuTimeListener = new TimePickerDialog.OnTimeSetListener() {
            @Override
            public void onTimeSet(TimePicker view, int HoraSelecionada, int MinutoSelecionado) {
                if (view.isShown()) {
                    HoraLigarAzul.setText(HoraSelecionada + ":" + MinutoSelecionado);
                }
            }
        };
        TimePickerDialog timePickerDialog = new TimePickerDialog(this, android.R.style.Theme_Holo_Light_Dialog_NoActionBar, meuTimeListener, hora, minuto, true);
        timePickerDialog.setTitle("Selecione a Hora:");
        timePickerDialog.getWindow().setBackgroundDrawableResource(android.R.color.transparent);
        timePickerDialog.show();
    }

    public void PegarHoraDesligarAzul(View v) {

        final Calendar MinhaHora = Calendar.getInstance();

        String tmpHora = HoraDesligarAzul.getText().toString();
        String[] strHora = tmpHora.split(":");
        int hora = Integer.parseInt(strHora[0]);
        int minuto = Integer.parseInt(strHora[1]);

        MinhaHora.set(Calendar.HOUR_OF_DAY, hora);
        MinhaHora.set(Calendar.MINUTE, minuto);

        TimePickerDialog.OnTimeSetListener meuTimeListener = new TimePickerDialog.OnTimeSetListener() {
            @Override
            public void onTimeSet(TimePicker view, int HoraSelecionada, int MinutoSelecionado) {
                if (view.isShown()) {
                    HoraDesligarAzul.setText(HoraSelecionada + ":" + MinutoSelecionado);
                }
            }
        };
        TimePickerDialog timePickerDialog = new TimePickerDialog(this, android.R.style.Theme_Holo_Light_Dialog_NoActionBar, meuTimeListener, hora, minuto, true);
        timePickerDialog.setTitle("Selecione a Hora:");
        timePickerDialog.getWindow().setBackgroundDrawableResource(android.R.color.transparent);
        timePickerDialog.show();
    }

    public void PegarHoraLigarAzulRoyal(View v) {

        final Calendar MinhaHora = Calendar.getInstance();

        String tmpHora = HoraLigarAzulRoyal.getText().toString();
        String[] strHora = tmpHora.split(":");
        int hora = Integer.parseInt(strHora[0]);
        int minuto = Integer.parseInt(strHora[1]);

        MinhaHora.set(Calendar.HOUR_OF_DAY, hora);
        MinhaHora.set(Calendar.MINUTE, minuto);

        TimePickerDialog.OnTimeSetListener meuTimeListener = new TimePickerDialog.OnTimeSetListener() {
            @Override
            public void onTimeSet(TimePicker view, int HoraSelecionada, int MinutoSelecionado) {
                if (view.isShown()) {
                    HoraLigarAzulRoyal.setText(HoraSelecionada + ":" + MinutoSelecionado);
                }
            }
        };
        TimePickerDialog timePickerDialog = new TimePickerDialog(this, android.R.style.Theme_Holo_Light_Dialog_NoActionBar, meuTimeListener, hora, minuto, true);
        timePickerDialog.setTitle("Selecione a Hora:");
        timePickerDialog.getWindow().setBackgroundDrawableResource(android.R.color.transparent);
        timePickerDialog.show();
    }

    public void PegarHoraDesligarAzulRoyal(View v) {

        final Calendar MinhaHora = Calendar.getInstance();

        String tmpHora = HoraDesligarAzulRoyal.getText().toString();
        String[] strHora = tmpHora.split(":");
        int hora = Integer.parseInt(strHora[0]);
        int minuto = Integer.parseInt(strHora[1]);

        MinhaHora.set(Calendar.HOUR_OF_DAY, hora);
        MinhaHora.set(Calendar.MINUTE, minuto);

        TimePickerDialog.OnTimeSetListener meuTimeListener = new TimePickerDialog.OnTimeSetListener() {
            @Override
            public void onTimeSet(TimePicker view, int HoraSelecionada, int MinutoSelecionado) {
                if (view.isShown()) {
                    HoraDesligarAzulRoyal.setText(HoraSelecionada + ":" + MinutoSelecionado);
                }
            }
        };
        TimePickerDialog timePickerDialog = new TimePickerDialog(this, android.R.style.Theme_Holo_Light_Dialog_NoActionBar, meuTimeListener, hora, minuto, true);
        timePickerDialog.setTitle("Selecione a Hora:");
        timePickerDialog.getWindow().setBackgroundDrawableResource(android.R.color.transparent);
        timePickerDialog.show();
    }

    public void PegarHoraLigarVioleta(View v) {

        final Calendar MinhaHora = Calendar.getInstance();

        String tmpHora = HoraLigarVioleta.getText().toString();
        String[] strHora = tmpHora.split(":");
        int hora = Integer.parseInt(strHora[0]);
        int minuto = Integer.parseInt(strHora[1]);

        MinhaHora.set(Calendar.HOUR_OF_DAY, hora);
        MinhaHora.set(Calendar.MINUTE, minuto);

        TimePickerDialog.OnTimeSetListener meuTimeListener = new TimePickerDialog.OnTimeSetListener() {
            @Override
            public void onTimeSet(TimePicker view, int HoraSelecionada, int MinutoSelecionado) {
                if (view.isShown()) {
                    HoraLigarVioleta.setText(HoraSelecionada + ":" + MinutoSelecionado);
                }
            }
        };
        TimePickerDialog timePickerDialog = new TimePickerDialog(this, android.R.style.Theme_Holo_Light_Dialog_NoActionBar, meuTimeListener, hora, minuto, true);
        timePickerDialog.setTitle("Selecione a Hora:");
        timePickerDialog.getWindow().setBackgroundDrawableResource(android.R.color.transparent);
        timePickerDialog.show();
    }

    public void PegarHoraDesligarVioleta(View v) {

        final Calendar MinhaHora = Calendar.getInstance();

        String tmpHora = HoraDesligarVioleta.getText().toString();
        String[] strHora = tmpHora.split(":");
        int hora = Integer.parseInt(strHora[0]);
        int minuto = Integer.parseInt(strHora[1]);

        MinhaHora.set(Calendar.HOUR_OF_DAY, hora);
        MinhaHora.set(Calendar.MINUTE, minuto);

        TimePickerDialog.OnTimeSetListener meuTimeListener = new TimePickerDialog.OnTimeSetListener() {
            @Override
            public void onTimeSet(TimePicker view, int HoraSelecionada, int MinutoSelecionado) {
                if (view.isShown()) {
                    HoraDesligarVioleta.setText(HoraSelecionada + ":" + MinutoSelecionado);
                }
            }
        };
        TimePickerDialog timePickerDialog = new TimePickerDialog(this, android.R.style.Theme_Holo_Light_Dialog_NoActionBar, meuTimeListener, hora, minuto, true);
        timePickerDialog.setTitle("Selecione a Hora:");
        timePickerDialog.getWindow().setBackgroundDrawableResource(android.R.color.transparent);
        timePickerDialog.show();
    }

    public void Publicar_Salvar() {

        String MQTTHOST = "tcp://io.adafruit.com:1883";
        String USERNAME = "USERNAME";
        String PASSWORD = "aio_CODE";

        // CANAIS
        String feedsBranco = USERNAME + "/feeds/canal-branco";
        String feedsAzul = USERNAME + "/feeds/canal-azul";
        String feedsAzulRoyal = USERNAME + "/feeds/canal-azul-royal";
        String feedsVioleta = USERNAME + "/feeds/canal-violeta";

        // TEMPERATURA
        String feedsTempArquario = USERNAME + "/feeds/temp-aquario";
        String feedsTempLuminaria = USERNAME + "/feeds/temp-luminaria";

        String tmpHoraLigarBranco = HoraLigarBranco.getText().toString();
        String tmpHoraDesligarBranco = HoraDesligarBranco.getText().toString();

        String tmpHoraLigarAzul = HoraLigarAzul.getText().toString();
        String tmpHoraDesligarAzul = HoraDesligarAzul.getText().toString();

        String tmpHoraLigarAzulRoyal = HoraLigarAzulRoyal.getText().toString();
        String tmpHoraDesligarAzulRoyal = HoraDesligarAzulRoyal.getText().toString();

        String tmpHoraLigarVioleta = HoraLigarVioleta.getText().toString();
        String tmpHoraDesligarVioleta = HoraDesligarVioleta.getText().toString();

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

                        String tmpLinha;

                        MqttMessage message = new MqttMessage();
                        message.setRetained(true);

                        String tmpStringBranco = tmpHoraLigarBranco + ";" + tmpHoraDesligarBranco + ";" + valorSeekBarBrancoEfeito + ";" + valorSeekBarBranco;
                        message.setPayload(tmpStringBranco.getBytes());
                        try {
                            client.publish(feedsBranco, message);
                        } catch (MqttException e) {
                            Toast.makeText(MainActivityConfig.this, "FALHOU publicar:" + tmpStringBranco, Toast.LENGTH_LONG).show();
                            e.printStackTrace();
                        }

                        String tmpStringAzul = tmpHoraLigarAzul + ";" + tmpHoraDesligarAzul + ";" + valorSeekBarAzulEfeito + ";" + valorSeekBarAzul;
                        message.setPayload(tmpStringAzul.getBytes());
                        try {
                            client.publish(feedsAzul, message);
                        } catch (MqttException e) {
                            Toast.makeText(MainActivityConfig.this, "FALHOU publicar:" + tmpStringAzul, Toast.LENGTH_LONG).show();
                            e.printStackTrace();
                        }

                        String tmpStringAzulRoyal = tmpHoraLigarAzulRoyal + ";" + tmpHoraDesligarAzulRoyal + ";" + valorSeekBarAzulRoyalEfeito + ";" + valorSeekBarAzulRoyal;
                        message.setPayload(tmpStringAzulRoyal.getBytes());
                        try {
                            client.publish(feedsAzulRoyal, message);
                        } catch (MqttException e) {
                            Toast.makeText(MainActivityConfig.this, "FALHOU publicar:" + tmpStringAzulRoyal, Toast.LENGTH_LONG).show();
                            e.printStackTrace();
                        }

                        String tmpStringVioleta = tmpHoraLigarVioleta + ";" + tmpHoraDesligarVioleta + ";" + valorSeekBarVioletaEfeito + ";" + valorSeekBarVioleta;
                        message.setPayload(tmpStringVioleta.getBytes());
                        try {
                            client.publish(feedsVioleta, message);
                        } catch (MqttException e) {
                            Toast.makeText(MainActivityConfig.this, "FALHOU publicar:" + tmpStringVioleta, Toast.LENGTH_LONG).show();
                            e.printStackTrace();
                        }

                        message.setPayload(valorSeekBarTempLuminaria.getBytes());
                        try {
                            client.publish(feedsTempLuminaria, message);
                        } catch (MqttException e) {
                            Toast.makeText(MainActivityConfig.this, "FALHOU publicar:" + feedsTempLuminaria, Toast.LENGTH_LONG).show();
                            e.printStackTrace();
                        }

                        message.setPayload(valorSeekBarTempAquario.getBytes());
                        try {
                            client.publish(feedsTempArquario, message);
                        } catch (MqttException e) {
                            Toast.makeText(MainActivityConfig.this, "FALHOU publicar:" + feedsTempArquario, Toast.LENGTH_LONG).show();
                            e.printStackTrace();
                        }

                        client.disconnect();

                        tmpLinha = "1;" + tmpStringBranco + "\n";
                        tmpLinha += "2;" + tmpStringAzul + "\n";
                        tmpLinha += "3;" + tmpStringAzulRoyal + "\n";
                        tmpLinha += "4;" + tmpStringVioleta + "\n";
                        tmpLinha += "5;" + valorSeekBarTempLuminaria + "\n";
                        tmpLinha += "6;" + valorSeekBarTempAquario + "\n";

                        FileOutputStream tmpArquivo = openFileOutput(ARQUIVO_PROPRIEDADES, MODE_PRIVATE);
                        tmpArquivo.write(tmpLinha.getBytes());
                        tmpArquivo.close();

                        Toast.makeText(MainActivityConfig.this, "Gravando arquivo: " + ARQUIVO_PROPRIEDADES, Toast.LENGTH_SHORT).show();

                    } catch (MqttException | FileNotFoundException e) {
                        Toast.makeText(MainActivityConfig.this, "Conexão falhou com o MQTT", Toast.LENGTH_LONG).show();
                        e.printStackTrace();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }

                }

                @Override
                public void onFailure(IMqttToken asyncActionToken, Throwable exception) {
                    Toast.makeText(MainActivityConfig.this, "Não conectado ao MQTT", Toast.LENGTH_LONG).show();

                }
            });

        } catch (MqttException e) {
            Toast.makeText(MainActivityConfig.this, "Conxão falhou com o MQTT", Toast.LENGTH_LONG).show();
            e.printStackTrace();
        }

    }

}

