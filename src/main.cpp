#include <Audio.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>
#include <SerialFlash.h>

AudioSynthWaveform waveform1;
AudioSynthWaveform waveform2;
AudioMixer4 mixer;
AudioEffectEnvelope envelope1;
AudioOutputI2S i2s1;
AudioConnection patchCord1(waveform1, 0, mixer, 0);
AudioConnection patchCord2(waveform2, 0, mixer, 1);
AudioConnection patchCord3(mixer, envelope1);
AudioConnection patchCord4(envelope1, 0, i2s1, 0);
AudioConnection patchCord5(envelope1, 0, i2s1, 1);
AudioControlSGTL5000 sgtl5000_1;

void setup() {
  AudioMemory(10);

  // output
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.8);

  // oscillator - sine
  waveform1.frequency(220.0);
  waveform1.amplitude(0.9);
  waveform1.begin(WAVEFORM_SINE);

  // oscillator - saw
  waveform2.frequency(220.0 * 0.997);
  waveform2.amplitude(0.3);
  waveform2.begin(WAVEFORM_SAWTOOTH);

  // mixer
  mixer.gain(0, 0.5);
  mixer.gain(1, 0.5);

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
