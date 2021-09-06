#include "RandomGenerator.h"
#include <utility>
#include <chrono>

RandomGenerator::RandomGenerator(){

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    random_engine.seed(seed);

}

RandomGenerator::~RandomGenerator(){}

RandomIntGenerator::RandomIntGenerator(const int min, const int max){

    std::uniform_int_distribution<int> d(min, max);
    distribution = std::move(d);    //No sé si se usa así o distribution(&&d);

}

RandomIntGenerator::~RandomIntGenerator(){}

int RandomIntGenerator::next(){

    return distribution(random_engine);

}

RandomGaussianGenerator::RandomGaussianGenerator(const float mean, const float std){

    std::normal_distribution<float> d(mean, std);
    distribution = std::move(d);    //No sé si se usa así o std::move(&&d);

}

RandomGaussianGenerator::~RandomGaussianGenerator(){}

float RandomGaussianGenerator::next(){

    return distribution(random_engine);

}