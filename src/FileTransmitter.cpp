#include <iostream>
#include <fstream>
#include <thread>

#include <boost/filesystem.hpp>
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
      try {
        cout << "h: "<< host << "f: " << filename << "o: " << offset << " s: " << size << endl;
        return write_to_file(host, filename, buffer.get(), offset, size);
      } catch (exception&) {
        cout << "ERROR: Cant\'t write file !" << endl;
      }
    }
  } catch(exception&) {
    return false;
  }

  return false;
}

bool FileTransmitter::write_to_file(const string& host, const string& filename,
                                    void* buffer, const size_t offset,
                                    const size_t size)
{
  path dir( host.c_str() );

  if ( !(exists(dir) && is_directory(dir) ) ) {
    create_directory(dir);
  }

  path p(dir);
  p += "/";
  p += filename;

  if ( !(exists(p) && is_regular_file(p) ) ) {
    ofstream f( p.c_str(), ios::binary );
    f.seekp(offset);
    f.write((char*)buffer, size);
    f.close();
  } else {
     fstream f(p.c_str(), ios::binary | ios::out | ios::in);
     f.seekp(offset, ios::beg);
     f.write((char*)buffer, size);
     f.close();
  }

  return true;
}
