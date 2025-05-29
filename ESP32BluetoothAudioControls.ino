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

          // read the state of the switch into a local variable:
      int reading = digitalRead(keys[i].pin);
      // check to see if you just pressed the button
      // (i.e. the input went from LOW to HIGH), and you've waited long enough
      // since the last press to ignore any noise:

      // If the switch changed, due to noise or pressing:
      if (reading != keys[i].debounce.lastButtonState)
      {
        // reset the debouncing timer
        keys[i].debounce.lastDebounceTime = millis();
      }

      if ((millis() - keys[i].debounce.lastDebounceTime) > debounceDelay)
      {
        // whatever the reading is at, it's been there for longer than the debounce
        // delay, so take it as the actual current state:

        // if the button state has changed:
        if (reading != keys[i].debounce.buttonState)
        {
          keys[i].debounce.buttonState = reading;

          // only toggle the LED if the new button state is HIGH
          if (keys[i].debounce.buttonState == LOW)
          {
            bleKeyboard.write(*keys[i].key);
          }
        }
      }


    // save the reading. Next time through the loop, it'll be the lastButtonState:
      keys[i].debounce.lastButtonState = reading;
    }
  }
}