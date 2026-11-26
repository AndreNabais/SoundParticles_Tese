#ifndef MICROPHONE_H
#define MICROPHONE_H

#include "Position.h"
#include "PhysicalObject.h"
#include "Source.h"
#include "Position_generator.h"

enum class PolarPattern { Omnidirectional, Cardioid };

class Microphone : public PhysicalObject {
    private:
        std::vector<float> captured_signal;
        int bufferSize;
        int Sample_Rate;
        PolarPattern polar_pattern;
        Position mic_facing;

    public:
    //constructor
    Microphone(std::unique_ptr<Position_generator> start_pos, int bs, int sample_rate);

    void mic_capture(const Source& src);

    float getCapturedSample(int n) const;

    void update_recv_sound(const std::vector<Source>& sources_vec, int current_sample);

    void setPolarPattern(PolarPattern p, Position facing = Position{1.0f, 0.0f, 0.0f});

    float directional_gain(const Position& source_pos, const Position& mic_pos) const;

    std::vector<float> root_emit_time;
    std::vector<float> root_arrival_time;
    std::vector<float> root_derivative;

};

#endif // MICROPHONE.H
