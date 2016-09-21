// Copyright Steinwurf APS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <vector>
#include <map>
#include <string>

#include <tables/table.hpp>
#include <kodo/has_set_systematic_off.hpp>
#include <kodo/has_set_systematic_on.hpp>
#include <kodo/set_systematic_off.hpp>
#include <kodo/set_systematic_on.hpp>

#include "packet.hpp"
#include "source.hpp"

/// Implementation of basic source
template<class Encoder>
class basic_source : public source
{
public:

    basic_source(const std::string& id,
                 const std::shared_ptr<Encoder>& encoder) :
        source(id),
        m_encoder(encoder)
    {
        assert(m_encoder);

        m_payload.resize(encoder->payload_size());
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
        if (kodo::has_set_systematic_off<Encoder>::value)
            kodo::set_systematic_off(*m_encoder);
    }

    void systematic_on()
    {
        if (kodo::has_set_systematic_on<Encoder>::value)
            kodo::set_systematic_on(*m_encoder);
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

    // Payload buffer
    std::vector<uint8_t> m_payload;

    // Pointer to the encoder
    std::shared_ptr<Encoder> m_encoder;

    // Counter for statistics
    std::map<std::string, uint32_t> m_counter;
};
