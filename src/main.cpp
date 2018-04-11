#include <Audio.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>
#include <SerialFlash.h>

AudioSynthWaveform waveform1;
AudioSynthWaveform waveform2;
AudioMixer4 mixer;
AudioEffectEnvelope envelopeL;
AudioEffectEnvelope envelopeR;
AudioOutputI2S i2s1;
AudioConnection patchCord1(waveform1, 0, mixer, 0);
AudioConnection patchCord2(waveform2, 0, mixer, 1);
AudioConnection patchCord3(mixer, envelopeL);
AudioConnection patchCord4(mixer, envelopeR);
AudioConnection patchCord5(envelopeL, 0, i2s1, 0);
AudioConnection patchCord6(envelopeR, 0, i2s1, 1);
AudioControlSGTL5000 sgtl5000_1;

void setup() {
  AudioMemory(10);

  // output
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.15);

  // oscillator - sine
  waveform1.frequency(110.0);
  waveform1.amplitude(0.9);
  waveform1.begin(WAVEFORM_SINE);

  // oscillator - saw
  waveform2.frequency(220.0 * 0.9997);
  waveform2.amplitude(0.3);
  waveform2.begin(WAVEFORM_SAWTOOTH);

  // mixer
  mixer.gain(0, 0.5);
  mixer.gain(1, 0.5);

  // envelope
  envelopeL.attack(500);
  envelopeL.decay(100);
  envelopeL.sustain(0.9);
  envelopeL.release(1000);

  envelopeR.attack(500);
  envelopeR.decay(100);
  envelopeR.sustain(0.9);
  envelopeR.release(1000);

  delay(1000);
}

void loop() {
  envelopeL.noteOn();
  delay(500);
  envelopeL.noteOff();
  delay(500);

  envelopeR.noteOn();
  delay(500);
  envelopeR.noteOff();
  delay(500);
}
