#pragma once

#include <random>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Text.hpp>

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

    static inline void RenderTextWithDropShadow(sf::RenderTarget& target, const sf::Text& text, 
        const sf::Vector2f& offset = sf::Vector2f(5.0f, 5.0f), const sf::Color& color = sf::Color(0, 0, 0))
    {
        auto textShadow = text;
        textShadow.move(offset);
        textShadow.setColor(color);

        target.draw(textShadow);
        target.draw(text);
    }
};