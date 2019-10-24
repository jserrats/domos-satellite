#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RCSwitch.h>
#include <Ethernet2.h>
#include <BME280I2C.h>
#include <MQTT.h>


BME280I2C bme;
LiquidCrystal_I2C lcd(0x27  , 20, 4);
RCSwitch mySwitch = RCSwitch();

EthernetClient net;
MQTTClient client;

unsigned long lastMillis = 0;

byte mac[] = {
  0x7e, 0x1e, 0xc0, 0xba, 0x55, 0x01
};

const char* codes[3][2] {
  {"0FF0FFFF0101", "0FF0FFFF0110"},
  {"0FF0FFFF1001", "0FF0FFFF1010"},
  {"0FF0FFF10001", "0FF0FFF10010"}
};

byte button_pins[] =  {
  40, 32, 41, 33, 25, 38, 30, 39, 31, 23, 36, 28, 37, 29, 24, 34, 26, 35, 27, 22
};
void connect() {
  while (!client.connect("arduino", "try", "try")) {
    delay(1000);
  }
  client.subscribe("domos/arduino/#");
}

void setup() {
  Serial.begin(9600);
  Serial.println("setup");


  // buttons
  for (int i = 0; i < sizeof(button_pins); i++) {
    pinMode(button_pins[i], INPUT_PULLUP);
  }

  // ethernet
  if (Ethernet.begin(mac) == 0) {// CS pin = 10
    Serial.println("Failed to configure Ethernet using DHCP");
  }

  // lcd
  lcd.init();
  lcd.backlight();
  lcd.clear();

  // radio
  mySwitch.enableTransmit(3);
  mySwitch.setPulseLength(177);

  // temp sensor
  while (!bme.begin()) {}

  //octocoupler
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);

  // mqtt
  client.begin("10.0.1.1", net);
  client.onMessage(messageReceived);
  Serial.println("setup complete");
}
void messageReceived(String &topic, String &payload) {

  // LCD
  if (topic.endsWith("lcd/0")) {
    lcd.setCursor(0, 0);
    lcd.print(payload);
  }
  else if (topic.endsWith("lcd/1")) {
    lcd.setCursor(0, 1);
    lcd.print(payload);
  }
  else if (topic.endsWith("lcd/2")) {
    lcd.setCursor(0, 2);
    lcd.print(payload);
  }
  else if (topic.endsWith("lcd/3")) {
    lcd.setCursor(0, 3);
    lcd.print(payload);
  }
  else if (topic.endsWith("lcd/bl")) {
    if (payload == "y") {
      lcd.backlight();
    } else {
      lcd.noBacklight();
    }
  }


  // radio
  else if (topic.endsWith("power/0")) {
    if (payload == "on") {
      power_action(0, true);
    } else {
      power_action(0, false);
    }
  }
  else if (topic.endsWith("power/1")) {
    if (payload == "on") {
      power_action(1, true);
    } else {
      power_action(1, false);
    }
  }

  // octocoupler
  else if (topic.endsWith("octocoupler/0")) {
    digitalWrite(4, HIGH);
    delay(1000);
    digitalWrite(4, LOW);
  }
  else if (topic.endsWith("octocoupler/1")) {
    digitalWrite(5, HIGH);
    delay(1000);
    digitalWrite(5, LOW);
  }
}

void loop() {
  if (!client.connected()) {
    connect();
  }
  client.loop();

  check_buttons();

  // do every 5min
  if (millis() - lastMillis > 300000) {
    lastMillis = millis();
    send_meteo();
  }
}

void check_buttons() {
  for (int i = 0; i < sizeof(button_pins); i++) {
    if (!digitalRead(button_pins[i])) {
      publish_button(i);
      delay(400);
    }
  }
}

void publish_button(int button) {
  String topic = "domos/button";
  client.publish(topic, String(button));
}

void send_meteo() {
  float temp(NAN), hum(NAN), pres(NAN);

  BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
  BME280::PresUnit presUnit(BME280::PresUnit_hPa);

  bme.read(pres, temp, hum, tempUnit, presUnit);

  char b[5];
  dtostrf(temp, 5, 2, b);
  client.publish("domos/info/meteo/temperature", b, true, 0);


  dtostrf(hum, 5, 2, b);
  client.publish("domos/info/meteo/humidity", b, true, 0);

  char a[8];
  dtostrf(pres, 8, 2, b);
  client.publish("domos/info/meteo/pressure", b, true, 0);
}

void power_action(byte switch_id, boolean power) {
  int onoff;
  if (power) {
    onoff = 0;
  }
  else {
    onoff = 1;
  }
  mySwitch.sendTriState(codes[switch_id][onoff]);
}
