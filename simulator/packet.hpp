// Copyright Steinwurf APS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

/// Simple packet with copy-on-write semantics
class packet
{
    struct impl
    {
        impl()
            {}

        impl(const impl &i)
            : m_data(i.m_data)
            {}

        impl(const std::vector<uint8_t> &d)
            : m_data(d)
            {}

        std::vector<uint8_t> m_data;
        std::string m_sender_id;
    };

public:

    packet()
        : m_impl(std::make_shared<impl>())
        { }

    packet(const std::vector<uint8_t> &d)
        : m_impl(std::make_shared<impl>(d))
        { }

    void detach()
        {
            if( !m_impl.unique() )
            {
                m_impl = std::make_shared<impl>(*m_impl);
            }
        }

    void set_data(const uint8_t *data, uint32_t size)
        {
            detach();
            m_impl->m_data.resize(size);
            std::copy(data, data + size, &m_impl->m_data[0]);
        }

    bool is_valid()
        {
            return m_impl->m_data.size() > 0;
        }

    void set_sender(const std::string &sender_id)
        {
            m_impl->m_sender_id = sender_id;
        }

    const std::string& get_sender() const
        {
            return m_impl->m_sender_id;
        }

    std::string get_sender()
        {
            return m_impl->m_sender_id;
        }

    const uint8_t* get_data() const
        {
            return &m_impl->m_data[0];
        }

    uint8_t* get_data()
        {
            detach();
            return &m_impl->m_data[0];
        }

    const uint8_t* data_begin() const
        {
            return get_data();
        }

    const uint8_t* data_end() const
        {
            return get_data() + get_data_size();
        }

    uint32_t get_data_size() const
        {
            return m_impl->m_data.size();
        }

private:

    std::shared_ptr<impl> m_impl;

};
