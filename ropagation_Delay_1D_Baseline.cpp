#include <iostream>
#include <string>
#include <cmath>
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <algorithm>
#include <list>
#include <fstream>
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
    double v;

    public:
    //constructor
    Environment(double xs, double xl, double xr, double speed)
    : x_source(xs), x_left(xl), x_right(xr), v(speed) {}

    pair<double,double> DelayTime() const{
        double delayLeft = abs(x_left - x_source)/v;
        double delayRight = abs(x_right - x_source)/v;   
        return make_pair(delayLeft, delayRight);
    }

    pair<int,int> DelaySample(const Source& src) const{
        auto [delayLeft, delayRight] = DelayTime();
        int delaySampleLeft = delayLeft*src.getSampleRate();
        int delaySampleRight = delayRight*src.getSampleRate();      
        return make_pair(delaySampleLeft, delaySampleRight);
    }
};

class Microphones {

    public:
    pair<vector<float>, vector<float>> capture(const Source& src, const Environment& env) {
        auto [delaySampleLeft, delaySampleRight] = env.DelaySample(src);
        auto buffer = src.buffer_source();
        vector<float> micLeft(src.getBufferSize() + delaySampleLeft);
        vector<float> micRight(src.getBufferSize() + delaySampleRight);
        double totalLength = src.getBufferSize() + max(delaySampleLeft, delaySampleRight);
        for (int n=0; n<totalLength; n++){
            if ((n-delaySampleLeft) < 0) {
                micLeft[n] = 0;
            }
            else {
                micLeft[n] = buffer[n-delaySampleLeft];
            }
            if ((n-delaySampleRight) < 0) {
                micRight[n] = 0;
            }
            else {
                micRight[n] = buffer[n-delaySampleRight];
            }
            }
        return make_pair(micLeft, micRight);
    }
};

int main(){
    Source src(0.5, 440.0, 44100, 512);
    Environment env(0, -1.0, 1.0, 343);
    Microphones mic;
    src.buffer_source();
    env.DelayTime();
    env.DelaySample(src);
    auto [micLeft, micRight] = mic.capture(src, env);
    vector<float> buffer = src.buffer_source();
    cout << "Samples at microphone on the left: " << micLeft.size() << endl;
    cout << "Samples at microphone on the right: " << micRight.size() << endl;
    

        //save csv files

    ofstream src_file("./source.csv");


        for (int i=0; i<buffer.size(); i++){
            float time_source = static_cast<float>(i)/static_cast<float>(src.getSampleRate());
            src_file << time_source << "," << buffer[i] << "\n";
            };


    src_file.close();


    string filenames[2] = {"./mic_left.csv", "./mic_right.csv"};
    ofstream ofs;

            ofs.open(filenames[0]);
            for (size_t i = 0; i < micLeft.size(); i++) {
                float time_left = static_cast<float>(i)/static_cast<float>(src.getSampleRate());
                ofs << time_left << "," << micLeft[i] << "\n"; //we use global_time here because microphone that is recording at a fixed rate, so we want to keep the time axis consistent with the source signal, which is also sampled at a fixed rate. The delay is already accounted for in the calculation of y_n_left, so we don't need to add it to the time axis when saving to CSV.
            }

        ofs.close();
        cout << "Saved mic_left.csv with simulated movement!" << endl;



        ofs.open(filenames[1]);
            for (size_t i = 0; i < micRight.size(); i++) {
                float time_right = static_cast<float>(i)/static_cast<float>(src.getSampleRate());
                ofs << time_right << "," << micRight[i] << "\n"; //we use global_time here because microphone that is recording at a fixed rate, so we want to keep the time axis consistent with the source signal, which is also sampled at a fixed rate. The delay is already accounted for in the calculation of y_n_right, so we don't need to add it to the time axis when saving to CSV.
            }

        ofs.close();
        cout << "Saved mic_right.csv with simulated movement!" << endl;

        return 0;
    

}
