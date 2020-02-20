// Copyright Steinwurf APS 2011-2013.
// Distributed under the "STEINWURF EVALUATION LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <fifi/finite_field.hpp>

/// Helper class which makes it a bit more convenient to build
/// the elements of a basic simulation
template<class Encoder, class Decoder>
class basic_factory
{
public:

    basic_factory(
        fifi::finite_field field, uint32_t symbols, uint32_t symbol_size,
        boost::random::mt19937& random_generator) :
        m_field(field), m_symbols(symbols), m_symbol_size(symbol_size),
        m_random_generator(random_generator)
    {
    }

    std::shared_ptr<basic_channel>
    build_channel(double success_probability, const std::string& id = "channel")
    {
        assert(success_probability >= 0.0);
        assert(success_probability <= 1.0);

        auto channel_conditions = std::make_shared<random_bool>(
            std::ref(m_random_generator), success_probability);

        return std::make_shared<basic_channel>(id, channel_conditions);
    }

    std::shared_ptr<basic_sink<Decoder> >
    build_sink(const std::string& id)
    {
        auto decoder =
            std::make_shared<Decoder>(m_field, m_symbols, m_symbol_size);
        auto sink = std::make_shared< basic_sink<Decoder> >(id, decoder);

        return sink;
    }

    std::shared_ptr<basic_relay<Decoder> >
    build_relay(const std::string& id)
    {
        auto decoder =
            std::make_shared<Decoder>(m_field, m_symbols, m_symbol_size);
        auto relay = std::make_shared< basic_relay<Decoder> >(id, decoder);

        return relay;
    }

    std::shared_ptr<basic_source<Encoder> >
    build_source(const std::string& id)
    {
        auto encoder =
            std::make_shared<Encoder>(m_field, m_symbols, m_symbol_size);
        auto source = std::make_shared< basic_source<Encoder> >(id, encoder);

        return source;
    }

    std::shared_ptr<tick_scheduler> build_scheduler()
    {
        return std::make_shared<tick_scheduler>();
    }

private:

    std::shared_ptr<random_bool> build_random_bool(double success_probability)
    {
        return std::make_shared<random_bool>(
            std::ref(m_random_generator), success_probability);
    }


private:

    /// The finite field for coders
    fifi::finite_field m_field;

    /// The number of symbols for coders
    uint32_t m_symbols;

    /// The symbol size for coders
    uint32_t m_symbol_size;

    /// The random generator
    boost::random::mt19937& m_random_generator;
};
