#include <Audio.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>
#include <SerialFlash.h>

#define NUM_FIR_COEFFS 4

void initOscillatorSmoothers();
void initEq();
void initOscillators();
void initOscillatorMixers();
void initEnvelope();
void setFrequency(float frequency);

// max positive value is 0x7FFF; sum to something near
short fir_coeffs[NUM_FIR_COEFFS] = {
  (short)0x1000,
  (short)0x3000,
  (short)0x3000,
  (short)0x1000,
};

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
AudioFilterFIR osc1firfilter;
AudioFilterFIR osc2firfilter;
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
AudioConnection patchCord8b(osc1mixer, 0, osc1firfilter, 0);
AudioConnection patchCord8c(osc2mixer, 0, osc2firfilter, 0);
AudioConnection patchCord8d(osc1firfilter, 0, osc1filter, 0);
AudioConnection patchCord8e(osc2firfilter, 0, osc2filter, 0);
AudioConnection patchCord9(osc1filter, 0, mixer, 0);
AudioConnection patchCord10(osc2filter, 0, mixer, 1);
AudioConnection patchCord11(mixer, envelope);
AudioConnection patchCord12(envelope, 0, usbOutput, 0);
AudioConnection patchCord13(envelope, 0, usbOutput, 1);
AudioControlSGTL5000 sgtl5000_1;

void setup() {
  AudioMemory(50);

  sgtl5000_1.enable();
  sgtl5000_1.volume(1.0);

  initOscillators();
  initOscillatorSmoothers();
  initEq();
  initOscillatorMixers();
  initEnvelope();
}

void loop() {
  float ratio = 1.0 + ((rand() % 4) / 4.0);
  float octave_multiplier = powf(2.0, rand() % 2);
  setFrequency(110.0 * ratio * octave_multiplier);

  float duration = 250.0 * powf(2.0, rand() % 3);
  const float legato = 0.75;

  envelope.noteOn();
  delay(duration * legato);

  envelope.noteOff();
  delay(duration * (1.0 - legato));
}

void initOscillatorSmoothers() {
  osc1firfilter.begin(fir_coeffs, NUM_FIR_COEFFS);
  osc2firfilter.begin(fir_coeffs, NUM_FIR_COEFFS);
}

void initEq() {
  float filter1freq = 457.572;
  float filter1q = 0.7;
  osc1filter.setLowpass(0, filter1freq, filter1q);
  osc1filter.setLowpass(1, filter1freq, filter1q);
  float filter2freq = 84.108;
  float filter2q = 0.9;
  osc2filter.setLowpass(0, filter2freq, filter2q);
}

void initOscillators() {
  osc1a.sync(1.0);
  osc1a.amplitude(1.0);
  osc1a.begin(WAVEFORM_SQUARE);
  osc1b.sync(1.0);
  osc1b.amplitude(1.0);
  osc1b.begin(WAVEFORM_SQUARE);
  osc1c.sync(1.0);
  osc1c.amplitude(1.0);
  osc1c.begin(WAVEFORM_SQUARE);
  osc1d.sync(1.0);
  osc1d.amplitude(1.0);
  osc1d.begin(WAVEFORM_SQUARE);

  osc2a.sync(1.0);
  osc2a.amplitude(1.0);
  osc2a.begin(WAVEFORM_SAWTOOTH_REVERSE);
  osc2b.sync(1.0);
  osc2b.amplitude(1.0);
  osc2b.begin(WAVEFORM_SAWTOOTH_REVERSE);
  osc2c.sync(1.0);
  osc2c.amplitude(1.0);
  osc2c.begin(WAVEFORM_SAWTOOTH_REVERSE);
  osc2d.sync(1.0);
  osc2d.amplitude(1.0);
  osc2d.begin(WAVEFORM_SAWTOOTH_REVERSE);
}

void initOscillatorMixers() {
  osc1mixer.gain(0, 0.15);
  osc1mixer.gain(1, 0.15);
  osc1mixer.gain(2, 0.15);
  osc1mixer.gain(3, 0.15);
  osc2mixer.gain(0, 0.15);
  osc2mixer.gain(1, 0.15);
  osc2mixer.gain(2, 0.15);
  osc2mixer.gain(3, 0.15);
  mixer.gain(0, 0.9);
  mixer.gain(1, 1.0);
}

void initEnvelope() {
  envelope.attack(10.0);
  envelope.decay(30.0);
  envelope.sustain(0.8);
  envelope.hold(0.0);
  envelope.release(100.0);
}

void setFrequency(float frequency) {
  float num_voices = 4.0;
  float spread_cents = 3.0;
  float spread_increment = (spread_cents * 2.0) / (num_voices - 1.0);
  const float CENTS_PER_OCTAVE = 1200.0;
  float spread_up = powf(2.0, spread_cents / CENTS_PER_OCTAVE);
  float spread_down = powf(2.0, -spread_cents / CENTS_PER_OCTAVE);
  float spread_up_half = powf(2.0, (spread_cents - spread_increment) / CENTS_PER_OCTAVE);
  float spread_down_half = powf(2.0, (-spread_cents + spread_increment) / CENTS_PER_OCTAVE);

  float osc1_base_freq = frequency;
  float osc2_base_freq = 4.0 * osc1_base_freq * powf(2, 10.9 / 1200.0);

  osc1a.frequency(osc1_base_freq * spread_up);
  osc1b.frequency(osc1_base_freq * spread_down);
  osc1c.frequency(osc1_base_freq * spread_up_half);
  osc1d.frequency(osc1_base_freq * spread_down_half);
  osc2a.frequency(osc2_base_freq * spread_up);
  osc2b.frequency(osc2_base_freq * spread_down);
  osc2c.frequency(osc2_base_freq * spread_up_half);
  osc2d.frequency(osc2_base_freq * spread_down_half);

  initOscillators();
}
