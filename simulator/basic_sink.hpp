// Copyright Steinwurf APS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <boost/shared_ptr.hpp>

#include <string>
#include <vector>
#include <map>

#include <tables/table.hpp>

#include "packet.hpp"
#include "sink.hpp"

/// Implementation of a basic sink
template<class Decoder>
class basic_sink : public sink
{
public:

    basic_sink(const std::string& id,
               const std::shared_ptr<Decoder>& decoder) :
        sink(id),
        m_decoder(decoder)
    {
        assert(m_decoder);

        m_decode_buffer.resize(m_decoder->payload_size());
    }

    void tick()
    {
        // no implementation needed
    }

    void receive(packet payload)
    {
        assert(payload.get_data_size() > 0);
        std::string sender = payload.get_sender();

        ++m_counter[node_id()+"_receive_from_"+sender];

        if (m_decoder->is_complete())
        {
            ++m_counter[node_id()+"_waste_from_"+sender];
            return;
        }

        assert(m_decoder->payload_size() == payload.get_data_size());

        std::copy(payload.data_begin(), payload.data_end(),
                  &m_decode_buffer[0]);

        uint32_t rank = m_decoder->rank();
        m_decoder->read_payload(&m_decode_buffer[0]);

        if (m_decoder->rank() > rank)
        {
            ++m_counter[node_id()+"_innovative_from_"+sender];
        }
        else
        {
            ++m_counter[node_id()+"_linear_dept_from_"+sender];
        }
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

    bool is_complete()
    {
        return m_decoder->is_complete();
    }

private:


    // Buffer for packets
    std::vector<uint8_t> m_decode_buffer;

    // The decoder
    std::shared_ptr<Decoder> m_decoder;

    // Counter for statistics
    std::map<std::string, uint32_t> m_counter;

};
