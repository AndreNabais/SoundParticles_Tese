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
    std::vector<Position> source_positions; // Vector to store the positions of the source for each sample in the buffer

    public:
    //constructor
    Source(std::unique_ptr<Position_generator> start_pos, double amp, double freq, int sr, int bs);

    void SignalGenerator(Source& sources_vec, float n);

    float getSignalSample(int n) const;

    float getAmplitude() const;

    double getSampleRate() const;

    double getBufferSize() const;

    const std::vector<Position>& getSourcePositions() const;
};

#endif // SOURCE.H
