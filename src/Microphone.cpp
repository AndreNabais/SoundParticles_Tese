#include "Microphone.h"
#include "Distance.h"
#include "Velocity.h"
#include "Acceleration.h"
#include "Position_generator.h"

using namespace std;

//constructor
Microphone::Microphone(std::unique_ptr<Position_generator> start_pos, int bs, int sample_rate) : PhysicalObject(std::move(start_pos)), bufferSize(bs), Sample_Rate(sample_rate) {
    captured_signal.resize(bufferSize);

}

void Microphone::mic_capture(const Source& src) {
    for (int n = 0; n < bufferSize; n++) {
        captured_signal[n] = src.getSignalSample(n);
    }
}

//getter for the captured signal buffer
float Microphone::getCapturedSample(int n) const {
    return captured_signal[n];
}


void Microphone::setPolarPattern(PolarPattern p, Position facing) {
    polar_pattern = p;

    // normalize the facing direction so that cos(theta) comes out right
    float len = sqrt(facing.x * facing.x + facing.y * facing.y + facing.z * facing.z);
    if (len > 0.0f) {
        mic_facing.x = facing.x / len;
        mic_facing.y = facing.y / len;
        mic_facing.z = facing.z / len;
    }
}

float Microphone::directional_gain(const Position& source_pos, const Position& mic_pos) const {
    if (polar_pattern == PolarPattern::Omnidirectional) {
        return 1.0f;
    }

    // direction from the mic to the source
    float dx = source_pos.x - mic_pos.x;
    float dy = source_pos.y - mic_pos.y;
    float dz = source_pos.z - mic_pos.z;
    float d = sqrt(dx * dx + dy * dy + dz * dz);
    if (d == 0.0f) return 1.0f;   // source exactly at the mic: no defined direction

    float cos_theta = (dx * mic_facing.x + dy * mic_facing.y + dz * mic_facing.z) / d;

    return 0.5f * (1.0f + cos_theta);   // cardioid: 1 at the front, 0.5 at the sides, 0 at the back
}


// WITH ATTENUATION AND MIC SENSITIVITY, OTIMIZED BUT WITHOUT THE VECTOR FOR SOURCE POSITIONS.
void Microphone::update_recv_sound(const std::vector<Source>& sources_vec, int current_sample) {
//current_sample is the sample index of the microphone's buffer that we are currently filling with the sound received from the sources


    captured_signal[current_sample] = 0.0f; // Inicializa o sample atual
    float c = 343.0f; // Velocidade do som em m/s

    float mic_time = static_cast<float>(current_sample) / static_cast<float>(Sample_Rate); //time of the current sample of the microphone's buffer
    Position mic_pos_now = this->getPosition(mic_time); //position of the microphone at the time of the current sample
    Position src_pos_emit = {0.0f, 0.0f, 0.0f}; //initialize the source position at the time of emission to 0.0f, will be updated for each source in the loop
    float gain = 0.0f; //initialize the directional gain to 0.0f, will be updated for each source in the loop

    float source_time = 0.0f; //initialize the source time to 0.0f, will be updated for each source in the loop
    float prev_source_time = 0.0f; //initialize the previous source time to 0.0f, will be updated for each source in the loop

    float distance_check = 0.0f; // distance from the source at emitted_sample to the mic at current_sample
    float prev_distance_check = 0.0f; // previous value of distance_check to detect when the sound has reached the mic

    float sound_distance = 0.0f; //distance the sound travels from the source at emitted_sample to the mic at current_sample
    //float prev_sound_distance = 0.0f; // previous value of sound_distance to detect when the sound has reached the mic

    float arrival_time = 0.0f; // time when the sound from the source sample reaches the mic sample
    float prev_arrival_time = 0.0f; // previous value of arrival_time to detect when the sound has reached the mic
    float delta_source_time = 0.0f; // time difference between the current and previous source sample
    float arrival_derivative = 0.0f; // derivative of arrival_time with respect to source_time, to detect when the sound has reached the mic 

    // n_float_mic is the fractional sample index of the microphone's buffer where the emitted sample lands, used for interpolation
    // if 0, it means it lands exactly at the current sample of the microphone's buffer
    float n_float_mic = 0.0f; 

    float sensitivity = 1.00f; // microphone sensitivity factor, can be adjusted based on the microphone's characteristics. For now we use a number from 0 to 1. If 1, the microphone captures the sound at full amplitude. If 0, the microphone captures no sound. This can be used to simulate different microphone sensitivities or to apply a gain factor to the captured signal.
    float Amp_min = (1.0f - sensitivity) * 1.0f;  // minimum amplitude that the microphone can capture based on its sensitivity. If the source amplitude is below this threshold, the microphone will not capture it. We multiply by 1.0f to keep the units consistent with the source amplitude, which is also in the range of 0 to 1. This can be adjusted based on the microphone's characteristics or the desired sensitivity level.
    


    for (const auto& source : sources_vec) {


        float Amp_source = source.getAmplitude(); // get the amplitude of the source signal, which is a value between 0 and 1. This can be adjusted based on the source's characteristics or the desired amplitude level.

        if (Amp_source < Amp_min) {
            continue; // skip this source if its amplitude is below the microphone's sensitivity threshold. We'll see later if the sound from this source sample reaches the mic with enough amplitude after attenuation and directional gain.
        }

        float r_max = 0.0f; // maximum distance considered (m): beyond this distance, the sound is considered too weak to be captured by the microphone. This is a simplification for performance reasons, as we don't want to check every source sample for every mic sample if they are too far apart.
        int lowest_sample = 1; // lowest sample index of the source's buffer that we will check for this source, based on the maximum distance and the speed of sound. If mic_sensitivity is 1, we will check all samples. If mic_sensitivity is 0, we will skip all samples. If mic_sensitivity is between 0 and 1, we will check only the samples that are within the distance that corresponds to the sensitivity threshold.
        if (Amp_min > 0.0f) {
            r_max = Amp_source / Amp_min; // maximum distance that the sound from this source can travel and still be above the microphone's sensitivity threshold, based on the inverse distance attenuation model (1/r). If the source amplitude is 0.5 and the mic sensitivity is 0.5, then r_max = 1.0 m. If the source amplitude is 1.0 and the mic sensitivity is 0.1, then r_max = 10.0 m.
            float max_delay_samples = static_cast<float>(r_max / c * static_cast<float>(Sample_Rate)) + 16.0f; // +16: margin for the Lagrange stencil
            if (current_sample > max_delay_samples) {
                lowest_sample = current_sample - static_cast<int>(max_delay_samples);
            }
        }

        float carried_distance = Distance_source_microphone(source.getSourcePositions()[current_sample], mic_pos_now);

        // emitted_sample is the sample index of the source's buffer that we are currently checking if it has reached the microphone. we start from the current sample of the microphone and go backwards in time to find the source sample that corresponds to the sound arriving at the microphone now.
        for (int emitted_sample = current_sample; emitted_sample >= lowest_sample ; emitted_sample--){


            source_time = static_cast<float>(emitted_sample) / static_cast<float>(Sample_Rate);
            prev_source_time = static_cast<float>(emitted_sample - 1) / static_cast<float>(Sample_Rate);
            distance_check = carried_distance;
            sound_distance = c*(mic_time - source_time); 
            //prev_sound_distance = c*(mic_time - prev_source_time);
            prev_distance_check = Distance_source_microphone(source.getSourcePositions()[emitted_sample - 1], mic_pos_now); //usar index do vetor
            carried_distance = prev_distance_check; // update the carried distance for the next iteration

            //offset parameter is created to check how many mic samples away the sound from the source sample is from the current mic sample.
            //if offset is 0, the means the sound from the emitted sample at the source lands exactly at the current sample of the microphone's buffer.
            //if offset is positive, the sound from the emitted sample at the source hasn't landed yet at the current sample of the microphone's buffer, it will land in the future.
            //if offset is negative, the sound from the emitted sample at the source has already landed before the current sample of the microphone's buffer.
            float offset = (distance_check - sound_distance) / c * static_cast<float>(Sample_Rate); // convert the time offset to a sample offset

            //arrival time is the emission time plus the time it takes for the sound to travel from the source to the mic
            arrival_time = source_time + distance_check / c;

            prev_arrival_time = prev_source_time + prev_distance_check / c;

            delta_source_time = source_time - prev_source_time;

            arrival_derivative = (arrival_time - prev_arrival_time) / delta_source_time;

            //The width represents the derivative of the arrival time, which tells us how much the sound wave is compressed or stretched.
            //If derivative > 1, the sound wave is stretched, so we'll need to rescale by the derivative so that the emissions are interpolated with no gaps.
            //if derivative < 1, the sound wave is compressed and no rescale is needed, so that the emissions stack causing compression of the waves which leads to sonic boom.
            float width = fabs(arrival_derivative); 
            if (width < 1.0f) {
                width = 1.0f; 
            }

            float inverse_width = 1.0f / width; // rescale the amplitude of the source signal by the inverse of the width to conserve energy

            //Lagrange interpolation on the mic time axis (the current mic sample is position 0)
            n_float_mic = offset * inverse_width;
            int M_mic = static_cast<int>(floor(n_float_mic));
            int N = 5; //5th-order interpolation with 6 points
            int start_index = M_mic - (N / 2); //stencil: M-2 ... M+3 (positions relative to the current mic sample)
            int k = 0 - start_index; //which of the 6 stencil nodes is the current mic sample

            //cheap test first: emissions that don't land within the 6-point stencil contribute nothing
            if (k < 0 || k > N) {
                continue;
            }

            //only emissions that do land here get the expensive work

            //directional gain based on the polar pattern of the microphone
            src_pos_emit = source.getSourcePositions()[emitted_sample]; //position of the source at the time of emission
            gain = directional_gain(src_pos_emit, mic_pos_now);

            //Attenuation
            
            float r = distance_check; // distance from the source at emitted_sample to the mic at current_sample
            if (r < 1.0f) {
                r = 1.0f; // prevent division by zero or too much amplification
            }
            float attenuation = 1.0f / r; // we use the inverse of the distance as a simple attenuation model (1/r), which is a common approximation for sound propagation in free space. More complex models could include air absorption, reflections, etc.


            //without attenuation:
            //float attenuation = 1.0f; // no attenuation for now, we can add it later if needed

            if (Amp_source * attenuation * gain < Amp_min) {
                continue; // skip this source sample if its amplitude after attenuation is below the microphone's sensitivity threshold
            }

            else {

            if (offset >= -0.5f && offset < 0.5f) {
                root_emit_time.push_back(source_time);
                root_arrival_time.push_back(mic_time);
                root_derivative.push_back(arrival_derivative);
            }

            
                int current_k_sample = 0; // x_k in the Lagrange formula = the current mic sample
                float numerator = 1.0f;
                float denominator = 1.0f;

                //product loop for Lagrange interpolation
                for (int j = 0; j <= N; j++) {
                    if (j != k) {
                        int current_j_sample = start_index + j; // x_j in the Lagrange formula
                        numerator *= n_float_mic - static_cast<float>(current_j_sample);
                        denominator *= static_cast<float>(current_k_sample) - static_cast<float>(current_j_sample);
                    }
                }

                float l_k = numerator / denominator; // Lagrange basis polynomial L_k(x)
                captured_signal[current_sample] += source.getSignalSample(emitted_sample) * (l_k * inverse_width) * gain * attenuation; // add the contribution of this source sample to the current mic sample
            }
        }
    }
}

