#include <iostream>
#include <fstream>
#include <thread>

#include <boost/interprocess/exceptions.hpp>
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
    std::fstream fs(file_name.c_str(), ios::binary | ios::in);
    std::unique_ptr<char> buffer(new char[portion]);

    size_t offset = 0;
    while ( offset != file_size(file_name) ) {
      fs.read(buffer.get(), portion);
      size_t tx_data = fs.gcount();
      if ( protocol_.send(file_name.string(), buffer.get(), offset, tx_data) ) {
        offset+= tx_data;
      }
    }
  } 
  catch (std::bad_alloc&) {
    cout << "ERROR: Out of memory for file buffers" << endl;
    throw;
  } 
  catch (boost::filesystem::filesystem_error&) {
    cout << "ERROR: Failed access to file" << endl;
    throw;
  }
  catch (boost::interprocess::interprocess_exception&) {
    cout << "ERROR: Server has been losted." << endl;
    throw;
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
        cout << "from: "<< host << " receive part of: " << filename << " offset: " << offset << " size: " << size << endl;
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
    std::ofstream f( p.c_str(), ios::binary );
    f.seekp(offset);
    f.write((char*)buffer, size);
    f.close();
  } else {
     std::fstream f(p.c_str(), ios::binary | ios::out | ios::in);
     f.seekp(offset, ios::beg);
     f.write((char*)buffer, size);
     f.close();
  }

  return true;
}
