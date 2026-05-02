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

    pair<double,double> DelayTime() const{
        double delayLeft = sqrt((x_left - x_source)*(x_left - x_source) + (y_left - y_source)*(y_left - y_source) +
        (z_left - z_source)*(z_left - z_source))/v;
        double delayRight = sqrt((x_right - x_source)*(x_right - x_source) + (y_right - y_source)*(y_right - y_source) +
        (z_right - z_source)*(z_right - z_source))/v;  
        return make_pair(delayLeft, delayRight);
    }

    pair<int,int> DelaySample(const Source& src) const{
        auto [delayLeft, delayRight] = DelayTime();
        int delaySampleLeft = delayLeft*src.getSampleRate();
        int delaySampleRight = delayRight*src.getSampleRate();      
        return make_pair(delaySampleLeft, delaySampleRight);
    }

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
    //let's simulate the discretized movement of the microphones throughout 3 different x positions
    Source src(0.5, 440.0, 44100, 512);
    Environment env1(0, -1.0, 1.0, 0, 0, 0, 0, 0, 0, 343); //for now change x over constant y and z
    Environment env2(0, -3.0, 5.0, 0, 0, 0, 0, 0, 0, 343);
    Environment env3(0, -5.0, 12.0, 0, 0, 0, 0, 0, 0, 343);
    Microphones mic;
    auto buffer = src.buffer_source();
    float offset = 0.03; //offset of 30ms to ensure we have enough samples for the delay at the beginning of the signal
    int nr_positions = 3;


    //compute velocity_left and velocity_right throughout total_time
    float total_time = nr_positions*(static_cast<float>(buffer.size())/static_cast<float>(src.getSampleRate())) + (nr_positions-1)*offset; //total time of the signal including the offset between bursts
    float velocity_left = ((env3.get_x_left()-env1.get_x_left())/total_time);  //x_start = env1.get_x_left() and x_end = env3.get_x_left()
    float velocity_right = ((env3.get_x_right()-env1.get_x_right())/total_time);  //x_start = env1.get_x_right() and x_end = env3.get_x_right()


    //save csv files

    ofstream src_file("./source.csv");
    for (size_t n = 0; n < nr_positions; n++) {    
        float burst_start_time = n * (static_cast<float>(buffer.size())/static_cast<float>(src.getSampleRate())+offset);
        for (size_t i = 0; i < buffer.size(); ++i) {
            float time_source = static_cast<float>(i)/static_cast<float>(src.getSampleRate()) + burst_start_time;
            src_file << time_source << "," << buffer[i] << "\n";
            };
            
        };

    src_file.close();


    string filenames[2] = {"./mic_left.csv", "./mic_right.csv"};
    ofstream ofs;



    //left_file
    ofs.open(filenames[0]);

        for (size_t n = 0; n < nr_positions; n++) {    
            float burst_start_time = n * (static_cast<float>(buffer.size())/static_cast<float>(src.getSampleRate())+offset);

            for (size_t i = 0; i < buffer.size(); ++i) {
                float global_time = static_cast<float>(i)/static_cast<float>(src.getSampleRate()) + burst_start_time;
                float x_mic_left = env1.get_x_left() + velocity_left*global_time;
                Environment env_moving(0, x_mic_left, 0, 0, 0, 0, 0, 0, 0, 343);
                auto [delayLeft, _] = env_moving.DelayTime();
                float time_left = delayLeft + global_time;
                float amplitude_left = buffer[i];

                ofs << time_left << "," << amplitude_left << "\n"; 
                };
            };
        ofs.close();
        cout << "Saved mic_left.csv with simulated movement!" << endl;

        //right_file
    ofs.open(filenames[1]);


            for (size_t n = 0; n < nr_positions; n++) {    
            float burst_start_time = n * (static_cast<float>(buffer.size())/static_cast<float>(src.getSampleRate())+offset);

            for (size_t i = 0; i < buffer.size(); ++i) {
                float global_time = static_cast<float>(i)/static_cast<float>(src.getSampleRate()) + burst_start_time;
                float x_mic_right = env1.get_x_right() + velocity_right*global_time;
                Environment env_moving(0, 0, x_mic_right, 0, 0, 0, 0, 0, 0, 343);
                auto [_, delayRight] = env_moving.DelayTime();
                float time_right = delayRight + global_time;
                float amplitude_right = buffer[i];

                ofs << time_right << "," << amplitude_right << "\n"; 
                };
            };
        ofs.close();
        cout << "Saved mic_right.csv with simulated movement!" << endl;


    


    cout << "Saved all CSVs in: " << filesystem::current_path() << endl;
    return 0;
    }
