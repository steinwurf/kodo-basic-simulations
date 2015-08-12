// Copyright Steinwurf APS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/bernoulli_distribution.hpp>

/// The simple object providing wrapping a bernoulli
/// distriubtion.
class random_bool
{
public:

    /// @param random_generator The random generator to use
    /// @param true_probability The probability of generation a 1
    random_bool(boost::random::mt19937 &random_generator,
                double true_probability)
        : m_random_generator(random_generator),
          m_distribution(true_probability)
    { }

    /// @return A random generated bool
    bool generate()
    {
        return m_distribution(m_random_generator);
    }

private:

    // The random generator
    boost::random::mt19937 &m_random_generator;

    // The distribution wrapping the random generator
    boost::random::bernoulli_distribution<> m_distribution;

};
