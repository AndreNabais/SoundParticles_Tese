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
    /*

    vector<float> getLeftTime() const {
        return micLeftTime;
    }
    vector<float> getRightTime() const {
        return micRightTime;
    }
};

*/

int main(){
    //let's simulate the continuous movement of the microphones throughout 2 different x positions
    Source src(0.5, 440.0, 44100, 512);
    double x_source = 0.0; //position of the source
    double c = 343.0; //speed of sound in m/s
    double x0_left = -1.0; //initial position of the left mic
    double x0_right = 1.0; //initial position of the right mic
    double v_left = 0.5; //velocity of the left mic in m/s
    double v_right = 0.5; //velocity of the right mic in m/s
    
    auto buffer = src.buffer_source();
    auto buffer_time = src.time_source();  
    int N = buffer.size();
    double fs = src.getSampleRate();

    vector<float> micLeft_all(N);
    vector<float> micRight_all(N);

    for (size_t n=0; n<N; n++){
        //time for each sample
        float total_time = static_cast<float>(n) / static_cast<float>(fs);

        //position of the microphones at each time step
        float x_mic_left = x0_left + v_left*total_time;
        float x_mic_right = x0_right + v_right*total_time;

        //distance from the source to each microphone
        float d_left = abs(x_mic_left - x_source);
        float d_right = abs(x_mic_right - x_source);

        //time delay for each microphone
        float delay_left = d_left / c;
        float delay_right = d_right / c;

        //fractional delay interpolation



        /*
        int delaySampleLeft = static_cast<int>(delay_left * fs);
        int delaySampleRight = static_cast<int>(delay_right * fs);

        if ((n-delaySampleLeft) < 0) {
            micLeft_all[n] = 0;
            }
        else {
            micLeft_all[n] = buffer[n-delaySampleLeft];
            }
        if ((n-delaySampleRight) < 0) {
            micRight_all[n] = 0;
            }
        else {
            micRight_all[n] = buffer[n-delaySampleRight];
            }
    }
    */

        float fractional_delay_left = delay_left * fs - floor(delay_left * fs);
        float fractional_delay_right = delay_right * fs - floor(delay_right * fs);

        int delaySampleLeft = static_cast<int>(floor(delay_left * fs));
        int delaySampleRight = static_cast<int>(floor(delay_right * fs));

        if ((n-delaySampleLeft) < 0) {
            micLeft_all[n] = 0;
            }
        else {
            micLeft_all[n] = (1 - fractional_delay_left) * buffer[n-delaySampleLeft] + fractional_delay_left * buffer[n-delaySampleLeft-1];
            }
        if ((n-delaySampleRight) < 0) {
            micRight_all[n] = 0;
            }
        else {
            micRight_all[n] = (1 - fractional_delay_right) * buffer[n-delaySampleRight] + fractional_delay_right * buffer[n-delaySampleRight-1];
            }
    }


    //save csv files

    ofstream src_file("./source.csv");
    for (size_t i = 0; i < buffer.size(); ++i) {
        float time_source = static_cast<float>(i)/static_cast<float>(src.getSampleRate());
         src_file << time_source << "," << buffer[i] << "\n";       
        };

    src_file.close();


    // Make a time axis for plotting
    vector<float> time_left(micLeft_all.size());
    for (size_t i = 0; i < time_left.size(); ++i)
        time_left[i] = static_cast<float>(i) / static_cast<float>(src.getSampleRate());

    vector<float> time_right(micRight_all.size());
    for (size_t i = 0; i < time_right.size(); ++i)
        time_right[i] = static_cast<float>(i) / static_cast<float>(src.getSampleRate());

    string filenames[2] = {"./mic_left_moving.csv", "./mic_right_moving.csv"};
    ofstream ofs;

        ofstream left_file("./mic_left_moving.csv");
        ofstream right_file("./mic_right_moving.csv");

        for (size_t i = 0; i < micLeft_all.size(); ++i) {
            left_file << time_left[i] << "," << micLeft_all[i] << "\n";
        }
        left_file.close();

        for (size_t i = 0; i < micRight_all.size(); ++i) {
            right_file << time_right[i] << "," << micRight_all[i] << "\n";
        }
        right_file.close();

    cout << "Saved all CSVs in: " << filesystem::current_path() << endl;
    return 0;
    }

    //TO FIX!!!!!!!!!!!
