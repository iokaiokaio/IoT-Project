#include <Servo.h>
#include <WiFiS3.h>
#include <ArduinoHttpClient.h>
#include <C:\Users\Ioannis\Downloads\IoT Project\secrets.h>


// Servo object
Servo myservo;

const char WIFI_SSID[] = "ICTlab1";
const char WIFI_PASSWORD[] = "ICT-lab-education";
  // const char WIFI_SSID[] = SECRET_SSID;
  // const char WIFI_PASSWORD[] = SECRET_PASS;

const char SERVER[] =  "192.168.40.67";    //"10.154.133.253";
const int SERVER_PORT = 5000;

IPAddress targetIP(192,168,40,67);

WiFiClient wifiClient;
HttpClient httpClient(wifiClient, SERVER, SERVER_PORT);
enum State {
  LOCKED,
  UNLOCKED
};

const unsigned long DEBOUNCE_MS = 50;

bool lastButtonReading = LOW;
bool stableButtonState = LOW;
unsigned long lastDebounceTime = 0;


void setup() {
  // put your setup code here, to run once:
  pinMode(2, INPUT_PULLUP);
  Serial.begin(9600);
  myservo.attach(9);
  connectWiFi();
}
State shouldState;
State isState;
void loop() {
  Serial.println(WiFi.ping(targetIP));
  bool wifiOK = (WiFi.ping(targetIP) > 0);
  
  if (wifiOK) {

    shouldState = getStateFromServer(); 
  }
  
  if (isState != shouldState) {
    Serial.println("Change isState");
    changestate();
  }
  
  if (buttonPressed()){
    Serial.println("Button pressed");
    shouldState = (shouldState == LOCKED) ? UNLOCKED : LOCKED;
    
    if (wifiOK) {
      toggleStateOnServer();
    }
  }
}


bool changestate(){
  if (shouldState == UNLOCKED){
    myservo.write(20);
  } else {
    myservo.write(150);
  }
  isState = shouldState;
}
void toggleStateOnServer() {
  httpClient.post("/api/toggle", "application/json", "{}");
}


void connectWiFi() {
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  for (int i = 0; i < 10; i++) {
  delay(500);
  Serial.print(".");
  }
  }
  Serial.println();
  Serial.println("WiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

State getStateFromServer(){
  httpClient.get("/api/state");
  String response = httpClient.responseBody();
  response.trim();
  if (response == "{\"servoOn\":true}") {
    return UNLOCKED;
  } else if (response == "{\"servoOn\":false}") {
    return LOCKED;
  }
}

bool buttonPressed() {
  bool reading = digitalRead(2);

  if (reading != lastButtonReading) {
    lastDebounceTime = millis();
    lastButtonReading = reading;
  }

  if (millis() - lastDebounceTime >= DEBOUNCE_MS &&
      reading != stableButtonState) {
    stableButtonState = reading;

    // INPUT_PULLUP: gedrückt bedeutet LOW
    if (stableButtonState == HIGH) {
      return true;
    }
  }

  return false;
}

