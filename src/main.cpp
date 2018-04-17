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
AudioFilterBiquad osc1filter;
AudioFilterBiquad osc2filter;
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
AudioConnection patchCord8b(osc1mixer, 0, osc1filter, 0);
AudioConnection patchCord8c(osc2mixer, 0, osc2filter, 0);
AudioConnection patchCord9(osc1filter, 0, mixer, 0);
AudioConnection patchCord10(osc2filter, 0, mixer, 1);
AudioConnection patchCord11(mixer, envelope);
AudioConnection patchCord12(envelope, 0, usbOutput, 0);
AudioConnection patchCord13(envelope, 0, usbOutput, 1);
AudioControlSGTL5000 sgtl5000_1;

void setup() {
  float num_voices = 4.0;
  float spread_cents = 3.0;
  float spread_increment = (spread_cents * 2.0) / (num_voices - 1.0);
  const float CENTS_PER_OCTAVE = 1200.0;
  float spread_up = powf(2.0, spread_cents / CENTS_PER_OCTAVE);
  float spread_down = powf(2.0, -spread_cents / CENTS_PER_OCTAVE);
  float spread_up_half = powf(2.0, (spread_cents - spread_increment) / CENTS_PER_OCTAVE);
  float spread_down_half = powf(2.0, (-spread_cents + spread_increment) / CENTS_PER_OCTAVE);

  AudioMemory(50);

  // output
  sgtl5000_1.enable();
  sgtl5000_1.volume(1.0);

  // Osc 1
  float osc1_base_freq = 110.0; // A3
  osc1a.frequency(osc1_base_freq * spread_up);
  osc1a.sync(4.0);
  osc1a.amplitude(1.0);
  osc1a.begin(WAVEFORM_SQUARETOOTH);
  osc1b.frequency(osc1_base_freq * spread_down);
  osc1b.sync(4.0);
  osc1b.amplitude(1.0);
  osc1b.begin(WAVEFORM_SQUARETOOTH);
  osc1c.frequency(osc1_base_freq * spread_up_half);
  osc1c.sync(4.0);
  osc1c.amplitude(1.0);
  osc1c.begin(WAVEFORM_SQUARETOOTH);
  osc1d.frequency(osc1_base_freq * spread_down_half);
  osc1d.sync(4.0);
  osc1d.amplitude(1.0);
  osc1d.begin(WAVEFORM_SQUARETOOTH);

  // Osc 2
  float osc2_base_freq = 4.0 * osc1_base_freq * powf(2, 10.9 / 1200.0);
  osc2a.frequency(osc2_base_freq * spread_up);
  osc2a.sync(1.476);
  osc2a.amplitude(1.0);
  osc2a.begin(WAVEFORM_SAWTOOTH_REVERSE);
  osc2b.frequency(osc2_base_freq * spread_down);
  osc2b.sync(1.476);
  osc2b.amplitude(1.0);
  osc2b.begin(WAVEFORM_SAWTOOTH_REVERSE);
  osc2c.frequency(osc2_base_freq * spread_up_half);
  osc2c.sync(1.476);
  osc2c.amplitude(1.0);
  osc2c.begin(WAVEFORM_SAWTOOTH_REVERSE);
  osc2d.frequency(osc2_base_freq * spread_down_half);
  osc2d.sync(1.476);
  osc2d.amplitude(1.0);
  osc2d.begin(WAVEFORM_SAWTOOTH_REVERSE);

  // filters
  float filter1freq = 457.572;
  float filter1q = 0.7;
  osc1filter.setLowpass(0, filter1freq, filter1q);
  osc1filter.setLowpass(1, filter1freq, filter1q);
  float filter2freq = 84.108;
  float filter2q = 0.5;
  osc2filter.setLowpass(0, filter2freq, filter2q);

  // mixers
  osc1mixer.gain(0, 0.15);
  osc1mixer.gain(1, 0.15);
  osc1mixer.gain(2, 0.15);
  osc1mixer.gain(3, 0.15);
  osc2mixer.gain(0, 0.15);
  osc2mixer.gain(1, 0.15);
  osc2mixer.gain(2, 0.15);
  osc2mixer.gain(3, 0.15);
  mixer.gain(0, 0.9); // Osc 1: eyeballing -0.61 dB
  mixer.gain(1, 1.0); // TEMP // Osc 2: eyeballing 0 dB

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
