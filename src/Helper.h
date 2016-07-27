#pragma once

#include <random>
#include <memory>

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
	template <typename RngT, typename IntType = int>
	static IntType GenerateRandomInt(RngT& rng, IntType min, IntType max)
	{
		std::uniform_int_distribution<IntType> dist(min, max);
		return dist(rng);
	}
	
	template <typename IntType = int>
	static inline IntType GenerateRandomInt(IntType min, IntType max)
	{
		return GenerateRandomInt<decltype(rng_), IntType>(rng_, min, max);
	}

    /**
    * Generate random real using an std::uniform_real_distribution.
    */
    template <typename RngT, typename RealType = float>
    static RealType GenerateRandomReal(RngT& rng, RealType min, RealType max)
    {
        std::uniform_real_distribution<RealType> dist(min, max);
        return dist(rng);
    }

    template <typename RealType = float>
    static inline RealType GenerateRandomReal(RealType min, RealType max)
    {
        return GenerateRandomReal<decltype(rng_), RealType>(rng_, min, max);
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

    static inline std::unique_ptr<sf::Drawable> GetTextDropShadow(const sf::Text& text,
        const sf::Vector2f& offset = sf::Vector2f(2.0f, 2.0f), const sf::Color& color = sf::Color(0, 0, 0))
    {
        auto textShadow = text;
        textShadow.move(offset);
        textShadow.setColor(color);

        return std::move(std::make_unique<sf::Text>(textShadow));
    }

    static inline void RenderTextWithDropShadow(sf::RenderTarget& target, const sf::Text& text, 
        const sf::Vector2f& offset = sf::Vector2f(2.0f, 2.0f), const sf::Color& color = sf::Color(0, 0, 0))
    {
        target.draw(*GetTextDropShadow(text, offset, color));
        target.draw(text);
    }

    static inline sf::Vector2f ComputeGoodAspectSize(sf::RenderTarget& target, float size)
    {
        auto aspectVertMul = static_cast<float>(target.getSize().y) / target.getSize().x;

        if (aspectVertMul <= 1.0f) {
            return sf::Vector2f(size, size * aspectVertMul);
        }
        else {
            return sf::Vector2f(size / aspectVertMul, size);
        }
    }

    static inline void ResetTargetView(sf::RenderTarget& target)
    {
        target.setView(sf::View(sf::FloatRect(0.0f, 0.0f,
            static_cast<float>(target.getSize().x), static_cast<float>(target.getSize().y))));
    }
};