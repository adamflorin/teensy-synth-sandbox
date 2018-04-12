#include <Audio.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>
#include <SerialFlash.h>

AudioSynthWaveform osc1;
// AudioSynthWaveform osc2;
AudioMixer4 mixer;
AudioEffectEnvelope envelope;
AudioOutputI2S fakeOutput;
AudioOutputUSB output;
AudioConnection patchCord1(osc1, 0, mixer, 0);
// AudioConnection patchCord2(osc2, 0, mixer, 1);
AudioConnection patchCord3(mixer, envelope);
AudioConnection patchCord5(envelope, 0, output, 0);
AudioConnection patchCord6(envelope, 0, output, 1);
AudioControlSGTL5000 sgtl5000_1;

void setup() {
  AudioMemory(10);

  // output
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.15);

  // Osc 1
  osc1.frequency(110.0); // A2
  osc1.sync(4.0);
  osc1.amplitude(0.9);
  osc1.begin(WAVEFORM_SQUARE_HARD_SYNC);

  // // oscillator - saw
  // osc2.frequency(220.0 * 0.9997);
  // osc2.amplitude(0.3);
  // osc2.begin(WAVEFORM_SAWTOOTH);

  // mixer
  mixer.gain(0, 0.5);
  // mixer.gain(1, 0.5);

  // envelope
  envelope.attack(10.0);
  envelope.decay(2144.0);
  envelope.sustain(0.6); // eyeballing -3.54 dB
  envelope.hold(0.0);
  envelope.release(1076.0);

  delay(1000);
}

void loop() {
  envelope.noteOn();
  delay(500);
  envelope.noteOff();
  delay(1500);
}
