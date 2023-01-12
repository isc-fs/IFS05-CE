# 1 "c:\\Users\\moran\\OneDrive - Universidad Pontificia Comillas\\UNI\\ICAI\\ISC RACING TEAM\\00_2022-2023\\03. Tests octubre 2022\\master_ecu\\master_ecu.ino"
# 2 "c:\\Users\\moran\\OneDrive - Universidad Pontificia Comillas\\UNI\\ICAI\\ISC RACING TEAM\\00_2022-2023\\03. Tests octubre 2022\\master_ecu\\master_ecu.ino" 2
# 3 "c:\\Users\\moran\\OneDrive - Universidad Pontificia Comillas\\UNI\\ICAI\\ISC RACING TEAM\\00_2022-2023\\03. Tests octubre 2022\\master_ecu\\master_ecu.ino" 2
# 4 "c:\\Users\\moran\\OneDrive - Universidad Pontificia Comillas\\UNI\\ICAI\\ISC RACING TEAM\\00_2022-2023\\03. Tests octubre 2022\\master_ecu\\master_ecu.ino" 2
//#include <TimerOne.h>



// ---------- VARIABLES DEL CAN ----------
MCP_CAN CAN_TEL(44); // CAN Telemetría
MCP_CAN CAN_INV(4); // CAN Inversor

byte ext = 0; // CAN_EXTID = 0：Standard data frame, based on CAN 2.0A standard. ID range: 0 ~ 0x7FF
byte len; // DLC: número de bytes que se transmiten en cada mensaje.

byte rxBuf[8] = {0,0,0,0,0,0,0,0}; // Único buffer para recibir
byte txBuf_tel[8] = {0,0,0,0,0,0,0,0}; // Buffer para transmitir por el CAN_TEL
byte txBuf_inv[8] = {0,0,0,0,0,0,0,0}; // Buffer para transmitir por el CAN_INV

unsigned long rxID; // ID para recibir (único para todas las recepciones)

// Datos a recibir del inversor
unsigned long datos_inversor[5] = {0x49 /* Motor temperature*/,0x4A /* Power stage temperature*/,0x4B /* Air temperature*/,0x30 /* Speed actual value*/,0X5F /* Filtered actual current*/};
//INT32U datos_inversor[N_DATOS_INV] = {N_ACTUAL};

// Detector de flanco botón de arranque
int start_button_act;
int start_button_ant = 1;

// ---------- VARIABLES DE LECTURA DE SENSORES ----------
// Inversor
int inv_dc_bus_voltage; // Lectura de DC_BUS_VOLTAGE
int inv_t_motor; // Lectura de motor temperature
int inv_t_igbt; // Lectura de power stage temperature
int inv_t_air; // Lectura de air temperature
int inv_n_actual; // Lectura de speed actual value
// Coche
int s1_aceleracion; // Lectura del sensor 1 del pedal de aceleración
int s2_aceleracion; // Lectura del sensor 2 del pedal de aceleración
float s1_aceleracion_aux;
float s2_aceleracion_aux;
int s_freno; // Lectura del sensor de freno
float s_freno_aux;
int sdd_suspension; // Lectura del sensor 1 de suspensión
int sdi_suspension; // Lectura del sensor 2 de suspensión
float aux_velocidad;
float v_celda_min=3600; //Contiene el ultimo valor de tension minima de una celda
                 //enviada por el AMS.

// ---------- VARIABLES DE CONTROL DEL INVERSOR ----------
int porcentaje_pedal_acel;
int torque_1;
int torque_2;
int torque_total;
int torque_limitado;
int media_s_acel;
int flag_EV_2_3=0;
int flag_T11_8_9=0;
int count_T11_8_9=0;



// ---------- VARIABLES DE CONTROL DEL TIEMPO ----------
unsigned long current_time; // Guarda el valor actual de millis()
unsigned long previous_time_inv = 0;
unsigned long previous_time_tel = 0;
int count_accel =0 ;


void setup()
{
 Serial.begin(115200);

  // Configuración de pines
  pinMode(A0,0x0);
  pinMode(A3,0x0);

  pinMode(A4,0x0);

  pinMode(A6,0x0);
  pinMode(A8,0x0);

  pinMode(46,0x0);
  pinMode(5,0x0);

  pinMode(31, 0x1);
  pinMode(14,0x0);
  digitalWrite(31,0x0);

  //Inicialización del CAN de Telemetría

   if (CAN_TEL.begin(3 /* Disables Masks and Filters   */,12,2) == (0)) {
    Serial.println("CAN_TEL: Inicializacion correcta!");
    CAN_TEL.setMode(0x00);
  } else {
    Serial.println("CAN_TEL: Fallo al incializar, reinicie por favor");
    while(1);
  }

  //Inicialización del CAN del Inversor
  if (CAN_INV.begin(3 /* Disables Masks and Filters   */,13,2) == (0)) {
    Serial.println("CAN_INV: Inicializacion correcta!");
    CAN_INV.setMode(0x00);
  } else {
    Serial.println("CAN_INV: Fallo al incializar, reinicie por favor");
    while(1);
  }

  SPI.setClockDivider(0x04); // Set SPI to run at 8MHz (16MHz / 2 = 8 MHz)


  // ---------- SECUENCIA DE ARRANQUE ----------
  // Configuración para recibir el registro DC_BUS_VOLTAGE del inversor
  while (config_inv_lectura_v == 0) {
    len = 3;
    txBuf_inv[0] = 0x3D /* REGID for reading data from the servo and transmission to the CAN*/;
    txBuf_inv[1] = 0xEB /* DC-Bus voltage*/;
    txBuf_inv[2] = 0x64; // Para que transmita cada 100ms
    CAN_INV.sendMsgBuf(rxID_inversor, ext, len, txBuf_inv);
    delay(100);
    Serial.println("Solicitar tension");

    // Esperamos ACK del inversor
    if (!digitalRead(5)) {
      CAN_INV.readMsgBuf(&rxID, &len, rxBuf);
      if(rxID == txID_inversor && len == 4 && rxBuf[0] == 0xEB /* DC-Bus voltage*/) {
        Serial.println("CAN_INV: Configurado lectura de DC_BUS_VOLTAGE correctamente.");
        config_inv_lectura_v = 1; // Salimos del bucle
      }
    }
  }

Serial.println("Pre-carga");
  // Pre-carga
  while (precarga_inv == 0) {

    // Leemos DC_BUS_VOLTAGE del CAN_INV
    if(!digitalRead(5)) {
      CAN_INV.readMsgBuf(&rxID, &len, rxBuf);
      if(rxID == txID_inversor && len == 4 && rxBuf[0] == 0xEB /* DC-Bus voltage*/) {
        inv_dc_bus_voltage = ((int)rxBuf[2]<<8 | (int)rxBuf[1]) / 55;
      }
      Serial.print("DC_BUS_VOLTAGE (V): ");
      Serial.println(inv_dc_bus_voltage);

      //Reenviamos DC_BUS_VOLTAGE al AMS por CAN_TEL
      len = 2;
      txBuf_inv[0] = inv_dc_bus_voltage >> 8 & 0xFF;
      txBuf_inv[1] = inv_dc_bus_voltage & 0xFF;
      CAN_TEL.sendMsgBuf(ID_dc_bus_voltage, ext, len, txBuf_inv);
      Serial.println("CAN_TEL: DC_BUS_VOLTAGE enviado a AMS");
    }

    // Esperamos a recibir el Ok Pre-Carga del AMS
    if (!digitalRead(46)) {
      CAN_TEL.readMsgBuf(&rxID, &len, rxBuf);
      if (rxID == ID_ack_precarga && len == 1 && rxBuf[0] == 0) {
        precarga_inv = 1; // Precarga lista
        Serial.println("CAN_TEL: Precarga correcta. OK del AMS");
      }
    }
  }


  // Comprobación ECU Telemería ON
  while (ecu_telemetria == 0) {
    len = 1;
    txBuf_tel[0] = 1;
    CAN_TEL.sendMsgBuf(ID_ok_telemetria, ext, len, txBuf_tel);

    if (!digitalRead(46)) {
      CAN_TEL.readMsgBuf(&rxID, &len, rxBuf);
      if (rxID == ID_ack_telemetria && len == 1 && rxBuf[0] == 1) {
        ecu_telemetria = 1; // ECU Telemetría ON
        Serial.println("CAN_TEL: ACK ECU Telemetria");
      }
    }
    delay(100);
  }

  // Comprobación ECU Caja Negra ON
  while (ecu_caja_negra == 0) {
    len = 1;
    txBuf_tel[0] = 1;
    CAN_TEL.sendMsgBuf(ID_ok_caja_negra, ext, len, txBuf_tel);

    if (!digitalRead(46)) {
      CAN_TEL.readMsgBuf(&rxID, &len, rxBuf);
      if (rxID == ID_ack_caja_negra && len == 1 && rxBuf[0] == 1) {
        ecu_caja_negra = 1; // ECU Caja Negra ON
        Serial.println("CAN_TEL: ACK ECU Caja Negra");
      }
    }
    delay(100);
  }

  // Comprobación si el inversor está READY-TO-START (RTS) (RFE y RUN pulsados)
  while (RTS_inv == 0) {
    // Pedimos el registro de RFE y RUN para comprobar si se han pulsado
    len = 3;
    txBuf_inv[0] = 0x3D /* REGID for reading data from the servo and transmission to the CAN*/;
    txBuf_inv[1] = 0xD8 /* RUN and RFE REGID*/;
    txBuf_inv[2] = 0x00; // Time interval (transmitting once)
    CAN_INV.sendMsgBuf(rxID_inversor, ext, len, txBuf_inv);

    if (!digitalRead(5)) {
      CAN_INV.readMsgBuf(&rxID, &len, rxBuf);
      if (rxID == txID_inversor && len == 4 /*COMPROBAR*/ && rxBuf[0] == 0xD8 /* RUN and RFE REGID*/ && (rxBuf[1] & 0x30 /* Si RUN (bit 4) y RFE (bit 5) están enable*/) == 0x30 /* Si RUN (bit 4) y RFE (bit 5) están enable*/) {
        RTS_inv = 1;
        Serial.println("CAN_INV: Inversor RTS");
      }
    }
    delay(100);
  }

  // Comprobar BTB Inversor (preparar para controlar por torque)
  if (BTB_todo == 0) {
    while(BTB_inv_1 == 0) {
      // Transmitting transmission request BTB
      len = 3;
      txBuf_inv[0] = 0x3D /* REGID for reading data from the servo and transmission to the CAN*/;
      txBuf_inv[1] = 0xE2 /* Device ready*/;
      txBuf_inv[2] = 0x00;
      CAN_INV.sendMsgBuf(rxID_inversor, ext, len, txBuf_inv);

      // Receiving BTB 0xE2
      CAN_INV.readMsgBuf(&rxID, &len, rxBuf);
      if (rxID == txID_inversor && len == 4 && rxBuf[0] == 0xE2 /* Device ready*/ && rxBuf[2] == 0x00 && rxBuf[3] == 0x00) {
        if (rxBuf[1] == 0x01) {
          BTB_inv_1 = 1;
          Serial.println("CAN_INV: BTB correcto, listo para arrancar.");
        } else {
          Serial.println("CAN_INV: BTB erróneo. No se puede arrancar.");
        }
      }
      delay(100);
    }

    // Transmitting disable
    len = 3;
    txBuf_inv[0] = 0x51 /* Mode State*/;
    txBuf_inv[1] = 0x04;
    txBuf_inv[2] = 0x00;
    CAN_INV.sendMsgBuf(rxID_inversor, ext, len, txBuf_inv);

    // Transmitting transmission request enable (hardware)
    len = 3;
    txBuf_inv[0] = 0x3D /* REGID for reading data from the servo and transmission to the CAN*/;
    txBuf_inv[1] = 0xE8 /* Digital input RUN*/;
    txBuf_inv[2] = 0x00;
    CAN_INV.sendMsgBuf(rxID_inversor, ext, len, txBuf_inv);

    while(BTB_inv_2 == 0) {
      // Receiving enable 0xE8
      CAN_INV.readMsgBuf(&rxID, &len, rxBuf);
      if (rxID == txID_inversor && len == 4 && rxBuf[0] == 0xE8 /* Digital input RUN*/ && rxBuf[1] == 0x01 && rxBuf[2] == 0x00 && rxBuf[3] == 0x00) {
        BTB_inv_2 = 1;
        Serial.println("CAN_INV: enable 0xE8 recibido correctamente");
      }
      delay(100);
    }

    // Transmitting no disable (enable)
    len = 3;
    txBuf_inv[0] = 0x51 /* Mode State*/;
    txBuf_inv[1] = 0x00;
    txBuf_inv[2] = 0x00;
    CAN_INV.sendMsgBuf(rxID_inversor, ext, len, txBuf_inv);
  }

  // Encender LED del botón de arranque para avisar al piloto que está todo listo

  // Peticiones de envío de datos de telemetría a inversor
  Serial.print("Peticiones inversor");
  for (size_t i = 0; i < 5; i++) {
    len = 3;
    txBuf_inv[0] = 0x3D /* REGID for reading data from the servo and transmission to the CAN*/;
    txBuf_inv[1] = datos_inversor[i];
    txBuf_inv[2] = 0xFA; // Delay: 1 - 254 (0xEF) ms --> 0xFF para parar 
    CAN_INV.sendMsgBuf(rxID_inversor, ext, len, txBuf_inv);
    delay(15);
  }
  Serial.println("RTS");

  // Esperar a que se pulse el botón de arranque mientras se pisa el freno
  while (boton_arranque == 0) {
    start_button_act = digitalRead(14);
    if (start_button_act == 0 && start_button_ant == 1) {
      s_freno = analogRead(A4);
        Serial.print("Freno: ");
        Serial.print(s_freno);
      if (s_freno > 210 /* Valor léido del ADC a partir del cual se considera que el pedal de freno ha sido pulsado*/) {
        boton_arranque = 1;
        Serial.println("Coche arrancado correctamente.");
      } else {
        Serial.println("Pulsar freno para arrancar");
      }
    }
  }

  // Activar READY-TO-DRIVE-SOUND (RTDS) durante un segundo y medio
  Serial.println("RTDS sonando...");
  digitalWrite(31,0x1);
  delay(2000);
  digitalWrite(31,0x0);
  Serial.println("RTDS apagado");

  // Avisar a resto de ECUs de que pueden comenzar ya a mandar datos al CAN (RTD_all)
  len = 1;
  txBuf_tel[0] = 1;
  CAN_TEL.sendMsgBuf(ID_RTD_all, ext, len, txBuf_tel);
  delay(0);
}

void loop()
{
  // ---------- CONTROL DEL INVERSOR ----------
 current_time = millis();
  if (current_time - previous_time_inv >= periodo_inv) {
    // Leemos sensores de posición del pedal de acelaración
    s1_aceleracion = analogRead(A0);
    s2_aceleracion = analogRead(A3);
    Serial.print("Sensor 1: ");
    Serial.print(s1_aceleracion);
    Serial.println("");
    Serial.print("Sensor 2: ");
    Serial.print(s2_aceleracion);
    Serial.println("");

    // Leemos sensor de freno
    s_freno = analogRead(A4);
    Serial.print("Sensor freno: ");
    Serial.println(s_freno);

    // Calculamos % torque  en función de la posición de los sensores
    s1_aceleracion_aux=(s1_aceleracion-294.0)/(10.23-2.94);
    s2_aceleracion_aux=(s2_aceleracion-29)/(7.87-0.29);
    Serial.print("Sensor % 1: ");
    Serial.print(s1_aceleracion_aux);
    Serial.println("");
    Serial.print("Sensor % 2: ");
    Serial.print(s2_aceleracion_aux);
    Serial.println("");




    //Torque enviado es la media de los dos sensores
    torque_total=(s1_aceleracion_aux+s2_aceleracion_aux)/2;

    //Por debajo de un 10% no acelera y por encima de un 90% esta a tope
    if( torque_total<10){
      torque_total=0;
    }else if(torque_total>90){
      torque_total=100;
    }

     //
    if(s1_aceleracion_aux> 7.1 && s2_aceleracion_aux > 12 && s1_aceleracion_aux< 8.6 && s2_aceleracion_aux < 20){
      torque_total=0;
    }

    // Comprobamos EV 2.3 APPS/Brake Pedal Plausibility Check
    // En caso de que se esté pisando el freno y mas de un 25% del pedal para. Se resetea 
    // solo si el acelerador vuelve por debajo del 5%
    if(s_freno > 210 /* Valor léido del ADC a partir del cual se considera que el pedal de freno ha sido pulsado*/ && torque_total > 25) {
      flag_EV_2_3=1;
    }else if(s_freno<210 /* Valor léido del ADC a partir del cual se considera que el pedal de freno ha sido pulsado*/ && torque_total < 5){
      flag_EV_2_3=0;
    }

    // T11.8.9 Implausibility is defined as a deviation of more than ten percentage points 
    // pedal travel between any of the used APPSs
    if(((s1_aceleracion_aux-s2_aceleracion_aux)>0?(s1_aceleracion_aux-s2_aceleracion_aux):-(s1_aceleracion_aux-s2_aceleracion_aux))>10){
      count_T11_8_9=count_T11_8_9+1;
      if( count_T11_8_9*periodo_inv>100){
        flag_T11_8_9=1;
      }
    }else{
      count_T11_8_9=0;
      flag_T11_8_9=0;
    }

    if(flag_EV_2_3 || flag_T11_8_9){
      torque_total = 0;
    }

    Serial.print("Torque total solicitado: ");
    Serial.println(torque_total);

    //Limitación del torque en función de la carga
    if(v_celda_min<3500){
      if(v_celda_min>2800){
        torque_limitado=torque_total*(1.357*v_celda_min-3750)/1000;

      }else{
          torque_limitado=torque_total*0.05;
      }
    }else{
      torque_limitado=torque_total;
    }

        Serial.print("Torque limitado en: ");
        Serial.println(torque_limitado);

    // Enviamos torque
    len = 3;
    txBuf_inv[0] = 0x90;
    txBuf_inv[1] = ((int) (torque_limitado*32767.0/100.0)) & 0xFF; // bits del 0-7
    txBuf_inv[2] = ((int) (torque_limitado*32767.0/100.0))>>8; // bits del 8-15
    CAN_INV.sendMsgBuf(rxID_inversor, ext, len, txBuf_inv);

    // Reseteamos contador de envío de torque
    previous_time_inv = millis();

    // Enviar telemetría aceleración y freno
    if(count_accel>=(periodo_tel/periodo_inv)-1){
      len = 2;
      //txBuf_tel[0] = 0x00;
      txBuf_tel[0] = ((int) torque_limitado) >> 8;
      txBuf_tel[1] = ((int) torque_limitado);
      CAN_TEL.sendMsgBuf(ID_torque_total, ext, len, txBuf_tel);
      Serial.print("Enviando a telemetria torque: ");
      Serial.println(torque_limitado);
      delay(0);
      Serial.print("Enviando freno: ");
      Serial.println(s_freno);

      s_freno_aux=s_freno;
      //s_freno_aux=(s_freno_aux-115.0)/818.0*100.0;

      s_freno_aux=(s_freno_aux-110.0)/572.0*100.0;
      len = 2;
      txBuf_tel[0] = ((int) s_freno_aux)>> 8;
      txBuf_tel[1] = ((int) s_freno_aux);
      CAN_TEL.sendMsgBuf(ID_s_freno, ext, len, txBuf_tel);
      Serial.print("Enviando a telemetria freno: ");
      Serial.println(s_freno_aux);
      Serial.print(txBuf_tel[0]);
      Serial.print(",");
      Serial.println(txBuf_tel[1]);

      delay(0);
      count_accel=0;

    }
    count_accel=count_accel+1;
  }

  // Leemos datos de lectura del inversor y envío al CAN_TEL
  if (!digitalRead(5)) {
    CAN_INV.readMsgBuf(&rxID, &len, rxBuf);

    if(rxID == txID_inversor && len == 4 && rxBuf[0] == 0xEB /* DC-Bus voltage*/) {
      len = 2;
      inv_dc_bus_voltage = ((int)rxBuf[2]<<8 | (int)rxBuf[1]) / 55;
      txBuf_tel[0] = inv_dc_bus_voltage >> 8 & 0xFF;
      txBuf_tel[1] = inv_dc_bus_voltage & 0xFF;
      CAN_TEL.sendMsgBuf(ID_dc_bus_voltage, ext, len, txBuf_tel);
      delay(0);
    } else if(rxID == txID_inversor && len == 4 && rxBuf[0] == 0x49 /* Motor temperature*/) {
      len = 2;
      txBuf_tel[0] = rxBuf[2];
      txBuf_tel[1] = rxBuf[1];
      CAN_TEL.sendMsgBuf(ID_t_motor, ext, len, txBuf_tel);
      delay(0);
    } else if(rxID == txID_inversor && len == 4 && rxBuf[0] == 0x4A /* Power stage temperature*/) {
      len = 2;
      txBuf_tel[0] = rxBuf[2];
      txBuf_tel[1] = rxBuf[1];
      CAN_TEL.sendMsgBuf(ID_t_igbt, ext, len, txBuf_tel);
      delay(0);
    } else if(rxID == txID_inversor && len == 4 && rxBuf[0] == 0x4B /* Air temperature*/) {
      len = 2;
      txBuf_tel[0] = rxBuf[2];
      txBuf_tel[1] = rxBuf[1];
      CAN_TEL.sendMsgBuf(ID_t_air, ext, len, txBuf_tel);
      delay(0);
    } else if(rxID == txID_inversor && len == 4 && rxBuf[0] == 0x30 /* Speed actual value*/) {
      len = 2;
      txBuf_tel[0] = rxBuf[2];
      txBuf_tel[1] = rxBuf[1];
      CAN_TEL.sendMsgBuf(ID_n_actual, ext, len, txBuf_tel);
      delay(0);

      len = 2;
      txBuf_tel[0] = rxBuf[2];
      txBuf_tel[1] = rxBuf[1];
      aux_velocidad= (txBuf_tel[0] << 8)|txBuf_tel[1];
      aux_velocidad=aux_velocidad*0.02185455759;
      txBuf_tel[0] =(int) aux_velocidad>>8;
      txBuf_tel[1] =(int) aux_velocidad;
      CAN_TEL.sendMsgBuf(ID_velocidad, ext, len, txBuf_tel);
      delay(0);
    }else if(rxID == txID_inversor && len == 4 && rxBuf[0] == 0X5F /* Filtered actual current*/) { //Corriente inversor
      len = 2;
      txBuf_tel[0] = rxBuf[2];
      txBuf_tel[1] = rxBuf[1];
      CAN_TEL.sendMsgBuf(0x305, ext, len, txBuf_tel);
      delay(0);
    }
  }

  current_time = millis();
  // ---------- RECOGIDA Y ENVÍO DE DATOS DE TELEMETRÍA ----------
  if (current_time - previous_time_tel >= periodo_tel) {

  }

   if (!digitalRead(46)){
   CAN_TEL.readMsgBuf(&rxID, &len, rxBuf);

     if (rxID == ID_v_celda_min && len == 2) {
        v_celda_min = ((int)rxBuf[0]<<8 | (int)rxBuf[1]);
        Serial.print("Tensión minima de la celdas: ");
        Serial.println(v_celda_min);
      }
   }
}
