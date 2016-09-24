// Copyright Steinwurf APS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <tables/table.hpp>

/// The node class is the basic building block in this simple
/// simulation tool. The node has and id and input / output functions.
class node
{
public:

    node(const std::string& id) :
        m_id(id)
    { }

    virtual ~node()
    { }

    virtual void receive(packet payload) = 0;
    virtual void tick() = 0;

    virtual void store_run(tables::table& /*results*/)
    { }

    // Adds a receiver to the output
    virtual void add_output(const std::shared_ptr<node>& recv)
    {
        assert(recv);
        m_receivers.push_back(recv);
    }

    // Forwards a payload to the output
    void forward(uint32_t receiver_index, packet payload)
    {
        m_receivers[receiver_index]->receive(payload);
    }

    uint32_t receiver_count()
    {
        return m_receivers.size();
    }

    std::shared_ptr<node> get_receiver(uint32_t receiver_index)
    {
        return m_receivers[receiver_index];
    }

    std::string node_id()
    {
        return m_id;
    }

    const std::string& node_id() const
    {
        return m_id;
    }

private:

    /// The id of this node
    std::string m_id;

    // Stores the output receivers
    std::vector< std::shared_ptr<node> > m_receivers;

};
