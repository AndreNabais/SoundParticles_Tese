#include "Microphone.h"
#include "Distance.h"
#include "Velocity.h"
#include "Acceleration.h"
//a header includes only what its own declarations need. Everything else goes in the .cpp.

using namespace std;

//constructor
Microphone::Microphone(int bs, float initX, float initY, float initZ) : bufferSize(bs) {
    captured_signal.resize(bufferSize);
    positions.resize(bufferSize);
            Velocity_history.resize(bufferSize);
        Acceleration_history.resize(bufferSize);
    for (int n=0; n<bufferSize; n++){
        Position pos;
        pos = {initX, initY, initZ}; //for now, the microphone is static at the specified position
        positions[n] = pos;
    }
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

Position Microphone::getPositionSample(int n) const {
    return positions[n];
}


void Microphone::update_recv_sound(const std::vector<Source>& sources_vec, int current_sample) {

    captured_signal[current_sample] = 0.0f; // Inicializa o sample atual
    float c = 343.0f;          // Velocidade do som em m/s

    for (const auto& source : sources_vec) {

        //calculate distance from the source to the microphone for each sample of the source's signal, and determine if the sound from that sample has reached the microphone at the current time step
        for (int i = current_sample; i > 0 ; i--){
            
            float distance_check = Distance_source_microphone(source, *this, i, current_sample);

            float delay_samples = current_sample - i;
            Velocity closing_v = Closing_velocity(source, *this, i, current_sample);

            float elapsed_time = delay_samples / static_cast<float>(source.getSampleRate());
            float required_time = distance_check / c;

            float mach_closeness = fabs(1.0f - closing_v.x / c); //for now, only velocity in the x axis exists
            float tolerance;

            if (mach_closeness < 0.01f) { 
                // near Mach 1 head-on: widen tolerance to reliably capture coalescence
                tolerance = 1.0f / source.getSampleRate(); // full sample period
                }
            else {
                // normal case: keep tight tolerance for accurate single-sample matching
                tolerance = 0.5f * mach_closeness * (1.0f / source.getSampleRate());
                }
            
            if (fabs(required_time - elapsed_time) < tolerance) {
                //fractional delay interpolation for mic
                float exact_delay_samples = static_cast<float>((distance_check / c) * source.getSampleRate());
                float n_float = static_cast<float>(current_sample) - exact_delay_samples;
                int M = static_cast<int>(floor(n_float));
                float frac = n_float - M;

                if (M >= 0 && (M + 1) < source.getBufferSize()) {
                    // This is y[n] = x[n-(M+1)]*frac + x[n-M]*(1-frac)
                    // Here, index is (M) and (M+1) because we already subtracted delay to get birth_time
                    float y_n = source.getSignalSample(M+1) * frac + source.getSignalSample(M) * (1.0 - frac);
                    float arbitrary_attenuation = 0.90f; //arbitrary attenuation factor to avoid too much attenuation
                    

                    captured_signal[current_sample] += y_n*arbitrary_attenuation;
                }

                else if (M == 0) {
                // Boundary safety fallback
                float arbitrary_attenuation = 0.90f; 
                captured_signal[current_sample] += source.getSignalSample(0)*arbitrary_attenuation;
                }

                else if (M == -1) {
                }

                else {
                    // Out of bounds, do nothing
                }


                if (current_sample % 500 == 0) {
                    
                cout << "Mic Sample: " << current_sample 
                    << " | Source Sample: " << i 
                    << " | Delay: " << delay_samples << " samples" << endl;
                }
            }
        }
    }
}
 // */


