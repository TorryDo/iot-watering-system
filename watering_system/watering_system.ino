#define BLYNK_TEMPLATE_ID "TMPL6cRGnaGuY"
#define BLYNK_TEMPLATE_NAME "plant watering"
#define BLYNK_AUTH_TOKEN "7ipFMtKuX0ghOOHdU0N_rBTY9vNzwq7o"

char ssid[] = "Android Xr";
char password[] = "1234567899";


#include <DHT.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>



// ----------------------------- the pin ports of the esp8266 module ---------------------------------------------
const int pin_dht = D1;
const int pin_relay = D2;
const int pin_button = D5;
const int pin_led = D8;


// -----------------------------the virtual ports of the blynk service --------------------------------------------
const int blynk_vpin_pump = V2;
const int blynk_vpin_led = V8;


// ------------------------------------------- sensors ------------------------------------------------------------
const int dht_type = DHT11;
DHT dht(pin_dht, dht_type);


// -------------------------------------------- states ------------------------------------------------------------
int threshold_humidity = 40;
int soil_moisture = -1;
int humidity = -1;
int temperature = -1;

boolean isButtonActivated = 1;
boolean isAuto = 1;

unsigned long lastTime = millis();
int count = 0;



void setup() {

  Serial.begin(9600);

  pinMode(pin_led, OUTPUT);
  pinMode(pin_button, INPUT_PULLUP);
  pinMode(pin_relay, OUTPUT);

  digitalWrite(pin_relay, LOW);

  dht.begin();

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);

  Blynk.syncVirtual(V30);
  Blynk.syncVirtual(V31);
  Blynk.syncVirtual(V2);

  while(count < 3){
    digitalWrite(pin_led, HIGH);
    delay(500);
    digitalWrite(pin_led, LOW);
    delay(500);
    count++;
  }
  
}

/*
* toggle main led status
*
* @param state: true or false
* @param setState: default true, push the state to the blynk or not
*/
void setMainLed(int state, bool setState = true){
  if(state == HIGH){
    digitalWrite(pin_led, HIGH);
  }else{
    digitalWrite(pin_led, LOW);
  }
}

/*
* toggle pump status
*
* @param state: true or false
* @param setState: default true, push the state to the blynk or not
*/
void setPump(int state, bool setState = true){
  if(state == HIGH){
    digitalWrite(pin_relay, HIGH);
    if(setState)
      Blynk.virtualWrite(blynk_vpin_pump, 1);
  }else{
    digitalWrite(pin_relay, LOW);
    if(setState)
      Blynk.virtualWrite(blynk_vpin_pump, 0);
  }
}

int isPumpRunning(){
  return digitalRead(pin_relay);
}

/*
* listen humidity and temperature via sensor
* then write to the global variables
*/
void handleHdt(){
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if(h != humidity){
      Blynk.virtualWrite(V1, h);
    }
    if(t != temperature){
      Blynk.virtualWrite(V3, t);
    }

    humidity = h;
    temperature = t;
    
    Serial.print("Nhiet do: " + String(t));
    Serial.print(" <> ");
    Serial.print("Do am: " + String(h));
}

/*
* listen soil moisture value via the sensor
* then write to the global variable
*/
void handleSoilMoisture(){
    int tempe = analogRead(A0);
    tempe = map(tempe, 0, 1023, 100, 0);
    if(tempe != soil_moisture){
      Blynk.virtualWrite(V0, tempe);
    }
    soil_moisture = tempe;
    
    Serial.println(" <> Độ ẩm đất: " + String(soil_moisture));
}

/*
* listen button click event 
* if the button is clicked, reverse pump state 
*/
void handleClickButton(){
  if(digitalRead(pin_button) == LOW){
    isButtonActivated = !isButtonActivated;
    setPump(!digitalRead(pin_relay));
    Serial.println("pump state: " + String(isButtonActivated));
  }
}

void loop() {
  Blynk.run();
  unsigned long currentMillis = millis();
  
  handleClickButton();

  if(currentMillis - lastTime > 1500){

    Serial.print("-----------------\n");

    handleHdt();
    handleSoilMoisture();
    Serial.println("threshold humidity = " + String(threshold_humidity) + " <> auto = " + String(isAuto));

    if(isAuto){

        if(soil_moisture < threshold_humidity){
          setPump(HIGH);
        }else{
          setPump(LOW);
        }
      }

      setMainLed(digitalRead(pin_relay));

      lastTime = millis();
  }
  
}

// ------------------------------------- listen data from blynk ---------------------------------------------------

/*
* set humidity threshold, 
* eg: if the humidity threshold = 50, then the pump will be turned on if the environment humidity smaller than 50%
*/
BLYNK_WRITE(V30){
  int value = param.asInt();

  Serial.print("updateThresholdHumidity: " + String(value));

  threshold_humidity = value;
}

/*
* set auto pump state
*/
BLYNK_WRITE(V31){
  int value = param.asInt();
  if(value == 0){
    isAuto = 0;
  }else{
    isAuto = 1;
  }
}

/*
* turn on or off the button
* turn off auto pump if this function is called
*/
BLYNK_WRITE(V2){
  int value = param.asInt();
  Blynk.virtualWrite(V31, 0);
  isAuto = 0;
  if(value == 0) {
    setPump(LOW);
  }else{
    setPump(HIGH);
  }
}
