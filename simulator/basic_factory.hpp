// Copyright Steinwurf APS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once


/// Helper class which makes it a bit more convenient to build
/// the elements of a basic simulation
template<class Encoder, class Decoder>
class basic_factory
{
public:

    basic_factory(uint32_t symbols, uint32_t symbol_size,
                  boost::random::mt19937 &random_generator) :
        m_decoder_factory(symbols, symbol_size),
        m_encoder_factory(symbols, symbol_size),
        m_random_generator(random_generator)
    {
        uint32_t max_block_size =
            m_encoder_factory.max_block_size();

        m_data.resize(max_block_size);

        for(auto& d : m_data)
            d = rand() % 256;
    }

    std::shared_ptr<basic_channel>
    build_channel(double success_probability, const std::string &id = "channel")
    {
        assert(success_probability >= 0.0);
        assert(success_probability <= 1.0);

        auto channel_conditions = std::make_shared<random_bool>(
            std::ref(m_random_generator), success_probability);

        return std::make_shared<basic_channel>(id, channel_conditions);
    }

    std::shared_ptr<basic_sink<Decoder> >
    build_sink(const std::string &id)
    {
        auto decoder = m_decoder_factory.build();
        auto sink = std::make_shared< basic_sink<Decoder> >(id, decoder);

        return sink;
    }

    std::shared_ptr<basic_relay<Decoder> >
    build_relay(const std::string &id)
    {
        auto decoder = m_decoder_factory.build();
        auto relay = std::make_shared< basic_relay<Decoder> >(id, decoder);

        return relay;
    }

    std::shared_ptr<basic_source<Encoder> >
    build_source(const std::string &id)
    {
        auto encoder = m_encoder_factory.build();
        encoder->set_symbols(sak::storage(m_data));

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
        return std::make_shared<random_bool>(std::ref(m_random_generator),
                                               success_probability);
    }


private:

    /// Factory for building decoders
    typename Decoder::factory m_decoder_factory;

    /// Factory for building encoders
    typename Encoder::factory m_encoder_factory;

    /// Data for the encoders
    std::vector<uint8_t> m_data;

    /// The random generator
    boost::random::mt19937 &m_random_generator;
};
