#include "Source.h"
#include "Velocity.h"
#include "Acceleration.h"


//constructor
    Source::Source(double amp, double freq, int sr, int bs, float initX, float initY, float initZ)
    : Amplitude(amp), frequency(freq), SampleRate(sr), bufferSize(bs) {
        signal.resize(bufferSize);
        positions.resize(bufferSize);
        Velocity_history.resize(bufferSize);
        Acceleration_history.resize(bufferSize);
        for (int n=0; n<bufferSize; n++){
            signal[n] = Amplitude * sin(2.0 * M_PI * frequency * n / SampleRate);
            Position pos;
            pos = {initX, initY, initZ}; //for now, the source is static at the specified position
            positions[n] = pos;
        }
    }

    //getter for the signal buffer

    float Source::getSignalSample(int n) const {
        return signal[n];
    }

    Position Source::getPositionSample(int n) const {
        return positions[n];
    }

    double Source::getSampleRate() const{
        return SampleRate;
    }

    double Source::getBufferSize() const{
        return bufferSize;
    }
