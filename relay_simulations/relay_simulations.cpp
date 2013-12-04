// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#include <ctime>
#include <string>

#include "../simulator/simulator.hpp"

#include <gauge/gauge.hpp>
#include <gauge/json_printer.hpp>
#include <gauge/console_printer.hpp>
#include <gauge/csv_printer.hpp>

// Helper function to convert to string
template<class T>
std::string to_string(T t)
{
    std::stringstream ss;
    ss << t;
    return ss.str();
}

// The encoders and decoders that we use from the Kodo library
typedef kodo::full_rlnc_encoder<fifi::binary> Encoder;
typedef kodo::full_rlnc_decoder<fifi::binary> Decoder;

typedef kodo::full_rlnc_encoder<fifi::binary8> Encoder8;
typedef kodo::full_rlnc_decoder<fifi::binary8> Decoder8;

typedef kodo::full_rlnc_encoder<fifi::binary16> Encoder16;
typedef kodo::full_rlnc_decoder<fifi::binary16> Decoder16;

// The simple relay simulation for the following topology:
//
//     .------------------------------------.
//     |                                    |
//     |              +--------+            |
//     +              | relay  |            v
// +--------+         +--------+        +---------+
// | source | +--->              +----> | sink    |
// +--------+         +--------+        +---------+
//                    | relay  |
//                    +--------+
//                        x
//                        x
//                        x
//                        x
//                    +--------+
//                    | relay  |
//                    +--------+
//
// The simulator uses the gauge benchmarking tool for
// driving and providing options to the simulations.
template<class Encoder, class Decoder>
class relay_simulation : public gauge::benchmark
{
public:

    typedef basic_factory<Encoder,Decoder> factory_type;

public:

    relay_simulation()
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
        gauge::config_set cs = get_current_configuration();


        double error_source_sink = cs.get_value<double>("error_source_sink");
        double error_source_relay = cs.get_value<double>("error_source_relay");
        double error_relay_sink = cs.get_value<double>("error_relay_sink");

        uint32_t relays =
            cs.get_value<uint32_t>("relays");

        bool source_systematic =
            cs.get_value<bool>("source_systematic");

        bool relay_transmit_on_receive =
            cs.get_value<bool>("relay_transmit_on_receive");

        bool recode =
            cs.get_value<bool>("recode");

        m_scheduler = m_factory->build_scheduler();

        auto channel_source_sink =
            m_factory->build_channel(error_source_sink);

        auto channel_source_relay =
            m_factory->build_channel(error_source_relay);

        auto channel_relay_sink =
            m_factory->build_channel(error_relay_sink);

        m_scheduler->add_node(channel_source_sink);
        m_scheduler->add_node(channel_source_relay);
        m_scheduler->add_node(channel_relay_sink);

        auto source = m_factory->build_source("source");

        if(source_systematic)
            source->systematic_on();
        else
            source->systematic_off();

        auto sink = m_factory->build_sink("sink");

        m_scheduler->add_node(source);
        m_scheduler->add_node(sink);

        source->add_output(channel_source_sink);
        channel_source_sink->add_output(sink);

        source->add_output(channel_source_relay);

        for(uint32_t i = 0; i < relays; ++i)
        {
            auto relay = m_factory->build_relay("relay"+to_string(i));

            channel_source_relay->add_output(relay);
            relay->add_output(channel_relay_sink);
            relay->set_transmit_on_receive(relay_transmit_on_receive);

            if(recode)
            {
                relay->set_recode_on();
            }
            else
            {
                relay->set_recode_off();
            }

            m_scheduler->add_node(relay);
        }

        channel_relay_sink->add_output(sink);
        m_sink = sink;

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

        auto error_source_sink =
            options["error_source_sink"].as<double>();

        auto error_source_relay =
            options["error_source_relay"].as<double>();

        auto error_relay_sink =
            options["error_relay_sink"].as<double>();

        auto relays = options["relays"].as<uint32_t>();

        auto source_systematic = options["source_systematic"].as<bool>();
        auto relay_transmit_on_receive = options["relay_transmit_on_receive"].as<bool>();
        auto recode = options["recode"].as<bool>();

        gauge::config_set cs;
        cs.set_value<uint32_t>("symbols", symbols);
        cs.set_value<uint32_t>("symbol_size", symbol_size);
        cs.set_value<double>("error_source_sink", error_source_sink);
        cs.set_value<double>("error_source_relay", error_source_relay);
        cs.set_value<double>("error_relay_sink", error_relay_sink);
        cs.set_value<uint32_t>("relays", relays);
        cs.set_value<bool>("source_systematic", source_systematic);
        cs.set_value<bool>("relay_transmit_on_receive", relay_transmit_on_receive);
        cs.set_value<bool>("recode", recode);

        add_configuration(cs);

        m_factory = boost::make_shared<factory_type>(
            symbols, symbol_size, m_generator);

    }

protected:

    boost::random::mt19937 m_generator;

    boost::shared_ptr<basic_sink<Decoder> > m_sink;
    boost::shared_ptr<tick_scheduler> m_scheduler;

    boost::shared_ptr<factory_type> m_factory;

};

BENCHMARK_OPTION(relay_line)
{
    gauge::po::options_description options("relay line");

    auto default_symbols =
        gauge::po::value<uint32_t>()->default_value(
            32);

    options.add_options()
        ("symbols", default_symbols, "Set symbols");

    auto default_symbol_size =
        gauge::po::value<uint32_t>()->default_value(
            1400);

    options.add_options()
        ("symbol_size", default_symbol_size, "Set symbols size");

    auto default_error_source_sink =
        gauge::po::value<double>()->default_value(
            0.5);

    options.add_options()
        ("error_source_sink", default_error_source_sink, "Error source to sink");

    auto default_error_source_relay =
        gauge::po::value<double>()->default_value(
            0.5);

    options.add_options()
        ("error_source_relay", default_error_source_relay, "Error source to relay");

    auto default_error_relay_sink =
        gauge::po::value<double>()->default_value(
            0.5);

    options.add_options()
        ("error_relay_sink", default_error_relay_sink, "Error relay to sink");

    auto default_relays =
        gauge::po::value<uint32_t>()->default_value(1);

    options.add_options()
        ("relays", default_relays, "Relays");

    auto default_source_systematic =
        gauge::po::value<bool>()->default_value(true);

    options.add_options()
        ("source_systematic", default_source_systematic,
         "Whether the source is systematic or not "
         "--systematic=1 turns on systematic "
         "source. Systematic means that all "
         "packets in a generation are sent first "
         "once without coding. After sending "
         "everything once coding starts.");

    auto default_relay_transmit_on_receive =
        gauge::po::value<bool>()->default_value(true);

    options.add_options()
        ("relay_transmit_on_receive", default_relay_transmit_on_receive,
         "Set true if the relay(s) should transmit in every "
         "tick or when a packet is received from the "
         "source");

    auto default_recode =
        gauge::po::value<bool>()->default_value(true);

    options.add_options()
        ("recode", default_recode,
         "Set true if the relay(s) should recode packets");


    gauge::runner::instance().register_options(options);
}

typedef relay_simulation<Encoder, Decoder> relay_fixture;

BENCHMARK_F(relay_fixture, Relay, binary, 10)
{
    RUN{

        while(!m_sink->is_complete())
        {
            m_scheduler->tick();
        }

    }
}

typedef relay_simulation<Encoder8, Decoder8> relay_fixture8;

BENCHMARK_F(relay_fixture8, Relay, binary8, 10)
{
    RUN{

        while(!m_sink->is_complete())
        {
            m_scheduler->tick();
        }

    }
}

typedef relay_simulation<Encoder16, Decoder16> relay_fixture16;

BENCHMARK_F(relay_fixture16, Relay, binary16, 10)
{
    RUN{

        while(!m_sink->is_complete())
        {
            m_scheduler->tick();
        }

    }
}

int main(int argc, const char *argv[])
{
    srand((uint32_t)time(0));

    gauge::runner::instance().printers().push_back(
        std::make_shared<gauge::console_printer>());

    gauge::runner::instance().printers().push_back(
        std::make_shared<gauge::json_printer>());

    gauge::runner::instance().printers().push_back(
        std::make_shared<gauge::csv_printer>());

    gauge::runner::run_benchmarks(argc, argv);

    return 0;
}
