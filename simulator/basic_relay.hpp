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
#include "relay.hpp"

// Relay
template<class Decoder>
class basic_relay : public relay
{
public:

    basic_relay(const std::string &id,
                const std::shared_ptr<Decoder> &decoder)
        : relay(id),
          m_decoder(decoder),
          m_recode_on(true),
          m_transmit_on_receive(false),
          m_new_packet(false)
    {
        assert(m_decoder);

        m_recode_buffer.resize(m_decoder->payload_size());
        m_decode_buffer.resize(m_decoder->payload_size());
    }


    virtual void receive(packet payload)
    {
        m_last_packet = payload;
        m_new_packet = true;

        if(m_decoder->is_complete())
        {
            std::string counter_id =
                node_id()+"_waste_from_"+payload.get_sender();

            ++m_counter[counter_id];
        }
        else
        {
            std::copy(payload.data_begin(), payload.data_end(),
                      &m_decode_buffer[0]);

            uint32_t rank = m_decoder->rank();
            m_decoder->read_payload(&m_decode_buffer[0]);

            if(rank < m_decoder->rank())
            {
                std::string counter_id =
                    node_id()+"_innovative_from_"+payload.get_sender();

                ++m_counter[counter_id];
            }
            else
            {
                std::string counter_id =
                    node_id()+"_linear_dept_from_"+payload.get_sender();

                ++m_counter[counter_id];
            }
        }
    }

    void tick()
    {
        if(m_transmit_on_receive && !m_new_packet)
        {
            // In this mode we only transmit if we got an packet
            return;
        }

        // We send a packet either:
        // 1) We are transmitting on receive and we got a packet
        // 2) We always transmit on every tick

        if(m_recode_on)
        {
            m_decoder->write_payload(&m_recode_buffer[0]);
            packet p(m_recode_buffer);
            p.set_sender(node_id());
            forward_packet(p);
        }
        else
        {
            if(!m_last_packet.is_valid())
                return;

            m_last_packet.set_sender(node_id());
            forward_packet(m_last_packet);
        }

        m_new_packet = false;

    }

    void store_run(tables::table& results)
    {
        for(auto& c : m_counter)
        {
            if(!results.has_column(c.first))
            {
                results.add_column(c.first);
                results.set_default_value(c.first, uint32_t(0));
            }

            results.set_value(c.first,c.second);

        }
    }


    void forward_packet(packet payload)
    {
        for(uint32_t j = 0; j < receiver_count(); ++j)
        {
            forward(j, payload);
        }
    }


    void set_recode_on()
    {
        m_recode_on = true;
    }

    void set_recode_off()
    {
        m_recode_on = false;
    }

    bool is_recode_on() const
    {
        return m_recode_on;
    }

    void set_transmit_on_receive(bool value)
    {
        m_transmit_on_receive = value;
    }


private:


    /// Buffer for packets
    std::vector<uint8_t> m_recode_buffer;
    std::vector<uint8_t> m_decode_buffer;

    /// Decoder used by the relay to recode
    std::shared_ptr<Decoder> m_decoder;

    /// Statistics
    std::map<std::string, uint32_t> m_counter;

    /// Boolean whether we recode or simply forward packets
    bool m_recode_on;

    /// Boolean whether relay should transmit in every tick, or when a
    /// packet is received from sink
    bool m_transmit_on_receive;

    /// We store the last packet for forwarding
    bool m_new_packet;
    packet m_last_packet;

};
