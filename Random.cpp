//
// Created by Aviv on 08/07/2021.
//
#include <ctime>
#include "Random.h"

double Random::Rand(double min, double max){
    std::random_device rd;
    std::default_random_engine re(rd());
    std::uniform_real_distribution<double> unif(min,max);
    return unif(re);
}