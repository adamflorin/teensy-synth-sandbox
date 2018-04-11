#include <Audio.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>
#include <SerialFlash.h>

AudioSynthWaveform    waveform1;
AudioOutputI2S        i2s1;
AudioConnection       patchCord1(waveform1, 0, i2s1, 0);
AudioConnection       patchCord2(waveform1, 0, i2s1, 1);
AudioControlSGTL5000  sgtl5000_1;

void setup() {
  AudioMemory(10);

  Serial.begin(115200);

  sgtl5000_1.enable();

  sgtl5000_1.volume(0.3);

  waveform1.begin(WAVEFORM_SINE);

  delay(1000);
}

void loop() {
  waveform1.frequency(1280);
  waveform1.amplitude(0.9);
  delay(250);

  waveform1.amplitude(0);
  delay(1750);
}
