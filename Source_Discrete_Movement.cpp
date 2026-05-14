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
    //2 moving sources
    private:
    double Amplitude;
    double frequency;
    int SampleRate;
    int bufferSize;


    public:
    //constructor
    Source(double amp, double freq, int sr, int bs)
    : Amplitude(amp), frequency(freq), SampleRate(sr), bufferSize(bs){}

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
    double x_source1;
    double x_source2;
    double x_mic;
    double y_source1;
    double y_source2;
    double y_mic;
    double z_source1;
    double z_source2;
    double z_mic;
    double v;

    public:
    //constructor
    Environment(double xs1, double xs2, double xmic, double ys1, double ys2, double ymic, double zs1, double zs2, double zmic, double speed)
    : x_source1(xs1), x_source2(xs2), x_mic(xmic), y_source1(ys1), y_source2(ys2), y_mic(ymic), z_source1(zs1), z_source2(zs2), z_mic(zmic), v(speed) {}

    pair<double,double> DelayTime() const{
        double delay1 = sqrt((x_mic - x_source1)*(x_mic - x_source1) + (y_mic - y_source1)*(y_mic - y_source1) +
        (z_mic - z_source1)*(z_mic - z_source1))/v;
        double delay2 = sqrt((x_mic - x_source2)*(x_mic - x_source2) + (y_mic - y_source2)*(y_mic - y_source2) +
        (z_mic - z_source2)*(z_mic - z_source2))/v;  
        return make_pair(delay1, delay2);
    }

    pair<int,int> DelaySample(const Source& src) const{
        auto [delay1, delay2] = DelayTime();
        int delaySample1 = delay1*src.getSampleRate();
        int delaySample2 = delay2*src.getSampleRate();      
        return make_pair(delaySample1, delaySample2);
    }

    double get_x1() const{
        return x_source1;
    }

    double get_x2() const{
        return x_source2;
    }

    double get_x_mic() const{
        return x_mic;
    }
};

class Microphones {
    //a single stationary microphone
    public:
    vector<float> capture(const Source& src, const Environment& env) {
        auto [delaySample1, delaySample2] = env.DelaySample(src);
        auto buffer = src.buffer_source();
        int microphone_resize = src.getBufferSize() + max(delaySample1, delaySample2);
        vector<float> microphone(microphone_resize, 0.0f); //initialize microphone signal with zeros, size is source buffer size + max delay in samples
        //microphone signal is the sum of the two sources, each delayed by their respective delay
        for (int n=0; n<microphone.size(); n++){
            if ((n-delaySample1) < 0) {
                microphone[n] += 0;
            }
            else {
                microphone[n] += buffer[n-delaySample1];
            }
        }
        for (int n=0; n<microphone.size(); n++){
            if ((n-delaySample2) < 0) {
                microphone[n] += 0;
            }
            else {
                microphone[n] += buffer[n-delaySample2];
            }
        }
        return microphone;
    }
    };


int main(){
    //let's simulate the discretized movement of the sources throughout 3 different x positions
    Source src(0.5, 440.0, 44100, 512);
    Environment env1(-1.0, 1.0, 0, 0, 0, 0, 0, 0, 0, 343); //for now change x over constant y and z
    Environment env2(-3.0, 5.0, 0, 0, 0, 0, 0, 0, 0, 343);
    Environment env3(-5.0, 12.0, 0, 0, 0, 0, 0, 0, 0, 343);
    Microphones mic;
    auto buffer = src.buffer_source();
    auto buffer_time = src.time_source();
    env1.DelayTime();
    env2.DelayTime();
    env3.DelayTime();
    env1.DelaySample(src);
    env2.DelaySample(src);
    env3.DelaySample(src);
    auto mic1 = mic.capture(src, env1);
    auto mic2 = mic.capture(src, env2);
    auto mic3 = mic.capture(src, env3);
    cout << "Samples at microphone in the 1st interval: " << mic1.size() << endl;
    cout << "Samples at microphone in the 2nd interval: " << mic2.size() << endl;
    cout << "Samples at microphone in the 3rd interval: " << mic3.size() << endl;


    // compute velocity_1 and velocity_2 (from the sources) throughout total_time
    float total_time = static_cast<float>(buffer.size())/static_cast<float>(src.getSampleRate());
    float velocity_1 = ((env3.get_x1()-env1.get_x1())/total_time);  //x_start = env1.get_x1() and x_end = env3.get_x1()
    float velocity_2 = ((env3.get_x2()-env1.get_x2())/total_time);  //x_start = env1.get_x2() and x_end = env3.get_x2()


    // Combine the mic into one long signal (simulating motion)
    vector<float> mic_all;
    mic_all.insert(mic_all.end(), mic1.begin(), mic1.end());
    mic_all.insert(mic_all.end(), mic2.begin(), mic2.end());
    mic_all.insert(mic_all.end(), mic3.begin(), mic3.end());

    //Create Reference Source CSV (Time, X1, X2, Audio)
    ofstream ofs_src1("./source1_moving.csv");
    ofstream ofs_src2("./source2_moving.csv");

    //Combine the sources into one long signal (simulating motion)
    vector<float> source1_all;
    vector<float> source2_all;
    source1_all.reserve(mic_all.size());
    source2_all.reserve(mic_all.size());

    auto src_buffer = src.buffer_source();
    auto buffer_size = src.getBufferSize();


    for (size_t i=0; i<mic_all.size(); i++){
        float current_time = static_cast<float>(i) / static_cast<float>(src.getSampleRate());
        double current_x1, current_x2;

        if (i < mic1.size()) {
            current_x1 = env1.get_x1();
            current_x2 = env1.get_x2();
        }

        else if (i < mic1.size() + mic2.size()) {
            current_x1 = env2.get_x1();
            current_x2 = env2.get_x2();
        }
        else {
            current_x1 = env3.get_x1();
            current_x2 = env3.get_x2();
        }

        //define only one audio source since they both have the same frequency and amplitude, so they only differ in their position and thus in their delay at the microphone
         float audio_source = src_buffer[i % static_cast<int>(buffer_size)]; //get the audio sample from the source buffer, using modulo to loop through the source buffer
         source1_all.push_back(audio_source);
         source2_all.push_back(audio_source);

        ofs_src1 << current_time << "," << source1_all[i] << "\n"; //save time and x position of source 1 to csv file
        ofs_src2 << current_time << "," << source2_all[i] << "\n"; //save time and x position of source 2 to csv file
    }
    ofs_src1.close();
    ofs_src2.close();


    //Create Microphone Result CSV (Time, Mic_Amplitude)
    ofstream ofs_mic("./mic_combined_movement.csv");
    // Make a time axis for plotting
    for (size_t i = 0; i < mic_all.size(); ++i) {
        float time_val = static_cast<float>(i) / static_cast<float>(src.getSampleRate());
        ofs_mic << time_val << "," << mic_all[i] << "\n";
    }
    ofs_mic.close();

    cout << "Saved mic_combined_movement.csv with two moving sources!" << endl;
    return 0;
    }
