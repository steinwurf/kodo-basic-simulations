// Copyright Steinwurf APS 2011-2013.
// Distributed under the "STEINWURF EVALUATION LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once


#include <string>
#include <vector>
#include <map>
#include <memory>

#include <tables/table.hpp>

#include "packet.hpp"
#include "sink.hpp"

/// Implementation of a basic sink
template<class Decoder>
class basic_sink : public sink
{
public:

    basic_sink(const std::string& id, std::shared_ptr<Decoder> decoder) :
        sink(id),
        m_decoder(decoder)
    {
        assert(m_decoder);

        m_payload.resize(m_decoder->max_payload_size());

        m_data.resize(m_decoder->block_size());
        m_decoder->set_symbols_storage(m_data.data());
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

        std::copy(payload.data_begin(), payload.data_end(),
                  &m_payload[0]);

        uint32_t rank = m_decoder->rank();
        m_decoder->consume_payload(&m_payload[0]);

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

    // Data buffer for the decoder
    std::vector<uint8_t> m_data;

    // Buffer for packets
    std::vector<uint8_t> m_payload;

    // The decoder
    std::shared_ptr<Decoder> m_decoder;

    // Counter for statistics
    std::map<std::string, uint32_t> m_counter;

};
