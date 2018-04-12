#include <Audio.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>
#include <SerialFlash.h>

AudioSynthWaveform osc1a;
AudioSynthWaveform osc1b;
AudioSynthWaveform osc1c;
AudioSynthWaveform osc1d;
AudioSynthWaveform osc2a;
AudioSynthWaveform osc2b;
AudioSynthWaveform osc2c;
AudioSynthWaveform osc2d;
AudioMixer4 osc1mixer;
AudioMixer4 osc2mixer;
AudioMixer4 mixer;
AudioEffectEnvelope envelope;
AudioOutputI2S headphoneOutput;
AudioOutputUSB usbOutput;
AudioConnection patchCord1(osc1a, 0, osc1mixer, 0);
AudioConnection patchCord2(osc1b, 0, osc1mixer, 1);
AudioConnection patchCord3(osc1c, 0, osc1mixer, 2);
AudioConnection patchCord4(osc1d, 0, osc1mixer, 3);
AudioConnection patchCord5(osc2a, 0, osc2mixer, 0);
AudioConnection patchCord6(osc2b, 0, osc2mixer, 1);
AudioConnection patchCord7(osc2c, 0, osc2mixer, 2);
AudioConnection patchCord8(osc2d, 0, osc2mixer, 3);
AudioConnection patchCord9(osc1mixer, 0, mixer, 0);
AudioConnection patchCord10(osc2mixer, 0, mixer, 1);
AudioConnection patchCord11(mixer, envelope);
AudioConnection patchCord12(envelope, 0, usbOutput, 0);
AudioConnection patchCord13(envelope, 0, usbOutput, 1);
AudioControlSGTL5000 sgtl5000_1;

void setup() {
  float spread_cents = 3.0;
  float spread_up = powf(2.0, spread_cents / 1200.0);
  float spread_down = powf(2.0, -spread_cents / 1200.0);
  float spread_up_half = powf(2.0, spread_cents / 2.0 / 1200.0);
  float spread_down_half = powf(2.0, -spread_cents / 2.0 / 1200.0);

  AudioMemory(50);

  // output
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.15);

  // Osc 1
  float osc1_base_freq = 110.0; // A3
  osc1a.frequency(osc1_base_freq * spread_up);
  osc1a.sync(4.0);
  osc1a.amplitude(1.0);
  osc1a.phase(0.0);
  osc1a.begin(WAVEFORM_SQUARE);
  osc1b.frequency(osc1_base_freq * spread_down);
  osc1b.sync(4.0);
  osc1b.amplitude(1.0);
  osc1b.phase(45.0);
  osc1b.begin(WAVEFORM_SQUARE);
  osc1c.frequency(osc1_base_freq * spread_up_half);
  osc1c.sync(4.0);
  osc1c.amplitude(1.0);
  osc1c.phase(90.0);
  osc1c.begin(WAVEFORM_SQUARE);
  osc1d.frequency(osc1_base_freq * spread_down_half);
  osc1d.sync(4.0);
  osc1d.amplitude(1.0);
  osc1d.phase(135.0);
  osc1d.begin(WAVEFORM_SQUARE);

  // Osc 2
  float osc2_base_freq = 440.0 * powf(2, 10.9 / 1200.0);
  osc2a.frequency(osc2_base_freq * spread_up);
  osc2a.sync(1.476);
  osc2a.amplitude(1.0);
  osc2a.phase(0.0);
  osc2a.begin(WAVEFORM_SAWTOOTH_REVERSE);
  osc2b.frequency(osc2_base_freq * spread_down);
  osc2b.sync(1.476);
  osc2b.amplitude(1.0);
  osc2b.phase(45.0);
  osc2b.begin(WAVEFORM_SAWTOOTH_REVERSE);
  osc2c.frequency(osc2_base_freq * spread_up_half);
  osc2c.sync(1.476);
  osc2c.amplitude(1.0);
  osc2c.phase(90.0);
  osc2c.begin(WAVEFORM_SAWTOOTH_REVERSE);
  osc2d.frequency(osc2_base_freq * spread_down_half);
  osc2d.sync(1.476);
  osc2d.amplitude(1.0);
  osc2d.phase(135.0);
  osc2d.begin(WAVEFORM_SAWTOOTH_REVERSE);

  // mixers
  osc1mixer.gain(0, 0.5);
  osc1mixer.gain(1, 0.5);
  osc1mixer.gain(2, 0.5);
  osc1mixer.gain(3, 0.5);
  osc2mixer.gain(0, 0.5);
  osc2mixer.gain(1, 0.5);
  osc2mixer.gain(2, 0.5);
  osc2mixer.gain(3, 0.5);
  mixer.gain(0, 0.45); // Osc 1: eyeballing -0.61 dB
  mixer.gain(1, 0.5); // Osc 2: eyeballing 0 dB

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
