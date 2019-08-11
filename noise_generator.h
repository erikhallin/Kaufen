#ifndef NOISE_GENERATOR_H
#define NOISE_GENERATOR_H


class noise_generator
{
    public:
        //Constructors
        noise_generator();
        noise_generator(float persistance,float frequency,float amplitude,int number_of_octaves,int seed);
        //Variables
        bool m_ready;
        //Functions
        float set_settings(float persistance,float frequency,float amplitude,int number_of_octaves,int seed);
        float get_noise(float x);

    private:
        //Variables
        int   m_number_of_octaves,m_seed;
        float m_persistance,m_frequency,m_amplitude;
        //Functions
        float Noise(int x);
        float SmoothNoise(float x);
        float CosineInterpolation(float a,float b,float x);
        float InterpolatedNoise(float x);


};

#endif // NOISE_GENERATOR_H
