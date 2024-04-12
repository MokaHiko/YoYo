#pragma once

#include "Defines.h"
#include <iostream>

#include <random>
#include <type_traits>
#include <assert.h>

#include <vector>

namespace yoyo
{
    // Generates psuedo random values of integer or real types (double, float).
    // By default, the seed is set to the value of random device and the distribution is normalized from 0 to 100
    template <typename T>
    class YAPI PRNGenerator
    {
    public:
        // Checks for rand type
        template <typename RandType>
        using uniform_distribution = typename std::conditional<std::is_floating_point<RandType>::value, std::uniform_real_distribution<RandType>,
            typename std::conditional<std::is_integral<RandType>::value, std::uniform_int_distribution<RandType>, void>::type>::type;

        ~PRNGenerator() = default;

        PRNGenerator()
        {
            m_dist = uniform_distribution<T>(0, 100);
            srand(time(0));
            SetSeed(rand());
        };

        PRNGenerator(T lower, T upper, int32_t seed = 0)
        {
            m_dist = uniform_distribution<T>(lower, upper);

            if(!seed)
            {
                srand(time(0));
                SetSeed(rand());
            }
            else
            {
                SetSeed(seed);
            }
        };

        // Gets the next value
        const T Next()
        {
            return m_dist(eng);
        };

        // Sets the lower and upper bound of the random values
        void SetDistribution(T low, T upper)
        {
            m_dist = uniform_distribution<T>(low, upper);
        }

        // Sets the seed of the random engine
        void SetSeed(uint32_t seed)
        {
            eng = std::mt19937{ seed };
        }

    private:
        std::random_device m_device;
        std::mt19937 eng{ m_device() };
        uniform_distribution<T> m_dist;
    };

    // Creates a psuedo random traversal of indices of n elements;
    class YAPI PrimeSearch
    {
    public:
        PrimeSearch(int n_elements);

        // Returns index to next element to traverse and -1 if complete
        int GetNext(bool restart = false);

        // Returns how many searches have occured
        inline const int GetSearchCount() const {return m_searches;}

        // Restarts traversal back to 0 searches
        void Restart();

        // Returns prime number greater than n and 0 if none found
        static int FindNextPrime(int n);

        // Checks if n is prime
        static int IsPrime(int n);
    private:
        int m_searches = 0;
        int m_n_elements = 0;

        int m_nextMember = 0;
        int m_a, m_b, m_c = 0;
        int m_prime = 0;
        int m_skip = 0;

        PRNGenerator<int> m_random_engine = { 1, 100 };
    };
}