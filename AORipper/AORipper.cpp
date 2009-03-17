// AORipper.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <iostream>
#include <Shared/AODatabaseParser.h>
#include <Shared/AODatabaseWriter.h>


namespace bpo = boost::program_options;
namespace bfs = boost::filesystem;

// Forwards
bool RippIt(std::string const& input, std::string const& output, bool doPostProcess);


int _tmain(int argc, _TCHAR* argv[])
{
    std::string input;
    std::string output;
    bool doPostProcess;
    bool doForce; 

    // Declare the supported options.
    bpo::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("input,i", bpo::value<std::string>(&input), "Specify the input AODB file.")
        ("output,o", bpo::value<std::string>(&output), "Specify the output SQLite file.")
        ("postprocess,p", bpo::value<bool>(&doPostProcess)->default_value(false), "Specify if postprocessing should be done or not.")
        ("force,f", bpo::value<bool>(&doForce)->default_value(false), "Force overwriting the output file if it exists.")
        ;

    bpo::variables_map vm;
    bpo::store(bpo::parse_command_line(argc, argv, desc), vm);
    bpo::notify(vm);    

    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 1;
    }

    if (input.empty() || output.empty()) {
        std::cerr << "Invalid arguments.\n";
        std::cout << desc << "\n";
        return 2;
    }

    if (!bfs::exists(input)) {
        std::cerr << "Could not locate input file.";
        return 3;
    }
    if (bfs::exists(output)) {
        if (!doForce) {
            std::cerr << "Output file already exists.";
            return 3;
        }
        bfs::remove(output);
    }

    if (!RippIt(input, output, doPostProcess)) {
        return 4;
    }

	return 0;
}


bool RippIt(std::string const& input, std::string const& output, bool doPostProcess)
{
    try {
        AODatabaseParser aodb(input);
        AODatabaseWriter writer(output);

        // Extract items
        boost::shared_ptr<ao_item> item = aodb.GetFirstItem(AODB_TYP_ITEM);
        if (item) {
            unsigned int itemcount = 1;
            writer.BeginWrite();
            writer.WriteItem(item);
            while (item = aodb.GetNextItem()) { 
                ++itemcount; 
                writer.WriteItem(item);
                if (itemcount % 10000 == 0) {
                    writer.CommitItems();
                    writer.BeginWrite();
                }
            }
            writer.CommitItems();
        }

        // Extract nano programs
        boost::shared_ptr<ao_item> nano = aodb.GetFirstItem(AODB_TYP_NANO);
        if (nano) {
            unsigned int itemcount = 1;
            writer.BeginWrite();
            writer.WriteItem(nano);
            while (nano = aodb.GetNextItem()) { 
                ++itemcount; 
                writer.WriteItem(nano);
                if (itemcount % 10000 == 0) {
                    writer.CommitItems();
                    writer.BeginWrite();
                }
            }
            writer.CommitItems();
        }

        writer.PostProcessData();
    }
    catch (std::bad_alloc &e) {
        assert(false);
        std::cerr << "Error creating item database. " << e.what();
        return false;
    }
    catch (std::exception &e) {
        assert(false);
        std::cerr << "Error creating item database. " << e.what();
        return false;
    }

    return true;
}