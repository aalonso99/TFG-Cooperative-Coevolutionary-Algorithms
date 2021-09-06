#ifndef RANDOMGEN_H
#define RANDOMGEN_H

#include <random>

class RandomGenerator{
    
    protected:
    
        std::mt19937 random_engine;

    public:

        RandomGenerator();
        ~RandomGenerator();

};

class RandomIntGenerator: public RandomGenerator {

    private:

        std::uniform_int_distribution<int> distribution;

    public:

        RandomIntGenerator(const int min, const int max);
        ~RandomIntGenerator();

        int next();
};

class RandomGaussianGenerator: public RandomGenerator {

    private:

        std::normal_distribution<float> distribution;

    public:

        RandomGaussianGenerator(const float mean, const float std);
        ~RandomGaussianGenerator();

        float next();
};


#endif