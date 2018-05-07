#include <iostream>
#include <string>
#include <vector>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include "FileTransmitter.h"
#include "SharedMemoryProtocol.h"
#include "namer.h"

using namespace std;
using namespace boost::program_options;
using namespace boost::filesystem;

int main( int argc, char *argv[] )
{
  Namer namer;

  options_description desc("Files transmition to server part");

  desc.add_options()
      ("help", "produce help message")
      ("target,t", value< vector<string> >()->multitoken(), "list of target files for transmit");

  variables_map vm;
  store( command_line_parser(argc, argv).options(desc).run(), vm);
  notify(vm);

  if ( vm.count("help") ) {
    cout << desc << endl;
    return 1;
  }

  if ( !vm.count("target") || (argc < 2) ) {
    cout << "Nothing to send" << endl;
    return 1;
  }

  std::unique_ptr<SharedMemoryProtocol> protocol;
  std::unique_ptr<FileTransmitter> ft;

  try  {
    protocol = make_unique<SharedMemoryProtocol>( namer.GetSharedMemoryName(),
                                                  to_string( reinterpret_cast<long>(main) ) );
    ft = make_unique<FileTransmitter>( *protocol );
  }
  catch (exception&) {
    cout << "ERROR: Server not available." << endl;
    return 1;
  }

  const vector<string>& file_list = vm["target"].as< vector<string> >();

  for (auto it : file_list) {
    try {
      path cfile = path(it);
      if ( exists(cfile) && is_regular_file(cfile) ) {
        if ( ft->transmit(cfile) ) {
          cout << "SUCCESS: " << cfile << endl;
        } else {
          cout << "FAIL: " << cfile << endl;
        }
      } else {
        cout << "WARN: File " << cfile.filename() << " not found" << endl;
      }
    } catch (exception& e) {
      cout << e.what() << endl;
    }
  }

  return 0;
}

