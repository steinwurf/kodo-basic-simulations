// Copyright Steinwurf APS 2011-2013.
// Distributed under the "STEINWURF EVALUATION LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <cstdlib>
#include <vector>
#include <map>
#include <string>

#include <tables/table.hpp>

#include "packet.hpp"
#include "source.hpp"

/// Implementation of basic source
template<class Encoder>
class basic_source : public source
{
public:

    basic_source(const std::string& id, std::shared_ptr<Encoder> encoder) :
        source(id),
        m_encoder(encoder)
    {
        assert(m_encoder);
        m_payload.resize(m_encoder->payload_size());

        m_data.resize(m_encoder->block_size());

        for (auto& d : m_data)
            d = rand() % 256;

        m_encoder->set_const_symbols(storage::storage(m_data));
    }

    void tick()
    {
        for (uint32_t i = 0; i < receiver_count(); ++i)
        {
            ++m_counter["source_sent"];

            m_encoder->write_payload(&m_payload[0]);

            packet p(m_payload);
            p.set_sender(node_id());
            forward(i,p);
        }
    }

    void systematic_off()
    {
        if (m_encoder->has_systematic_mode())
            m_encoder->set_systematic_off();
    }

    void systematic_on()
    {
        if (m_encoder->has_systematic_mode())
            m_encoder->set_systematic_on();
    }

    void store_run(tables::table& results)
    {
        for (auto& c : m_counter)
        {
            if (!results.has_column(c.first))
            {
                results.add_column(c.first);
                results.set_default_value(c.first, uint32_t(0));
            }

            results.set_value(c.first,c.second);
        }
    }

    void receive(packet /*p*/)
    {
        assert(0);
    }

private:

    // Data buffer for the encoder
    std::vector<uint8_t> m_data;

    // Payload buffer
    std::vector<uint8_t> m_payload;

    // Pointer to the encoder
    std::shared_ptr<Encoder> m_encoder;

    // Counter for statistics
    std::map<std::string, uint32_t> m_counter;
};
