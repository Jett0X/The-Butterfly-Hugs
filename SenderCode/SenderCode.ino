#include <esp_now.h>
#include <WiFi.h>
#include <Bounce2.h>

const int buttonPin = 23;
const int ledPin = 27;

Bounce debouncer = Bounce();
bool ledState = false;

// REPLACE WITH YOUR ESP RECEIVER'S MAC ADDRESS
uint8_t broadcastAddress1[] = { 0xd8, 0x13, 0x2a, 0x73, 0x79, 0x48 };

typedef struct test_struct {
  bool isOn;
} test_struct;

test_struct test;

esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  // Copies the sender mac address to a string
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
}

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  // register peer
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  // register first peer
  memcpy(peerInfo.peer_addr, broadcastAddress1, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  pinMode(buttonPin, INPUT_PULLUP);  // Set the button pin as input with internal pull-up resistor
  pinMode(ledPin, OUTPUT);           // Set the built-in LED pin as output

  debouncer.attach(buttonPin);  // Attach debouncer to the button pin
  debouncer.interval(5);
}

void loop() {

  debouncer.update();  // Update the debouncer state

  if (debouncer.fell()) {            // Check if the button was pressed (fell is true when button goes from HIGH to LOW)
    ledState = !ledState;            // Toggle the LED state
    digitalWrite(ledPin, ledState);  // Update the built-in LED
    Serial.println(ledState == true ? "The system is working." : "The system is not working.");
  }

  if (ledState) {
    test.isOn = true;
  } else {
    test.isOn = false;
  }


  esp_err_t result = esp_now_send(broadcastAddress1, (uint8_t *)&test, sizeof(test_struct));
}