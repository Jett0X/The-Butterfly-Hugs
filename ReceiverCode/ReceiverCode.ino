#include <esp_now.h>
#include <WiFi.h>

bool isOn = false;

const int intakeRelayPin = 27;
const int outtakeRelayPin = 26;
const int intakeDelay = 1000;
const int outtakeDelay = 1000;
const int intakeInitializationDelay = 2000;
const int outtakeInitializationDelay = 2000;

//Structure example to receive data
//Must match the sender structure
typedef struct test_struct {
  bool isOn;
} test_struct;

//Create a struct_message called myData
test_struct myData;

//callback function that will be executed when data is received
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  isOn = myData.isOn;
}



void setup() {
  //Initialize Serial Monitor
  Serial.begin(115200);

  //Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  //Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

  pinMode(intakeRelayPin, OUTPUT);
  pinMode(outtakeRelayPin, OUTPUT);

  digitalWrite(intakeRelayPin, HIGH);
  digitalWrite(outtakeRelayPin, HIGH);
}

void loop() {

  if (isOn) {
    // Initialize intake
    Serial.println("Initializing Butterfly Hug");
    intake();
    delay(intakeInitializationDelay);
    intakeStop();

    while (true) {
      // outtake
      Serial.println("Outtaking...");
      outtake();
      delay(outtakeDelay);
      outtakeStop();

      // intake
      Serial.println("Intaking...");
      intake();
      delay(intakeDelay);
      intakeStop();

      if (!isOn) {

        // outtake
        Serial.println("Initializing Stop Butterfly Hug");
        outtake();
        delay(outtakeInitializationDelay);

        digitalWrite(intakeRelayPin, HIGH);

        break;
      }
    }
  }

  // Butterfly Loop
}

void outtake(){

  digitalWrite(outtakeRelayPin, HIGH);
  digitalWrite(intakeRelayPin, LOW);

}

void outtakeStop(){

  digitalWrite(outtakeRelayPin, LOW);

}

void intake(){

  digitalWrite(outtakeRelayPin, LOW);
  digitalWrite(intakeRelayPin, HIGH);

}

void intakeStop(){

  digitalWrite(intakeRelayPin, LOW);

}

