#pragma once

/** \defgroup Tools
 *
 * \brief Basic Toolbox for Audio
 *
 * This file contains basic tools, one might consider useful,
 * such as signal generators and data plotters
 *
*/

#include <iostream>
#include <cmath>
#include <limits>
#include <fstream>
#include <cstdio>

#include "samplespecs.h"

namespace Nl {

int requestRealtime(void);

/** \ingroup Tools
 *
 * \brief Store a buffer with samples to a csv file.
 * \param buffer Buffer with samples.
 * \param buffersize Buffersize in bytes.
 * \param path Path to store the data to
 *
 * Store the content of a buffer to a coma separated files
 *
 * \code{.cpp}
 * Nl::sinewave<double>(buffer, frequency, false, sampleSpecs);
 * Nl::store<double>(buffer, sampleSpecs.buffersizeInBytes, "./sampleData.txt");
 * \endcode
*/
template<typename T>
void store(const T *buffer, unsigned int buffersize, const std::string& path)
{
	unsigned int i = 0;

    std::ofstream out;
	out.open(path, std::ofstream::out /*| std::ofstream::app*/);

    for (i=0; i<buffersize; i++)
		out << i << "," << buffer[i] << std::endl;

    out.flush();
    out.close();
}

/** \ingroup Tools
 *
 * \brief Generate a sine fragment with a given frequency.
 * \tparam T Defines the type of the generated samples. For uint8_t: sin(0) = 127 and sin(pi/2) = 255 and for int8_t: sin(0) = 0 and sin(pi/2) = 127
 * \param buffer Buffer to store samples to
 * \param frequency Desired frequency
 * \param reset This must be set true, if sampleSpecs or frequency have changed since last call.
 * \param sampleSpecs Containing all necessary sampleinformtaion.
 *
 * \code{.cpp}
 * Nl::sinewave<int32_t>(buffer, frequency, false, sampleSpecs);
 * \endcode
*/
template<typename T>
inline void sinewave(T *buffer, float frequency, bool reset, const SampleSpecs& sampleSpecs)
{
	static T scale = std::numeric_limits<T>::is_signed ?
				((1 << (sampleSpecs.bytesPerSample * 8)) / 2) :
				(1 << (sampleSpecs.bytesPerSample * 8));

	static T offset = !std::numeric_limits<T>::is_signed ?
				(1 << (sampleSpecs.bytesPerSample * 8) / 2) :
				0;

    static float phase = 0.f;
	static float inc = frequency / static_cast<float>(sampleSpecs.samplerate);

    if (reset) {
        phase = 0.f;
		inc = frequency / static_cast<float>(sampleSpecs.samplerate);
	}

	for (unsigned int i=0; i<sampleSpecs.buffersizeInFramesPerPeriode; i++) {
        phase += inc;

	   if (phase > 0.5) phase -= 1.0;

#if 1
        float x = 2 * phase;

		if (x < 0) {
			x = -x;
		}

		x = 0.5 - x;

        float x_square = x * x;
		x = x * (x_square * (x_square * 2.26548 - 5.13274) + 3.14159);
		buffer[i] = static_cast<T>(x * scale + offset);
#else
		buffer[i] = sin(2.f * M_PI * phase) * scale + offset;
#endif
	}
}

/** \ingroup Tools
 *
 * \brief Generate a sine fragment with a given frequency. Specialized template for double
 * \param buffer Buffer to store samples to
 * \param frequency Desired frequency
 * \param reset This must be set true, if sampleSpecs or frequency have changed since last call.
 * \param sampleSpecs Containing all necessary sampleinformtaion.
 *
 * \code{.cpp}
 * Nl::sinewave<double>(buffer, frequency, false, sampleSpecs);
 * \endcode
*/
template<>
inline void sinewave<double>(double *buffer, float frequency, bool reset, const SampleSpecs& sampleSpecs)
{
	static double phase = 0.f;
	static double inc = frequency / static_cast<double>(sampleSpecs.samplerate);

	if (reset) {
		phase = 0.f;
		inc = frequency / static_cast<double>(sampleSpecs.samplerate);
	}

	for (unsigned int i=0; i<sampleSpecs.buffersizeInFramesPerPeriode; i++) {
		phase += inc;

	   if (phase > 0.5) phase -= 1.0;

		double x = 2 * phase;
		if (x < 0) x = -x;
		x = 0.5 - x;

		double x_square = x * x;
		buffer[i] = x * (x_square * (x_square * 2.26548 - 5.13274) + 3.14159);
	}
}

/** \ingroup Tools
 *
 * \brief Generate a sine fragment with a given frequency. Specialized template for float
 * \param buffer Buffer to store samples to
 * \param frequency Desired frequency
 * \param reset This must be set true, if sampleSpecs or frequency have changed since last call.
 * \param sampleSpecs Containing all necessary sampleinformtaion.
 *
 * \code{.cpp}
 * Nl::sinewave<float>(buffer, frequency, false, sampleSpecs);
 * \endcode
*/
template<>
inline void sinewave<float>(float *buffer, float frequency, bool reset, const SampleSpecs& sampleSpecs)
{
	static float phase = 0.f;
    float inc = frequency / static_cast<float>(sampleSpecs.samplerate);

	if (reset) {
		phase = 0.f;
		inc = frequency / static_cast<float>(sampleSpecs.samplerate);
	}

	for (unsigned int i=0; i<sampleSpecs.buffersizeInFramesPerPeriode; i++) {
		phase += inc;

	   if (phase > 0.5) phase -= 1.0;

		float x = 2 * phase;
		if (x < 0) x = -x;
		x = 0.5 - x;

		float x_square = x * x;
        buffer[i] = x * (x_square * (x_square * 2.26548 - 5.13274) + 3.14159);
    }
}

/** \ingroup Tools
 *
 * \brief Generate a complete sine from 0 to 2Pi with a given frequency.
 * \param buffer Buffer to store samples to
 * \param buffersize Size of buffer and therefore x resolution
 * \code{.cpp}
 * Nl::sinewave<int16_t>(buffer, 360);
 * \endcode
*/
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

/** \ingroup Tools
 *
 * \brief Generate a complete sine from 0 to 2Pi with a given frequency. Specialized template for double
 * \param buffer Buffer to store samples to
 * \param buffersize Size of buffer and therefore x resolution
 * \code{.cpp}
 * Nl::sinewave<double>(buffer, 360);
 * \endcode
*/
template<>
inline void sinewave<double>(double *buffer, unsigned int buffersize)
{
    for (unsigned int i=0; i<buffersize; i++) {
        buffer[i] = sin(2.f * M_PI * static_cast<double>(i) / static_cast<double>(buffersize));
    }
}

/** \ingroup Tools
 *
 * \brief Generate a complete sine from 0 to 2Pi with a given frequency. Specialized template for float
 * \param buffer Buffer to store samples to
 * \param buffersize Size of buffer and therefore x resolution
 * \code{.cpp}
 * Nl::sinewave<float>(buffer, 360);
 * \endcode
*/
template<>
inline void sinewave<float>(float *buffer, unsigned int buffersize)
{
    for (unsigned int i=0; i<buffersize; i++) {
        buffer[i] = sinf(2.f * M_PI * static_cast<float>(i) / static_cast<float>(buffersize));
    }
}

/** \ingroup Tools
 *
 *  \brief Calculate the sine of the incoming value (as often found in Reaktor)
 *  \param value
 *  \code{.cpp}
 *  Nl::sin<float>(val)
 *  \endcode
*/
template<typename T>
inline T sin(T val)
{
    T val_square = val * val;

    val = (((((val_square * -2.39f * pow(10.f, -8.f) + 2.7526f * pow(10.f, -6.f))
            * val_square + (0.198409f * pow(10.f, -3.f)))
           * val_square + 0.008333f)
          * val_square + (-0.166667f))
         * val_square + 1.f) * val;

    return val;
}

/** \ingroup Tools
 *
 *  \brief Calculate the cosine of the incoming value (as often found in Reaktor)
 *  \param value
 *  \code{.h}
 *  Nl::cos<float>(val)
 *  \endcode
*/
template<typename T>
inline T cos(T val)
{
    T val_square = val * val;

    val = (((((val_square * -2.605f * pow(10.f, -7.f) + 2.47609 * pow(10.f, -5.f))
            * val_square + (-0.00138884))
           * val_square + 0.0416666)
          * val_square + (-0.499923))
         * val_square) + 1.f;

    return val;
}

/** \ingroup Tools
 *
 *  \brief Calculate the tangent of the incoming value (as found in Reaktor)
 *  \param value
 *  \code{.h}
 *  Nl::tan<float>(val)
 *  \endcode
*/
template<typename T>
inline T tan(T val)
{
    val = 0.133333 * pow(val, 5.f) + 0.333333 * pow(val, 3.f) + val;

    return val;
}

/** @brief    calculation of dry and wet amounts depending on mix amount
 *  @param    raw Sample
 *  @param    processed sample
 *  @return   a mix of both samples
*/
template<typename T>
inline T db2af(T dbIn)
{
    return pow(1.12202f, dbIn);
}
} // namespace Nl
