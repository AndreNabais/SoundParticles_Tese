#ifndef MICROPHONE_H
#define MICROPHONE_H

#include "Position.h"
#include "PhysicalObject.h"
#include "Source.h"

class Microphone : public PhysicalObject {
    private:
        std::vector<float> captured_signal;
        int bufferSize;

    public:
    //constructor
    Microphone(int bs, float initX, float initY, float initZ);

    void mic_capture(const Source& src);

    float getCapturedSample(int n) const;

    Position getPositionSample(int n) const;

    void update_recv_sound(const std::vector<Source>& sources_vec, int current_sample);
};

#endif // MICROPHONE.H