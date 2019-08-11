#include "noise_generator.h"
#include <math.h>

noise_generator::noise_generator()
{
    m_ready=false;
}

noise_generator::noise_generator(float persistance,float frequency,float amplitude,int number_of_octaves,int seed)
{
    m_number_of_octaves=number_of_octaves;
    m_persistance=persistance;
    m_frequency=frequency;
    m_amplitude=amplitude;
    m_seed=seed;
    m_ready=true;
}

float noise_generator::set_settings(float persistance,float frequency,float amplitude,int number_of_octaves,int seed)
{
    m_number_of_octaves=number_of_octaves;
    m_persistance=persistance;
    m_frequency=frequency;
    m_amplitude=amplitude;
    m_seed=seed;
    m_ready=true;
}

float noise_generator::get_noise( float x )
{
    if(!m_ready) return -1;//error

    float Total=0.0;
    float amplitude=1;
    float amplitud_sum=amplitude;
    float frequency=m_frequency;

    for( unsigned int i = 0; i < m_number_of_octaves; ++i )
    {
        //int Freqency = 1<<i; // 2^i
        //float Amplitude = pow( m_persistance, i );

        Total += InterpolatedNoise( x * m_frequency + m_seed) * amplitude;
        frequency*=2;
        amplitude*=m_persistance;
        amplitud_sum+=amplitude;
    }

    return Total/amplitud_sum*m_amplitude+m_amplitude;
}

float noise_generator::InterpolatedNoise( float x )
{
    int IntX = (int)x;
    float FraqX = x - IntX;

    float v1 = SmoothNoise( IntX );
    float v2 = SmoothNoise( IntX + 1 );

    return CosineInterpolation( v1, v2, FraqX );
}

float noise_generator::Noise( int x )
{
    x = (x << 13) ^ x;

    return ( 1.0 - ( (x * (x * x * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
}

float noise_generator::CosineInterpolation( float a, float b, float x )
{
    float ft = x * M_PI;
    float f = (1 - cos( ft )) * 0.5;

    return a * (1 - f) + b * f;
}

float noise_generator::SmoothNoise( float x )
{
    return Noise( x )*0.5 + (Noise( x - 1 ) + Noise( x + 1 ))*0.25;
}
