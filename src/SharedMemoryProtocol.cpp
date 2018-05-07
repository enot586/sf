#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/sharable_lock.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>

#include "SharedMemoryProtocol.h"

using namespace std;
using namespace boost::interprocess;

SharedMemoryProtocol::SharedMemoryProtocol(const std::string& name, const std::string& host) :
  host_(host), name_(name)
{
  cout << "name: " << name_ << endl;
  cout << "host: " << host_ << endl;
}

SharedMemoryProtocol::SharedMemoryProtocol(const std::string& name, const std::string& host, size_t size) :
  host_(host), name_(name)
{
  cout << "name: " << name_ << endl;
  cout << "host: " << host_ << endl;

  shared_memory_object::remove( name_.c_str() );

  p_ = std::make_unique<shared_memory_object>( create_only, name_.c_str(), read_write );
  p_->truncate( std::max(sizeof(SharedMemoryBuffer), size) );

  mapped_region region(*p_, read_write);
  void* addr = region.get_address();

  /*__attribute__((unused))*/ SharedMemoryBuffer* pBuffer = new (addr) SharedMemoryBuffer;
}

SharedMemoryProtocol::~SharedMemoryProtocol()
{

}

bool SharedMemoryProtocol::connect()
{
  cout << "shm name: " << name_.c_str() << endl;
  try {
    p_ = std::make_unique<shared_memory_object>( open_only, name_.c_str(), read_write );
  } catch(exception&) {
    cout << "ERROR: Out of memory" << endl;
    return false;
  }

  return (p_.get() != nullptr);
}

void SharedMemoryProtocol::disconnect()
{

}

bool SharedMemoryProtocol::isConnected()
{
  return (p_.get() != nullptr);
}

bool SharedMemoryProtocol::send(const string& filename, const void* buffer, size_t offset, size_t size)
{
  try {
    mapped_region region(*p_, read_write);
    void* addr = region.get_address();
    SharedMemoryBuffer* pBuffer = (SharedMemoryBuffer*) addr;

    pBuffer->write_mutex.lock();
    while (!pBuffer->isReadyForData) {};

    if (get_buffer_size() < size)
      return false;

    memset( pBuffer->host, 0x00, sizeof( pBuffer->host) );
    memcpy( pBuffer->host, host_.c_str(), host_.length() );

    memset( pBuffer->filename, 0x00, sizeof(pBuffer->filename) );
    memcpy( pBuffer->filename, filename.c_str(), filename.length() );

    memcpy( pBuffer->byte_array, (char*)buffer, size);
    pBuffer->offset   = offset;
    pBuffer->size     = size;

    pBuffer->isReadyForData = false;
  } catch (exception&) {
    return false;
  }

  return true;
}

bool SharedMemoryProtocol::receive(string& host, string& filename, unique_ptr<char>& buffer, size_t& offset, size_t& size)
{
  try {
    mapped_region region(*p_, read_write);
    void* addr = region.get_address();
    SharedMemoryBuffer* pBuffer = (SharedMemoryBuffer*) addr;

    pBuffer->isReadyForData = true;
    while (pBuffer->isReadyForData) {};

    if (pBuffer->size) {
      host.assign( pBuffer->host );
      filename.assign( pBuffer->filename );
      offset    = pBuffer->offset;
      size      = pBuffer->size;

      buffer = unique_ptr<char>(new char[size]);
      memcpy(buffer.get(), pBuffer->byte_array, pBuffer->size);

      pBuffer->write_mutex.unlock();
    }
  } catch (exception&) {
    return false;
  }

  return true;
}

size_t SharedMemoryProtocol::get_buffer_size()
{
  return SharedMemoryBuffer::get_size();
}
