
// Definitions are in: C:\Program Files (x86)\Arduino\hardware\arduino\avr\boards.txt
// Arduino Leonardo
// VID: 0x2341
// PID: 0x8036

// Libraries
#include <EEPROM.h>
#include <MIDIUSB.h>
//#include <MIDI.h>
//MIDI_CREATE_DEFAULT_INSTANCE();

// Some definitions
#define PIN_Fader     A0
#define PIN_Motor_A   5
#define PIN_Motor_B   6
#define DEADBAND      8
#define FADER_TIMEOUT_MS  500

// Global variables
int prev_adc = -1;
int fader_value;
int faderCanSend = 0;
unsigned long fader_timer;

// Called to generate the MIDI CC message
void SendMidiCC(int channel, int num, int value)
{
  midiEventPacket_t CC = {0x0B, 0xB0 | channel, num, value};
  MidiUSB.sendMIDI(CC);
  MidiUSB.flush();

  // Midi lib wants channels 1~16
  //MIDI.sendControlChange(num, value, channel+1);
}

// Called to read the fader value
void ReadFader()
{
  int value = analogRead(PIN_Fader);

  // Get difference from current and previous value
  int diff = abs(value - prev_adc);

  // Exit this function if the new value is not within the deadband
  if (diff <= DEADBAND) return;

  // Store new value
  prev_adc = value;

  // Get the 7 bit value
  fader_value = value >> 3;

  // Send Midi
  if (faderCanSend)
  {
    Serial.print("MIDI Send value: "); Serial.println(fader_value);
    SendMidiCC(0, 7, fader_value);
  }
}

// Called to move fader
void MoveFader(int dest) // dest is 10 bit value
{
  // Don't send MIDI values while moving the fader
  faderCanSend = 0;

  // Move down
  if (dest < analogRead(PIN_Fader))
  {
    // Set motor pins
    digitalWrite(PIN_Motor_A, 0);
    digitalWrite(PIN_Motor_B, 1);
    // Wait while fader reaches destination
    while (analogRead(PIN_Fader) > dest) {}
  }
  // Move up
  else
  {
    // Set motor pins
    digitalWrite(PIN_Motor_A, 1);
    digitalWrite(PIN_Motor_B, 0);
    // Wait while fader reaches destination
    while (analogRead(PIN_Fader) < dest) {}
  }

  // Stop motor
  digitalWrite(PIN_Motor_A, 0);
  digitalWrite(PIN_Motor_B, 0);

  // Lock midi for a short while after fader stops
  fader_timer = millis();
}

void setup()
{
  pinMode(PIN_Motor_A, OUTPUT);
  pinMode(PIN_Motor_B, OUTPUT);

  Serial.begin(115200);

  // Setup MIDI
  //MIDI.begin(MIDI_CHANNEL_OMNI);
  //MIDI.turnThruOff();
}

void loop()
{
  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // Unlock MIDI?
  if (millis() - fader_timer > FADER_TIMEOUT_MS)
  {
    faderCanSend = 1;
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // Read fader
  ReadFader();

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // Read incoming MIDI messages from USB-MIDI port
  midiEventPacket_t rx;

  do {
    rx = MidiUSB.read();
    if (rx.header != 0)
    {
      int Status  = rx.byte1  & 0xf0; // Event type
      int Channel = rx.byte1  & 0x0f; //

      // Respond to Pitch bender on ch.8 or to CC #7
      if ((Status == 0xE0 && Channel == 8) ||
          (Status == 0xB0 && rx.byte2 == 7))
      {
        // Move only if the requested value is distant from the current by 4 (to avoid a loopback)
        if (abs(rx.byte3 - fader_value) > 4)
          // Move to a 10 bit value
          MoveFader(rx.byte3 << 3);
      }
    }
  } while (rx.header != 0);
}
