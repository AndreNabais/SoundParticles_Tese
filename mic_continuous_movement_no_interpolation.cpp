#include <iostream>
#include <string>
#include <cmath>
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <algorithm>
#include <list>
#include <fstream>
#include <filesystem>
#include <chrono>  // for high_resolution_clock
using namespace std;



void save_to_csv(const vector<float>& time, const vector<float>& data, const string& filename) {
    ofstream file(filename);
    for (size_t i = 0; i < time.size(); ++i)
        file << time[i] << "," << data[i] << "\n";
}
class Source {
    private:
    double Amplitude;
    double frequency;
    int SampleRate;
    int bufferSize;

    public:
    //constructor
    Source(double amp, double freq, int sr, int bs)
    : Amplitude(amp), frequency(freq), SampleRate(sr), bufferSize(bs) {}

    vector<float> buffer_source() const {

        vector<float> source(bufferSize);
        for (int n=0; n<bufferSize; n++){
            source[n] = Amplitude * sin(2.0 * M_PI * frequency * n / SampleRate);
        }
        return source;     
        }

    vector<float> time_source() const {
        vector<float> time_s(bufferSize);
        for (int n=0; n<bufferSize; n++){
            time_s[n] = static_cast<float>(n) / static_cast<float>(SampleRate);  
        } 
        return time_s;  
    } 



    double getSampleRate() const{
        return SampleRate;
    }

    double getBufferSize() const{
        return bufferSize;
    }
};

class Environment {
    private:
    double x_source;
    double x_left;
    double x_right;
    double y_source;
    double y_left;
    double y_right;
    double z_source;
    double z_left;
    double z_right;
    double v;

    public:
    //constructor
    Environment(double xs, double xl, double xr, double ys, double yl, double yr, double zs, double zl, double zr, double speed)
    : x_source(xs), x_left(xl), x_right(xr), y_source(ys), y_left(yl),
    y_right(yr), z_source(zs), z_left(zl), z_right(zr), v(speed) {}


    //LINEAR INTERPOLATION FOR SIMULATING CONTINUOUS MOVEMENT

    pair<double,double> DelayTime() const{

        double delayLeft = sqrt((x_left - x_source)*(x_left - x_source) + (y_left - y_source)*(y_left - y_source) +
        (z_left - z_source)*(z_left - z_source))/v;
        double delayRight = sqrt((x_right - x_source)*(x_right - x_source) + (y_right - y_source)*(y_right - y_source) +
        (z_right - z_source)*(z_right - z_source))/v;  
        return make_pair(delayLeft, delayRight);
    }

    pair<double,double> DelaySample(const Source& src) const{
        auto [delayLeft, delayRight] = DelayTime();
        double delaySampleLeft = delayLeft*src.getSampleRate();
        double delaySampleRight = delayRight*src.getSampleRate();      
        return make_pair(delaySampleLeft, delaySampleRight);
    }

    // --------------------------------------

    double get_x_left() const{
        return x_left;
    }

    double get_x_right() const{
        return x_right;
    }
};

class Microphones {
    private:
       // vector<float> micLeftTime; 
       // vector<float> micRightTime; 
        vector<float> micLeft;
        vector<float> micRight; 

    public:
    pair<vector<float>, vector<float>> capture(const Source& src, const Environment& env) {
        auto [delaySampleLeft, delaySampleRight] = env.DelaySample(src);
        auto buffer = src.buffer_source();
        micLeft.resize(src.getBufferSize() + delaySampleLeft);
        micRight.resize(src.getBufferSize() + delaySampleRight); 
        //int totalLength = src.getBufferSize() + max(delaySampleLeft, delaySampleRight);
        for (int n=0; n<micLeft.size(); n++){
            if ((n-delaySampleLeft) < 0) {
                micLeft[n] = 0;
            }
            else {
                micLeft[n] = buffer[n-delaySampleLeft];
               // micLeftTime[n] = static_cast<float>(n-delaySampleLeft)/static_cast<float>(src.getSampleRate());
            }
        }
        for (int n=0; n<micRight.size(); n++){
            if ((n-delaySampleRight) < 0) {
                micRight[n] = 0;
               // micRightTime[n] = 0;
            }
            else {
                micRight[n] = buffer[n-delaySampleRight];
               // micRightTime[n] = static_cast<float>(n-delaySampleRight)/static_cast<float>(src.getSampleRate());
            }
        }
        return make_pair(micLeft, micRight);
        }
    };


int main(){
    // Record start time
    auto start = std::chrono::high_resolution_clock::now();


    int base_buffer_size = 512;
    int sample_rate = 44100;
    //If T = 0.5s, a = 343 / 0.5 = 686 m/s^2
    float acceleration = 686.0; //acceleration in m/s^2, we can adjust this value to simulate different speeds of movement for the microphones. A higher acceleration will result in a faster movement, while a lower acceleration will result in a slower movement. We can choose an appropriate value for the acceleration based on the desired speed of movement for the microphones in our simulation.


    // 2. Calculate the long duration
    //float total_time = (static_cast<float>(base_buffer_size) / sample_rate)*5;
    float total_time = 0.5; //microphones reach speed of sound in the air at t=0.5s, we can set total_time to 1s to capture the movement of the microphones as they accelerate from rest to the speed of sound and then continue moving at that speed for a short duration, allowing us to observe the changes in the captured sound over time as the microphones move through space.
    int total_samples = total_time * sample_rate;


    Source src(0.5, 440.0, 44100, total_samples); //we use total_samples here to ensure that the source signal covers the entire duration of the movement, including the offsets between bursts, allowing us to capture the continuous movement of the microphones and the corresponding changes in the captured sound over time.
    Environment env1(0, -1.0, 1.0, 0, 0, 0, 0, 0, 0, 343); //for now change x over constant y and z
    Environment env2(0, -3.0, 6.0, 0, 0, 0, 0, 0, 0, 343);
    Microphones mic;
    auto buffer = src.buffer_source();
    //float offset = 0.03; //offset of 30ms to ensure we have enough samples for the delay at the beginning of the signal
    //int nr_positions = 2;
    float y_n_left, y_n_right; //we will use these variables to store the interpolated values for each sample at the left and right microphones, which we will then save to CSV files. We declare them outside the loop so that they can be updated and used for each sample as we iterate through the time axis of the signal, allowing us to capture the continuous movement of the microphones and the corresponding changes in the captured sound over time.


    //compute velocity_left and velocity_right throughout total_time
    float velocity_left = ((env2.get_x_left()-env1.get_x_left())/total_time);  //x_start = env1.get_x_left() and x_end = env3.get_x_left()
    float velocity_right = ((env2.get_x_right()-env1.get_x_right())/total_time);  //x_start = env1.get_x_right() and x_end = env3.get_x_right()
    cout << "Velocity Left: " << velocity_left << " m/s" << endl;
    cout << "Velocity Right: " << velocity_right << " m/s" << endl;

    //save csv files

    ofstream src_file("./source.csv");


        for (int i=0; i<total_samples; i++){
            float time_source = static_cast<float>(i)/static_cast<float>(src.getSampleRate());
            src_file << time_source << "," << buffer[i] << "\n";
            };


    src_file.close();


    string filenames[2] = {"./mic_left.csv", "./mic_right.csv"};
    ofstream ofs;



    //left_file

    vector<float> time_left_storage;
    vector<float> amplitude_left_storage;


            for (int i=0; i<total_samples; i++){
                float global_time = static_cast<float>(i)/static_cast<float>(src.getSampleRate());
                //float x_mic_left = env1.get_x_left() + velocity_left*global_time; //uniform velocity

                //use initial velocity=0
                float x_mic_left = env1.get_x_left() + 0.5*acceleration*global_time*global_time; //with acceleration, we can use the formula: x = x0 + v0*t + 0.5*a*t^2, where x0 is the initial position (env1.get_x_left()), v0 is the initial velocity (which is 0 in this case), a is the acceleration, and t is the global_time. This will give us the position of the left microphone at each time step, taking into account the acceleration of the movement.
                Environment env_moving(0, x_mic_left, 0, 0, 0, 0, 0, 0, 0, 343);
                auto [delayLeft, _] = env_moving.DelayTime();
                float time_left = global_time - delayLeft; //we subtract the delay from the global time to get the birth time of the sample that we want to capture at the current global time, which is the time when the sound was emitted from the source and started traveling towards the microphone. This is because we want to find out which sample from the source signal corresponds to the sound that is being captured by the microphone at the current global time, taking into account the time it takes for the sound to travel from the source to the microphone (the delay). By subtracting the delay from the global time, we can determine the birth time of that sample in the source signal, which allows us to perform interpolation and capture the correct amplitude value for that sample at the current global time.

                //we won't use interpolation for this version, we will just capture the sample that corresponds to the birth time of the sound at the microphone without performing interpolation, which means we will capture the amplitude value of the sample that is closest to the birth time without considering the fractional part of the delay. This will give us a more discrete representation of the captured sound at the microphone as it moves through space, without smoothing out the changes in amplitude that occur due to the movement and the corresponding changes in delay.
                float n_float_left = time_left * src.getSampleRate();
                int M_left = static_cast<int>(round(n_float_left));

                if (M_left >= 0 && (M_left + 1) < buffer.size()) {
                y_n_left = buffer[M_left];
                time_left_storage.push_back(global_time);
                amplitude_left_storage.push_back(y_n_left);
                }
            };
        //};

            ofs.open(filenames[0]);
            for (size_t i = 0; i < time_left_storage.size(); i++) {
                ofs << time_left_storage[i] << "," << amplitude_left_storage[i] << "\n"; //we use global_time here because microphone that is recording at a fixed rate, so we want to keep the time axis consistent with the source signal, which is also sampled at a fixed rate. The delay is already accounted for in the calculation of y_n_left, so we don't need to add it to the time axis when saving to CSV.
            }

        ofs.close();
        cout << "Saved mic_left.csv with simulated movement!" << endl;


    //right_file
    vector<float> time_right_storage;
    vector<float> amplitude_right_storage;


            for (int i=0; i<total_samples; i++){
                float global_time = static_cast<float>(i)/static_cast<float>(src.getSampleRate());
                //float x_mic_left = env1.get_x_left() + velocity_left*global_time; //uniform velocity

                //use initial velocity=0
                float x_mic_right = env1.get_x_right() + 0.5*acceleration*global_time*global_time; //with acceleration, we can use the formula: x = x0 + v0*t + 0.5*a*t^2, where x0 is the initial position (env1.get_x_left()), v0 is the initial velocity (which is 0 in this case), a is the acceleration, and t is the global_time. This will give us the position of the left microphone at each time step, taking into account the acceleration of the movement.
                Environment env_moving(0, 0, x_mic_right, 0, 0, 0, 0, 0, 0, 343);
                auto [_, delayRight] = env_moving.DelayTime();
                float time_right = global_time - delayRight; //we subtract the delay from the global time to get the birth time of the sample that we want to capture at the current global time, which is the time when the sound was emitted from the source and started traveling towards the microphone. This is because we want to find out which sample from the source signal corresponds to the sound that is being captured by the microphone at the current global time, taking into account the time it takes for the sound to travel from the source to the microphone (the delay). By subtracting the delay from the global time, we can determine the birth time of that sample in the source signal, which allows us to perform interpolation and capture the correct amplitude value for that sample at the current global time.

                //we won't use interpolation for this version, we will just capture the sample that corresponds to the birth time of the sound at the microphone without performing interpolation, which means we will capture the amplitude value of the sample that is closest to the birth time without considering the fractional part of the delay. This will give us a more discrete representation of the captured sound at the microphone as it moves through space, without smoothing out the changes in amplitude that occur due to the movement and the corresponding changes in delay.
                float n_float_right = time_right * src.getSampleRate();
                int M_right = static_cast<int>(round(n_float_right));

                if (M_right >= 0 && (M_right + 1) < buffer.size()) {
                y_n_right = buffer[M_right];
                time_right_storage.push_back(global_time);
                amplitude_right_storage.push_back(y_n_right);
                }
            };
        //};

            ofs.open(filenames[1]);
            for (size_t i = 0; i < time_right_storage.size(); i++) {
                ofs << time_right_storage[i] << "," << amplitude_right_storage[i] << "\n"; //we use global_time here because microphone that is recording at a fixed rate, so we want to keep the time axis consistent with the source signal, which is also sampled at a fixed rate. The delay is already accounted for in the calculation of y_n_left, so we don't need to add it to the time axis when saving to CSV.
            }

        ofs.close();
        cout << "Saved mic_right.csv with simulated movement!" << endl;
    


    cout << "Saved all CSVs in: " << filesystem::current_path() << endl;

    // Record end time
    auto finish = std::chrono::high_resolution_clock::now();

    // Calculate duration
    // Use microseconds (us) if the code runs very fast, or milliseconds (ms) for longer simulations
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);

    cout << "Execution time: " << duration.count() << " milliseconds" << endl;
    return 0;
    }
