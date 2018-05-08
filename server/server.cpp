#include <iostream>
#include <string>
#include <vector>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>

#include "server.h"
#include "AbstractResource.h"
#include "SharedMemoryBuffer.h"
#include "SharedMemoryProtocol.h"
#include "FileTransmitter.h"
#include "ComNamer.h"

using namespace std;
using namespace boost::program_options;
using namespace boost::filesystem;
using namespace boost::interprocess;

int main( int argc, char *argv[] )
{
  ComNamer namer;

  try  {
    check_arguments(argc, argv);

    SharedMemoryProtocol protocol( namer.GetSharedMemoryName(),
                                   to_string( reinterpret_cast<long>(main) ),
                                   true );
    FileTransmitter ft(protocol);

    cout << "Receiving files..." << endl;

    while (true) {
      ft.receive();
    }
  } 
  catch (boost::interprocess::interprocess_exception&) {
    cout << "ERROR: Server not available." << endl;
    return -1;
  } 
  catch (boost::program_options::error& e) {
    cout << e.what() << endl;
    return -1;
  } 
  catch (std::invalid_argument& e) {
    cout << e.what() << endl;
    return -1;
  }

  return 0;
}

void check_arguments(int argc, char* argv[])
{
  options_description desc("Server part of the file transmitter.");

  desc.add_options()
      ("help", "produce help message");

  variables_map vm;
  store( command_line_parser(argc, argv).options(desc).run(), vm);
  notify(vm);

  if ( vm.count("help") ) {
    cout << desc << endl;
    exit(0);
  }

  //TODO: add flag for delete all files
}
