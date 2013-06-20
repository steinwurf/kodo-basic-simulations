// Copyright Steinwurf APS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <map>
#include <vector>

#include <gauge/table.hpp>

#include "packet.hpp"
#include "random_bool.hpp"
#include "channel.hpp"

/// Implementation of a basic channel with a specific loss rate
class basic_channel : public channel
{
public:

    /// Create a simple coin-flip channel
    basic_channel(const std::string &id,
                  const boost::shared_ptr<random_bool> &channel_condition)
        : channel(id),
          m_channel_condition(channel_condition)
    { }

    /// Receives a payload
    virtual void receive(packet payload)
    {
        for(uint32_t j = 0; j < receiver_count(); ++j)
        {
            std::string recv_id = get_receiver(j)->node_id();
            std::string src_id = payload.get_sender();

            // If true we drop
            if(m_channel_condition->generate())
            {
                ++m_counter[node_id()+"_"+src_id+"_to_"+recv_id+"_dropped"];
            }
            else
            {
                ++m_counter[node_id()+"_"+src_id+"_to_"+recv_id+"_ok"];

                // Deliver packet to receiver j
                forward(j, payload);
            }
        }

    }

    void store_run(gauge::table& results)
    {
        for(auto& c : m_counter)
        {
            if(!results.has_column(c.first))
            {
                results.add_column(c.first, uint32_t(0));
            }

            results.set_value(c.first,c.second);
        }
    }

    virtual void tick()
    {
        // Channels just forward only sinks, relays, sources
        // use the tick()
    }

private:

    // Incoming packet queue
    std::vector<packet> m_packets;

    // The channel
    boost::shared_ptr<random_bool> m_channel_condition;

    // Statistics
    std::map<std::string, uint32_t> m_counter;
};

