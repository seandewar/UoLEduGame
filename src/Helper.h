#pragma once

#include <random>

#include "Types.h"

/**
* Contains Helper functions.
*/
class Helper
{
	/**
	* Internal RNG for quickly generating random numbers without needing
	* to specify your own.
	*/
	static Rng rng_;

public:
	/**
	* Generate random int using an std::uniform_int_distribution.
	*/
	template <typename RngT, typename IntType = RngInt>
	static IntType GenerateRandomInt(RngT& rng, IntType min, IntType max)
	{
		std::uniform_int_distribution<IntType> dist(min, max);
		return dist(rng);
	}
	
	template <typename IntType = RngInt>
	static inline IntType GenerateRandomInt(IntType min, IntType max)
	{
		return GenerateRandomInt<decltype(rng_), IntType>(rng_, min, max);
	}

	/**
	* Generate random boolean value using an std::bernoulli_distribution.
	*/
	template <typename RngT>
	static bool GenerateRandomBool(RngT& rng, double trueProbability = 0.5)
	{
		std::bernoulli_distribution dist(trueProbability);
		return dist(rng);
	}

	static inline bool GenerateRandomBool(double trueProbability = 0.5)
	{
		return GenerateRandomBool<decltype(rng_)>(rng_, trueProbability);
	}
};