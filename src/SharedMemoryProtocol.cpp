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
  p_->truncate(sizeof(SharedMemoryBuffer)+size);
}

SharedMemoryProtocol::~SharedMemoryProtocol()
{
  if (p_.get() != nullptr)
    shared_memory_object::remove( p_->get_name() );
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
    SharedMemoryBuffer* pBuffer = new (addr) SharedMemoryBuffer;

    //wait until server read the data
    pBuffer->read_mutex.lock();
    scoped_lock<interprocess_mutex> lock(pBuffer->write_mutex);

    if (get_buffer_size() < size)
      return false;

    pBuffer->host     = host_;
    pBuffer->filename = filename;
    pBuffer->offset   = offset;
    pBuffer->size     = size;

    memcpy(&pBuffer->byte_array, (char*)buffer+offset, size);
    pBuffer->start_read.notify_one();
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
    SharedMemoryBuffer* pBuffer = new (addr) SharedMemoryBuffer;

    boost::interprocess::interprocess_mutex m;
    scoped_lock<interprocess_mutex> read_lock(m);
    pBuffer->start_read.wait(read_lock);

    scoped_lock<interprocess_mutex> write_lock(pBuffer->write_mutex);
    if (pBuffer->size) {
      if (pBuffer->size > size)
        return false;

      host      = pBuffer->host;
      filename  = pBuffer->filename;
      offset    = pBuffer->offset;
      size      = pBuffer->size;

      buffer = unique_ptr<char>(new char[size]);
      memcpy(buffer.get(), pBuffer->byte_array, pBuffer->size);

      pBuffer->read_mutex.unlock();
    }

  } catch (exception&) {
    return false;
  }

  return true;
}

size_t SharedMemoryProtocol::get_buffer_size()
{
  offset_t s = 0;
  p_->get_size(s);

  offset_t service_size = (sizeof(SharedMemoryBuffer) - sizeof(char*) );
  if ( s > service_size)
    return (s - service_size);

  return 0;
}
