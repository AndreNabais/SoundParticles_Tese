#include "Source.h"
#include "Velocity.h"
#include "Acceleration.h"
#include "Position_generator.h"


//constructor
    Source::Source(std::unique_ptr<Position_generator> start_pos, double amp, double freq, int sr, int bs)
    : PhysicalObject(std::move(start_pos)), Amplitude(amp), frequency(freq), SampleRate(sr), bufferSize(bs) {
        signal.resize(bufferSize);
    }

    void Source::SignalGenerator(Source& sources_vec, float n) {
        //sinusoidal wave
        signal[n] = Amplitude * sin(2.0 * M_PI * frequency * n / SampleRate); // +1.0f to shift the signal to be positive, for troubleshooting purposes, can be removed later
    }


    //getter for the signal buffer

    float Source::getSignalSample(int n) const {
        return signal[n];
    }

    float Source::getAmplitude() const {
        return Amplitude;
    }

    double Source::getSampleRate() const{
        return SampleRate;
    }

    double Source::getBufferSize() const{
        return bufferSize;
    }
