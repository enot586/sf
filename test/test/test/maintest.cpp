#define BOOST_TEST_MAIN

#include <memory>
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>

#include "TestProtocol.h"
#include "FileTransmitter.h"
#include "SharedMemoryProtocol.h"

using namespace std;

BOOST_AUTO_TEST_CASE(TestFileTransmitter)
{
  const string test_host("test_host");
  const string test_file("test_file.bin");

  TestProtocol protocol(test_host);
  FileTransmitter ft( protocol );
  
  string out(test_host);
  out += "/";
  out += test_file;

  path output(out);
  path input(test_file);

  if ( exists(output) ) {
    remove(output);
    BOOST_CHECK( !exists(output) );
  }

  BOOST_CHECK( exists(input) && is_regular_file(input) );
  BOOST_CHECK( ft.transmit(input) );
  BOOST_CHECK( ft.receive() );

  BOOST_CHECK( exists(output) );
  BOOST_CHECK( file_size(input) == file_size(output) );

  string remover(test_host);
  remover += "/";
  remove_all(remover);
}

BOOST_AUTO_TEST_CASE(TestSharedMemoryProtocol)
{

}



