// Copyright Steinwurf ApS 2015.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <ctime>
#include <string>

#include "../simulator/simulator.hpp"

#include <gauge/gauge.hpp>
#include <gauge/python_printer.hpp>
#include <gauge/console_printer.hpp>
#include <gauge/csv_printer.hpp>

#include <boost/random/mersenne_twister.hpp>

#include <kodo/rlnc/full_vector_codes.hpp>

// Helper function to convert to string
template<class T>
std::string to_string(T t)
{
    std::stringstream ss;
    ss << t;
    return ss.str();
}

// The encoders and decoders that we use from the Kodo library
typedef kodo::rlnc::full_vector_encoder<fifi::binary> Encoder;
typedef kodo::rlnc::full_vector_decoder<fifi::binary> Decoder;

typedef kodo::rlnc::full_vector_encoder<fifi::binary8> Encoder8;
typedef kodo::rlnc::full_vector_decoder<fifi::binary8> Decoder8;

typedef kodo::rlnc::full_vector_encoder<fifi::binary16> Encoder16;
typedef kodo::rlnc::full_vector_decoder<fifi::binary16> Decoder16;

// The simple butterfly simulation for the following topology:
//             +--------+
//      +------+ source +------+
//      |      +--------+      |
//      |                      |
//      |                      |
//      |                      |
// +----v---+             +----v---+
// | relay1 +----+    +---+ relay2 |
// +----+---+    |    |   +----+---+
//      |        |    |        |
//      |        |    |        |
//      |      +-v----v +      |
//      |      | relay3 |      |
//      |      +----+---+      |
//      |           |          |
//      |           |          |
//      |           |          |
//      |      +----+---+      |
//      |  +---+ relay4 +---+  |
//      |  |   +--------+   |  |
//      |  |                |  |
//      |  |                |  |
// +----v--v+             +-v--v---+
// | sink1  |             | sink2  |
// +--------+             +--------+
//
// The simulator uses the gauge benchmarking tool for
// driving and providing options to the simulations.
template<class Encoder, class Decoder>
class butterfly : public gauge::benchmark
{
public:

    using factory_type = basic_factory<Encoder,Decoder>;

public:

    butterfly()
    {
        m_generator.seed((uint64_t)time(0));
    }

    /// Start benchmarking (not needed for the simulator)
    void start()
    { }

    /// Stop benchmarking (not needed for the simulator)
    void stop()
    { }

    /// Store the results from a run in the simulator
    void store_run(tables::table& results)
    {
        m_scheduler->store_run(results);
    }

    /// Build the topology and setup the simulation
    void setup()
    {

        m_scheduler = m_factory->build_scheduler();

        double channel_error = 0.0;

        auto channel_s_to_r1 =
            m_factory->build_channel(channel_error);

        auto channel_s_to_r2 =
            m_factory->build_channel(channel_error);

        auto channel_r1_to_r3_s1 =
            m_factory->build_channel(channel_error);

        auto channel_r2_to_r3_s2 =
            m_factory->build_channel(channel_error);

        auto channel_r3_to_r4 =
            m_factory->build_channel(channel_error);

        auto channel_r4_to_s1_s2 =
            m_factory->build_channel(channel_error);

        m_scheduler->add_node(channel_s_to_r1);
        m_scheduler->add_node(channel_s_to_r2);
        m_scheduler->add_node(channel_r1_to_r3_s1);
        m_scheduler->add_node(channel_r2_to_r3_s2);
        m_scheduler->add_node(channel_r3_to_r4);
        m_scheduler->add_node(channel_r4_to_s1_s2);

        auto s = m_factory->build_source("source");
        m_s1 = m_factory->build_sink("sink1");
        m_s2 = m_factory->build_sink("sink2");
        auto r1 = m_factory->build_relay("relay1");
        auto r2 = m_factory->build_relay("relay2");
        auto r3 = m_factory->build_relay("relay3");
        auto r4 = m_factory->build_relay("relay4");

        m_scheduler->add_node(s);
        m_scheduler->add_node(m_s1);
        m_scheduler->add_node(m_s2);
        m_scheduler->add_node(r1);
        m_scheduler->add_node(r2);
        m_scheduler->add_node(r3);
        m_scheduler->add_node(r4);

        s->add_output(channel_s_to_r1);
        s->add_output(channel_s_to_r2);

        channel_s_to_r1->add_output(r1);
        channel_s_to_r2->add_output(r2);

        r1->add_output(channel_r1_to_r3_s1);
        channel_r1_to_r3_s1->add_output(r3);
        channel_r1_to_r3_s1->add_output(m_s1);

        r2->add_output(channel_r2_to_r3_s2);
        channel_r2_to_r3_s2->add_output(r3);
        channel_r2_to_r3_s2->add_output(m_s2);

        r3->add_output(channel_r3_to_r4);
        channel_r3_to_r4->add_output(r4);

        r4->add_output(channel_r4_to_s1_s2);
        channel_r4_to_s1_s2->add_output(m_s1);
        channel_r4_to_s1_s2->add_output(m_s2);

        gauge::config_set cs = get_current_configuration();

        // Whether we want to recode at relay3
        bool recode =
            cs.get_value<bool>("recode");


        if(recode)
        {
            r3->set_recode_on();
        }
        else
        {
            r3->set_recode_off();
        }
    }

    /// The unit we are measuring in
    std::string unit_text() const
    {
        return "packets";
    }

    /// Get the options specified on the command-line and map this to a
    /// gauge::config which will be used by the simulator to run different
    /// configurations.
    void get_options(gauge::po::variables_map& options)
    {
        auto symbols =
            options["symbols"].as<uint32_t>();

        auto symbol_size =
            options["symbol_size"].as<uint32_t>();

        auto recode = options["recode"].as<bool>();

        gauge::config_set cs;
        cs.set_value<uint32_t>("symbols", symbols);
        cs.set_value<uint32_t>("symbol_size", symbol_size);
        cs.set_value<bool>("recode", recode);

        add_configuration(cs);

        m_factory = std::make_shared<factory_type>(
            symbols, symbol_size, m_generator);

    }

protected:

    boost::random::mt19937 m_generator;

    std::shared_ptr<basic_sink<Decoder> > m_s1;
    std::shared_ptr<basic_sink<Decoder> > m_s2;
    std::shared_ptr<tick_scheduler> m_scheduler;

    std::shared_ptr<factory_type> m_factory;

};

BENCHMARK_OPTION(butterfly)
{
    gauge::po::options_description options("butterfly");

    options.add_options()
        ("symbols", gauge::po::value<uint32_t>()->default_value(32),
         "Set symbols");

    options.add_options()
        ("symbol_size", gauge::po::value<uint32_t>()->default_value(1400),
         "Set symbols size");

    options.add_options()
        ("recode", gauge::po::value<bool>()->default_value(true),
         "Set true if the relay(s) should recode packets");


    gauge::runner::instance().register_options(options);
}

typedef butterfly<Encoder, Decoder> butterfly_fixture;

BENCHMARK_F_INLINE(butterfly_fixture, Butterfly, binary, 10)
{
    RUN{

        while(!(m_s1->is_complete() && m_s2->is_complete()))
        {
            m_scheduler->tick();
        }

    }
}

// typedef butterfly_simulation<Encoder8, Decoder8> butterfly_fixture8;

// BENCHMARK_F_INLINE(butterfly_fixture8, Butterfly, binary8, 10)
// {
//     RUN{

//         while(!m_sink->is_complete())
//         {
//             m_scheduler->tick();
//         }

//     }
// }

// typedef butterfly_simulation<Encoder16, Decoder16> butterfly_fixture16;

// BENCHMARK_F_INLINE(butterfly_fixture16, Butterfly, binary16, 10)
// {
//     RUN{

//         while(!m_sink->is_complete())
//         {
//             m_scheduler->tick();
//         }

//     }
// }

int main(int argc, const char *argv[])
{
    srand(static_cast<uint32_t>(time(0)));

    gauge::runner::add_default_printers();
    gauge::runner::run_benchmarks(argc, argv);

    return 0;
}
