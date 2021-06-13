
/*
  LoRa Duplex communication with Spreading Factor

  Sends a message every half second, and polls continually
  for new incoming messages. Sets the LoRa radio's spreading factor.

  Spreading factor affects how far apart the radio's transmissions
  are, across the available bandwidth. Radios with different spreading
  factors will not receive each other's transmissions. This is one way you
  can filter out radios you want to ignore, without making an addressing scheme.

  Spreading factor affects reliability of transmission at high rates, however,
  so avoid a hugh spreading factor when you're sending continually.

  See the Semtech datasheet, http://www.semtech.com/images/datasheet/sx1276.pdf
  for more on Spreading Factor.

  created 28 April 2017
  by Tom Igoe
*/
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <SPI.h>              // include libraries
#include <LoRa.h>
//#include <GyverWDT.h>        //Зависимость таймаутов от делителей см. в GyverWDT.h
//#include <avr/sleep.h>
#include <GyverPower.h>


const int pin_water = 4;
const int csPin = 7;          // LoRa radio chip select
const int resetPin = 9;       // LoRa radio reset
const int irqPin = 2;         // change for your board; must be a hardware interrupt pin

String water;
byte msgCount = 0;            // count of outgoing messages
int interval = 2000;          // interval between sends
long lastSendTime = 0;        // time of last packet send

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme;

void setup() {
  power.autoCalibrate();
  Serial.begin(9600);                   // initialize serial
  while (!Serial);

  Serial.println("LoRa Duplex - Set spreading factor");

  // override the default CS, reset, and IRQ pins (optional)
  LoRa.setPins(csPin, resetPin, irqPin); // set CS, reset, IRQ pin

  if (!LoRa.begin(868E6)) {             // initialize ratio at 868 MHz
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }

  LoRa.setSpreadingFactor(12);           // ranges from 6-12,default 7 see API docs
  LoRa.setSignalBandwidth(125000);
  LoRa.setCodingRate4(5);
 // LoRa.enableCrc();
  Serial.println("LoRa init succeeded.");

  unsigned status;
  status = bme.begin(0x76);
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
        Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(),16);
        Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
        Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
        Serial.print("        ID of 0x60 represents a BME 280.\n");
        Serial.print("        ID of 0x61 represents a BME 680.\n");
        while (1) delay(10);
    }

  
}

void loop() {
  //  if (millis() - lastSendTime > interval) { откр                       //убираем уcловие таймера в пользу WDT
    String message = "Data:  ";   // send a message
    String temp = ("Temp:  " + String(bme.readTemperature()) + "  ");
    String hum = ("Humidity:  " + String(bme.readHumidity())+ "  ");
    String pres = ("Pressure:  " + String(bme.readPressure() / 100.0F));
    get_water_sens();
    String wat = ("Water sensor:  " + water);
//    memcpy(&message[7], &temp, sizeof(float));
    message += (temp + hum + pres + wat);
    sendMessage(message);
    Serial.println("Sending:" + message );
    delay(50);
    // lastSendTime = millis();            // timestamp the message
    // msgCount++;  зачем 2 раза прибоавлять еденичку? убираем это
    //           interval =  9000;    // default 2-3 seconds
    
     
     

    
 // закр   }


  // parse for a packet, and call onReceive with the result:
  onReceive(LoRa.parsePacket());
  delay (50);
  power.sleepDelay(5000);
  /*Watchdog.enable(INTERRUPT_MODE, WDT_PRESCALER_1024);// Режим прерываний , таймаут ~8 c
     sleep_enable();                                     // Разрешаем сон
     sleep_cpu();                                        // Уходим в сон
*/
}


void get_water_sens() {
  int buttonState = digitalRead(pin_water);
  if (buttonState == 1) {
      water = ("0");
  }
  else {water = ("1");}
  return water;
}

void sendMessage(String outgoing) {
  //outgoing += "{"+'"temp":'+String(20.1)+"}";
  LoRa.beginPacket();                   // start packet
  LoRa.print(outgoing);                 // add payload
  LoRa.endPacket();                     // finish packet and send it
  msgCount++;                           // increment message ID

  }


void onReceive(int packetSize) {
    if (packetSize == 0) {
    
    return; // if there's no packet, return
    }
  

  // read packet header bytes:
  String incoming = "";

  while (LoRa.available()) {
    incoming += (char)LoRa.read();
  }

  Serial.println("Message: " + incoming);
  Serial.println("RSSI: " + String(LoRa.packetRssi()));
  Serial.println("Snr: " + String(LoRa.packetSnr()));
  Serial.println();
 
}
/*
ISR(WATCHDOG) {        // прерывание WD, в котором мы просыпаемся
  
  sleep_disable();     // Запрещаем сон
  Watchdog.disable();  // Выключаем watchdog
}
*/
