#include <Audio.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>
#include <SerialFlash.h>

#define NUM_FIR_COEFFS 4

void initOscillatorSmoothers();
void setEqEnvelope(float t);
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
AudioMixer4 osc1Lmixer;
AudioMixer4 osc1Rmixer;
AudioMixer4 osc2Lmixer;
AudioMixer4 osc2Rmixer;
AudioFilterFIR osc1Lfirfilter;
AudioFilterFIR osc1Rfirfilter;
AudioFilterFIR osc2Lfirfilter;
AudioFilterFIR osc2Rfirfilter;
AudioFilterBiquad osc1Lfilter;
AudioFilterBiquad osc1Rfilter;
AudioFilterBiquad osc2Lfilter;
AudioFilterBiquad osc2Rfilter;
AudioMixer4 mixerL;
AudioMixer4 mixerR;
AudioEffectEnvelope envelopeL;
AudioEffectEnvelope envelopeR;
AudioOutputI2S headphoneOutput;
AudioOutputUSB usbOutput;

// dry osc mix
AudioConnection patchCord1(osc1a, 0, osc1Lmixer, 0);
AudioConnection patchCord1a(osc1a, 0, osc1Rmixer, 0);
AudioConnection patchCord2(osc1b, 0, osc1Lmixer, 1);
AudioConnection patchCord2a(osc1b, 0, osc1Rmixer, 1);
AudioConnection patchCord3(osc1c, 0, osc1Lmixer, 2);
AudioConnection patchCord3a(osc1c, 0, osc1Rmixer, 2);
AudioConnection patchCord4(osc1d, 0, osc1Lmixer, 3);
AudioConnection patchCord4a(osc1d, 0, osc1Rmixer, 3);
AudioConnection patchCord5(osc2a, 0, osc2Lmixer, 0);
AudioConnection patchCord5a(osc2a, 0, osc2Rmixer, 0);
AudioConnection patchCord6(osc2b, 0, osc2Lmixer, 1);
AudioConnection patchCord6a(osc2b, 0, osc2Rmixer, 1);
AudioConnection patchCord7(osc2c, 0, osc2Lmixer, 2);
AudioConnection patchCord7a(osc2c, 0, osc2Rmixer, 2);
AudioConnection patchCord8(osc2d, 0, osc2Lmixer, 3);
AudioConnection patchCord8a(osc2d, 0, osc2Rmixer, 3);

// dry osc mix -> FIR
AudioConnection patchCord8b(osc1Lmixer, 0, osc1Lfirfilter, 0);
AudioConnection patchCord8b2(osc1Rmixer, 0, osc1Rfirfilter, 0);
AudioConnection patchCord8c(osc2Lmixer, 0, osc2Lfirfilter, 0);
AudioConnection patchCord8c2(osc2Rmixer, 0, osc2Rfirfilter, 0);

// FIR -> biquad
AudioConnection patchCord8d(osc1Lfirfilter, 0, osc1Lfilter, 0);
AudioConnection patchCord8d2(osc1Rfirfilter, 0, osc1Rfilter, 0);
AudioConnection patchCord8e(osc2Lfirfilter, 0, osc2Lfilter, 0);
AudioConnection patchCord8e2(osc2Rfirfilter, 0, osc2Rfilter, 0);

AudioConnection patchCord9(osc1Lfilter, 0, mixerL, 0);
AudioConnection patchCord9a(osc1Rfilter, 0, mixerR, 0);
AudioConnection patchCord10(osc2Lfilter, 0, mixerL, 1);
AudioConnection patchCord10a(osc2Rfilter, 0, mixerR, 1);

AudioConnection patchCord11(mixerL, envelopeL);
AudioConnection patchCord11a(mixerR, envelopeR);

AudioConnection patchCord12(envelopeL, 0, usbOutput, 0);
AudioConnection patchCord13(envelopeR, 0, usbOutput, 1);

AudioControlSGTL5000 sgtl5000_1;

void setup() {
  AudioMemory(50);

  sgtl5000_1.enable();
  sgtl5000_1.volume(1.0);

  initOscillators();
  initOscillatorSmoothers();
  initOscillatorMixers();
  initEnvelope();
}

void loop() {
  float ratio = 1.0 + ((rand() % 4) / 4.0);
  float octave_multiplier = powf(2.0, rand() % 2);
  setFrequency(110.0 * ratio * octave_multiplier);

  float duration = 250.0 * powf(2.0, rand() % 3);
  const float legato = 0.75;

  envelopeL.noteOn();
  envelopeR.noteOn();
  float t;
  float startMillis = millis();
  while ((t = (millis() / (startMillis + (duration * legato)))) < 1.0) {
    setEqEnvelope(t);
  }

  envelopeL.noteOff();
  envelopeR.noteOff();
  delay(duration * (1.0 - legato));
}

void initOscillatorSmoothers() {
  osc1Lfirfilter.begin(fir_coeffs, NUM_FIR_COEFFS);
  osc1Rfirfilter.begin(fir_coeffs, NUM_FIR_COEFFS);
  osc2Lfirfilter.begin(fir_coeffs, NUM_FIR_COEFFS);
  osc2Rfirfilter.begin(fir_coeffs, NUM_FIR_COEFFS);
}

void setEqEnvelope(float t) {
  float t_cubic = t * t * t;
  float filter1freq = 457.572 + (t_cubic * 1000.0);
  float filter1q = 0.7 + (t_cubic * 0.5);
  osc1Lfilter.setLowpass(0, filter1freq, filter1q);
  osc1Rfilter.setLowpass(0, filter1freq, filter1q);
  osc1Lfilter.setLowpass(1, filter1freq, filter1q);
  osc1Rfilter.setLowpass(1, filter1freq, filter1q);
  float filter2freq = 84.108;
  float filter2q = 0.9;
  osc2Lfilter.setLowpass(0, filter2freq, filter2q);
  osc2Rfilter.setLowpass(0, filter2freq, filter2q);
}

void initOscillators() {
  float osc1sync = 1.0;
  osc1a.sync(osc1sync);
  osc1a.amplitude(1.0);
  osc1a.begin(WAVEFORM_SQUARETOOTH);
  osc1b.sync(osc1sync);
  osc1b.amplitude(1.0);
  osc1b.begin(WAVEFORM_SQUARETOOTH);
  osc1c.sync(osc1sync);
  osc1c.amplitude(1.0);
  osc1c.begin(WAVEFORM_SQUARETOOTH);
  osc1d.sync(osc1sync);
  osc1d.amplitude(1.0);
  osc1d.begin(WAVEFORM_SQUARETOOTH);

  float osc2sync = 1.476;
  osc2a.sync(osc2sync);
  osc2a.amplitude(1.0);
  osc2a.begin(WAVEFORM_SAWTOOTH_REVERSE);
  osc2b.sync(osc2sync);
  osc2b.amplitude(1.0);
  osc2b.begin(WAVEFORM_SAWTOOTH_REVERSE);
  osc2c.sync(osc2sync);
  osc2c.amplitude(1.0);
  osc2c.begin(WAVEFORM_SAWTOOTH_REVERSE);
  osc2d.sync(osc2sync);
  osc2d.amplitude(1.0);
  osc2d.begin(WAVEFORM_SAWTOOTH_REVERSE);
}

void initOscillatorMixers() {
  osc1Lmixer.gain(0, 0.15);
  osc1Lmixer.gain(1, 0.10);
  osc1Lmixer.gain(2, 0.05);
  osc1Lmixer.gain(3, 0.0);

  osc1Rmixer.gain(0, 0.0);
  osc1Rmixer.gain(1, 0.05);
  osc1Rmixer.gain(2, 0.10);
  osc1Rmixer.gain(3, 0.15);

  osc2Lmixer.gain(0, 0.15);
  osc2Lmixer.gain(1, 0.10);
  osc2Lmixer.gain(2, 0.05);
  osc2Lmixer.gain(3, 0.0);

  osc2Rmixer.gain(0, 0.0);
  osc2Rmixer.gain(1, 0.05);
  osc2Rmixer.gain(2, 0.10);
  osc2Rmixer.gain(3, 0.15);

  mixerL.gain(0, 0.9);
  mixerL.gain(1, 1.0);
  mixerR.gain(0, 0.9);
  mixerR.gain(1, 1.0);
}

void initEnvelope() {
  envelopeL.attack(10.0);
  envelopeR.attack(10.0);
  envelopeL.decay(30.0);
  envelopeR.decay(30.0);
  envelopeL.sustain(0.8);
  envelopeR.sustain(0.8);
  envelopeL.hold(0.0);
  envelopeR.hold(0.0);
  envelopeL.release(100.0);
  envelopeR.release(100.0);
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
