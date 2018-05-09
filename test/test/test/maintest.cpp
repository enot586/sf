#define BOOST_TEST_MAIN

#include <memory>
#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>
#include <thread>
#include <chrono>
#include <future>  

#include "TestProtocol.h"
#include "FileTransmitter.h"
#include "SharedMemoryProtocol.h"

using namespace std;

BOOST_AUTO_TEST_CASE(TestFileTransmitter)
{
  const string test_host("test_host");

  //!!! real file in project directory !!!
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
  const string server_host("test_server_host");
  const string client_host("test_client_host");
  const string name("test_shared_memory");
  const string test_file("any_file_name");

  SharedMemoryProtocol server_protocol(name, server_host, true);
  SharedMemoryProtocol client_protocol(name, client_host);

  BOOST_CHECK( server_protocol.connect() );
  BOOST_CHECK( client_protocol.connect() );

  char buffer[100] = {0};

  string receive_host;
  string receive_filename;
  unique_ptr<char[]> receive_buffer;
  size_t receive_offset;
  size_t receive_size;
 
  using namespace std::chrono;

  std::future<bool> server_receive = std::async( &SharedMemoryProtocol::receive, std::ref(server_protocol),
                                                  std::ref(receive_host), std::ref(receive_filename), 
                                                  std::ref(receive_buffer), std::ref(receive_offset), 
                                                  std::ref(receive_size) );
  this_thread::sleep_for( seconds(1) );

  BOOST_CHECK( client_protocol.send( test_file, buffer, 0, sizeof(buffer) ) );

  BOOST_CHECK( server_receive.get() );
  
  BOOST_CHECK(receive_host == client_host);
  BOOST_CHECK(receive_filename == test_file);
  BOOST_CHECK(receive_offset == 0);
  BOOST_CHECK( receive_size == sizeof(buffer) );
  BOOST_CHECK( !memcmp( receive_buffer.get(), buffer, sizeof(buffer) ) );
}



