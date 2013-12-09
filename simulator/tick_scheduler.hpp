// Copyright Steinwurf APS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <vector>

#include <boost/shared_ptr.hpp>
#include <tables/table.hpp>

#include "node.hpp"

/// Simple scheduler task which sends a tick to each
/// node added
class tick_scheduler
{
public:

    typedef boost::shared_ptr<node> node_ptr;

    void add_node(const node_ptr &node)
    {
        m_nodes.push_back(node);
    }

    void tick()
    {
        for(uint32_t i = 0; i < m_nodes.size(); ++i)
        {
            m_nodes[i]->tick();
        }
    }

    void store_run(tables::table& results)
    {
        for(auto& n: m_nodes)
        {
            n->store_run(results);
        }
    }

    // Stores the output receivers
    std::vector<node_ptr> m_nodes;
};

