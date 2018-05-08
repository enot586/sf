#include <iostream>
#include <string>
#include <vector>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include "client.h"
#include "FileTransmitter.h"
#include "SharedMemoryProtocol.h"
#include "namer.h"

using namespace std;
using namespace boost::program_options;
using namespace boost::filesystem;

int main( int argc, char *argv[] )
{
  Namer namer;

  try  {
    vector<string> file_list = check_arguments(argc, argv);

    SharedMemoryProtocol protocol( namer.GetSharedMemoryName(),
                                   to_string( reinterpret_cast<long>(main) ) );
    FileTransmitter ft( protocol );

    for (auto it : file_list) {
      try {
        path cfile = path(it);
        if ( exists(cfile) && is_regular_file(cfile) ) {
          if ( ft.transmit(cfile) ) {
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

vector<string> check_arguments(int argc, char* argv[])
{
  options_description desc("Files transmition to server part");

  desc.add_options()
      ("help", "produce help message")
      ("target,t", value< vector<string> >()->multitoken(), "list of target files for transmit");

  variables_map vm;
  store( command_line_parser(argc, argv).options(desc).run(), vm);
  notify(vm);

  if ( vm.count("help") ) {
    cout << desc << endl;
    exit(0);
  }

  if ( !vm.count("target") || (argc < 2) ) {
    throw boost::program_options::error("Nothing to send");
  }

  const vector<string>& file_list = vm["target"].as< vector<string> >();

  if ( file_list.empty() ) {
    throw boost::program_options::error("Nothing to send");
  }

  return file_list;
}
