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
    float referenceLevel; //reference sound pressure level in dB
    float referenceDistance; //reference distance from the source to the point that has the referenceLevel sound pressure
    std::vector<float> signal;

    public:
    //constructor
    Source(double amp, double freq, int sr, int bs, float initX, float initY, float initZ);
    
    float getSignalSample(int n) const;

    Position getPositionSample(int n) const;

    double getSampleRate() const;

    double getBufferSize() const;
};

#endif // SOURCE.H
