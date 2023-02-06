#include "pch.h"
#include "Random.h"
#include <random>

float Random::Float(float aMin, float aMax)
{
	std::random_device seedForSeed;
	std::mt19937 seed(seedForSeed());
	std::uniform_real_distribution<> randomNumber(aMin, aMax);
	return static_cast<float>(randomNumber(seed));
}
