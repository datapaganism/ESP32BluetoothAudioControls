#include <BleKeyboard.h>

// For Nologo ESP32C3 Super Mini

typedef struct debounce
{
  int buttonState;
  int lastButtonState;
  unsigned long lastDebounceTime;  
} debounce_t;

debounce_t default_debounce = { LOW, LOW, 0};
unsigned long debounceDelay = 50;

typedef struct pin_to_key
{
  const int pin;
  debounce_t debounce;
  const MediaKeyReport* key;
  
} pin_to_key_t;

pin_to_key_t keys[] = { 
  {5,  default_debounce, &KEY_MEDIA_PLAY_PAUSE },
  {6,  default_debounce, &KEY_MEDIA_PREVIOUS_TRACK },
  {7,  default_debounce, &KEY_MEDIA_NEXT_TRACK },
  {8,  default_debounce, &KEY_MEDIA_VOLUME_DOWN },
  {9,  default_debounce, &KEY_MEDIA_VOLUME_UP },
  {10, default_debounce, &KEY_MEDIA_CONSUMER_CONTROL_CONFIGURATION },
};

BleKeyboard bleKeyboard("Aleno Media Buttons", "Alenas", 100);

void setup() {


  for (int i = 0; i < (sizeof(keys) / sizeof(keys[0])); i++)
  {
    pinMode(keys[i].pin, INPUT_PULLUP);
  }

  Serial.begin(115200);
  Serial.println("Starting BLE work!");
  bleKeyboard.begin();
}




void loop()
{
  if(bleKeyboard.isConnected())
  {
    for (int i = 0; i < (sizeof(keys) / sizeof(keys[0])); i++)
    {

      int reading = digitalRead(keys[i].pin);
      if (reading != keys[i].debounce.lastButtonState)
      {
        keys[i].debounce.lastDebounceTime = millis();
      }

      if ((millis() - keys[i].debounce.lastDebounceTime) > debounceDelay)
      {
        // if the button state has changed:
        if (reading != keys[i].debounce.buttonState)
        {
          keys[i].debounce.buttonState = reading;

          if (keys[i].debounce.buttonState == LOW)
          {
            bleKeyboard.write(*keys[i].key);
          }
        }
      }

      keys[i].debounce.lastButtonState = reading;
    }
  }
}