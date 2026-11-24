#ifndef SOURCE_H
#define SOURCE_H

#include "Position.h"
#include "PhysicalObject.h"

class Source : public PhysicalObject {
    private:
    double Amplitude;
    double frequency;
    int SampleRate;
    int bufferSize;
    std::vector<float> signal;

    public:
    //constructor
    Source(std::unique_ptr<Position_generator> start_pos, double amp, double freq, int sr, int bs);

    void SignalGenerator(Source& sources_vec, float n);

    float getSignalSample(int n) const;

    float getAmplitude() const;

    double getSampleRate() const;

    double getBufferSize() const;
};

#endif // SOURCE.H

