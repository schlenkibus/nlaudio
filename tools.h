#pragma once

#include <iostream>
#include <cmath>
#include <limits>
#include <fstream>
#include <cstdio>

#define NOT_IMPLEMENTED { std::cout << "ALERT: " << __func__ << " not implemented, yet. Aborting!"; exit(-1); }


namespace Nl {

struct SampleSpecs_t {
    unsigned int samplerate;		/// Samplerate
    unsigned int channels;			/// Channels
    unsigned int bytesPerFrame;		/// = channels * bytesPerSample
    unsigned int buffersizeInFrames;		/// Size of buffer. This is relevant for latency
    unsigned int buffersizeInFramesPerPeriode;
    unsigned int buffersizeInBytes; ///
    unsigned int buffersizeInBytesPerPeriode;
    unsigned int buffersizeInSamples;
    unsigned int buffersizeInSamplesPerPeriode;
    unsigned int bytesPerSample;	/// with 24_BE3 this would be 3, with S16 this would be 2
    unsigned int bytesPerSamplePhysical; /// Sometimes 24_BE3 or so can be stored in 4Bytes
    bool isFloat;
    bool isLittleEndian;
    bool isSigned;
    // bool isInterleaved
};
std::ostream& operator<<(std::ostream& lhs, const SampleSpecs_t& rhs);

//TODO: This only works with interleaved samples
template<typename T>
void store(const T *buffer, unsigned int buffersize, const std::string& path)
{
    int i = 0;

    std::ofstream out;
    out.open(path, std::ofstream::out | std::ofstream::app);

    for (i=0; i<buffersize; i++)
        out << buffer[i] << std::endl;

    out.flush();
    out.close();
}

/// Type HickHack
template<typename T>
T typeCast()
{
    return T(0);
}

template<typename T>
inline T getTypeForBitlenght(const SampleSpecs_t& specs) {

    if (specs.isFloat)
        return typeCast<float>();

    // Signed
    if (specs.isSigned) {
        if (specs.bytesPerSample == 1)
            return typeCast<int8_t>();
        else if (specs.bytesPerFrame == 2)
            return typeCast<int16_t>();
        else if (specs.bytesPerSample == 3 || specs.bytesPerSample == 4)
            return typeCast<int32_t>();
        // Unsigned
    } else {
        if (specs.bytesPerSample == 1)
            return typeCast<uint8_t>();
        else if (specs.bytesPerFrame == 2)
            return typeCast<uint16_t>();
        else if (specs.bytesPerFrame == 3 || specs.bytesPerFrame == 4)
            return typeCast<uint32_t>();
    }

}

/// Signal generators
template<typename T>
inline void sinewave(T *buffer, unsigned int buffersize, float frequency, unsigned int samplerate, bool reset)
{
    static T scale = std::numeric_limits<T>::is_signed ?
                std::numeric_limits<T>::max() :
                std::numeric_limits<T>::max() / 2;

    static T offset = !std::numeric_limits<T>::is_signed ?
                std::numeric_limits<T>::max() / 2 :
                0;

    static float phase = 0.f;
    static float inc = frequency / static_cast<float>(samplerate);

    if (reset) {
        phase = 0.f;
        inc = frequency / static_cast<float>(samplerate);
    }

    for (int i=0; i<buffersize; i++) {
        phase += inc;

        if (phase > 0.5)
            phase -= 1.0;

        float x = 2 * phase;
        fabs(x);
        x = 0.5 - x;

        float x_square = x * x;
        x = x * (x_square * (x_square * 2.26548 - 5.13274) + M_PI);
        //buffer[i] = x * scale + offset;

        buffer[i] = sin(2.f * M_PI * phase) * scale + offset;

    }
}

//Specialize Template for floating point types
template<>
inline void sinewave<double>(double *buffer, unsigned int buffersize, float frequency, unsigned int samplerate, bool reset)
{
    static double phase = 0.f;
    static double inc = frequency / static_cast<double>(samplerate);

    if (reset) {
        phase = 0.f;
        inc = frequency / static_cast<double>(samplerate);
    }

    for (unsigned int i=0; i<buffersize; i++) {
        phase += inc;
        phase = (phase > 0.5 ? phase - 1.0 : phase);

        double x = 2 * phase;
        double x_square = x * x;
        buffer[i] = x * (x_square * (x_square * 2.26548 - 5.13274) + M_PI);
    }
}

template<>
inline void sinewave<float>(float *buffer, unsigned int buffersize, float frequency, unsigned int samplerate, bool reset)
{
    static float phase = 0.f;
    static float inc = frequency / static_cast<float>(samplerate);

    if (reset) {
        phase = 0.f;
        inc = frequency / static_cast<float>(samplerate);
    }

    for (unsigned int i=0; i<buffersize; i++) {
        phase += inc;
        phase = (phase > 0.5 ? phase - 1.0 : phase);

        float x = 2 * phase;
        float x_square = x * x;
        buffer[i] = x * (x_square * (x_square * 2.26548 - 5.13274) + M_PI);
    }
}

template<typename T>
inline void sinewave(T *buffer, unsigned int buffersize)
{
    T scale = std::numeric_limits<T>::is_signed ?
                std::numeric_limits<T>::max() :
                std::numeric_limits<T>::max() / 2;

    T offset = !std::numeric_limits<T>::is_signed ?
                std::numeric_limits<T>::max() / 2 :
                0;

    for (unsigned int i=0; i<buffersize; i++) {
        buffer[i] = sin(2.f * M_PI * static_cast<double>(i) / static_cast<double>(buffersize)) * scale + offset;
    }
}

//Specialize Template for floating point types
template<>
inline void sinewave<double>(double *buffer, unsigned int buffersize)
{
    for (unsigned int i=0; i<buffersize; i++) {
        buffer[i] = sin(2.f * M_PI * static_cast<double>(i) / static_cast<double>(buffersize));
    }
}

template<>
inline void sinewave<float>(float *buffer, unsigned int buffersize)
{
    for (unsigned int i=0; i<buffersize; i++) {
        buffer[i] = sinf(2.f * M_PI * static_cast<float>(i) / static_cast<float>(buffersize));
    }
}

} // namespace Nl
