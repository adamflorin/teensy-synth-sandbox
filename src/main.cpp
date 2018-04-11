#include <Audio.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>
#include <SerialFlash.h>

AudioSynthWaveform waveform1;
AudioEffectEnvelope envelope1;
AudioOutputI2S i2s1;
AudioConnection patchCord1(waveform1, envelope1);
AudioConnection patchCord2(envelope1, 0, i2s1, 0);
AudioConnection patchCord3(envelope1, 0, i2s1, 1);
AudioControlSGTL5000 sgtl5000_1;

void setup() {
  AudioMemory(10);

  // output
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.8);

  // oscillator
  waveform1.frequency(220.0);
  waveform1.amplitude(0.9);
  waveform1.begin(WAVEFORM_SINE);

  // envelope
  envelope1.attack(100);
  envelope1.decay(100);
  envelope1.sustain(0.9);
  envelope1.release(900);

  delay(1000);
}

void loop() {
  // on
  envelope1.noteOn();
  delay(250);

  // off
  envelope1.noteOff();
  delay(1750);
}
