#include <BleKeyboard.h>

// Requires https://github.com/T-vK/ESP32-BLE-Keyboard library 0.3.2
// Requires esp32 arduino bsp 3.0.2


// For Nologo ESP32C3 Super Mini

typedef struct debounce
{
  int button_state;
  int last_button_state;
  unsigned long last_debounce_time;  
} debounce_t;

debounce_t dd = { LOW, LOW, 0};
unsigned long debounce_delay = 50;

typedef struct pin_to_key
{
  const int pin;
  debounce_t debounce;
  const MediaKeyReport* key;
  
} pin_to_key_t;

pin_to_key_t keys[] = { 
  {5,  dd, &KEY_MEDIA_PLAY_PAUSE },
  {6,  dd, &KEY_MEDIA_PREVIOUS_TRACK },
  {7,  dd, &KEY_MEDIA_NEXT_TRACK },
  {8,  dd, &KEY_MEDIA_VOLUME_DOWN },
  {9,  dd, &KEY_MEDIA_VOLUME_UP },
  {10, dd, &KEY_MEDIA_CONSUMER_CONTROL_CONFIGURATION }, // Should open the default set music app
};

BleKeyboard bleKeyboard("Aleno Media Buttons", "datapaganism", 100);


void setup()
{

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
      if (reading != keys[i].debounce.last_button_state)
      {
        keys[i].debounce.last_debounce_time = millis();
      }

      if ((millis() - keys[i].debounce.last_debounce_time) > debounce_delay)
      {
        // if the button state has changed:
        if (reading != keys[i].debounce.button_state)
        {
          keys[i].debounce.button_state = reading;

          // HIGH = on release LOW = on press
          if (keys[i].debounce.button_state == HIGH)
          {
            bleKeyboard.write(*keys[i].key);
          }
        }
      }

      keys[i].debounce.last_button_state = reading;
    }
  }
}