#include <iostream>
#include <fstream>
#include <thread>

#include <boost/asio/ip/host_name.hpp>

#include "FileTransmitter.h"

FileTransmitter::FileTransmitter(AbstractProtocol& protocol):
  protocol_(protocol)
{
  protocol_.connect();
}

FileTransmitter::~FileTransmitter()
{
  protocol_.disconnect();
}

bool FileTransmitter::transmit(const path& file_name)
{
  try {
    size_t portion = protocol_.get_buffer_size();
    std::ifstream fs( file_name.c_str() );
    char buffer[portion];

    size_t offset = 0;
    while ( offset != file_size(file_name) ) {
      int tx_data = fs.readsome(buffer, portion);
      if ( protocol_.send(file_name.string(), buffer, offset, tx_data) ) {
        offset+= tx_data;
      }
    }
  } catch(exception&) {
    return false;
  }

  return true;
}

bool FileTransmitter::receive()
{
  try {
    string host;
    string filename;
    unique_ptr<char> buffer;
    size_t offset;
    size_t size;

    if ( protocol_.receive(host, filename, buffer, offset, size) ) {
      //TODO: create folder, file...
      return true;
    }
  } catch(exception&) {
    return false;
  }

  return false;
}
