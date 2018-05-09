#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/windows_shared_memory.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/sharable_lock.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>

#include "SharedMemoryProtocol.h"

using namespace std;
using namespace boost::interprocess;

SharedMemoryProtocol::SharedMemoryProtocol(const std::string& name, const std::string& host, bool isServer) :
  host_(host), name_(name), isServer_(isServer)
{
  cout << ((isServer_) ? "Server" : "Client") << endl;
  cout << "name: " << name_ << endl;
  cout << "host: " << host_ << endl;
}

SharedMemoryProtocol::~SharedMemoryProtocol()
{
#ifndef WINDOWS
	if (isServer_) {
		shared_memory_object::remove( name_.c_str() );
	}
#endif
}

bool SharedMemoryProtocol::connect()
{
	try {
		if (isServer_) {

#ifdef WINDOWS
			p_ = make_unique<windows_shared_memory>( create_only, name_.c_str(), read_write, sizeof(SharedMemoryBuffer) );
#else
     shared_memory_object::remove( name_.c_str() );
     p_ = make_unique<shared_memory_object>(create_only, name_.c_str(), read_write);
     p_->truncate( sizeof(SharedMemoryBuffer) );
#endif

			mapped_region region(*p_, read_write);
			void* addr = region.get_address();

      SharedMemoryBuffer* pBuffer = new (addr) SharedMemoryBuffer;
		} else {
#ifdef WINDOWS
		p_ = make_unique<windows_shared_memory>( open_only, name_.c_str(), read_write );

    mapped_region region(*p_, read_write);
    SharedMemoryBuffer* pBuffer = (SharedMemoryBuffer*)region.get_address();
#else
    p_ = make_unique<shared_memory_object>(open_only, name_.c_str(), read_write);
#endif
		}
	} catch(exception&) {
		cout << "ERROR: Out of memory" << endl;
		throw;
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
    SharedMemoryBuffer* pBuffer = (SharedMemoryBuffer*)region.get_address();

    pBuffer->write_mutex.lock();
    while (!pBuffer->isReadyForData) {};

    if ( (get_buffer_size() < size) ||
         ( host_.length() >= sizeof(pBuffer->host) ) ||
         ( filename.length() >= sizeof(pBuffer->filename) ) )
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

bool SharedMemoryProtocol::receive(string& host, string& filename, unique_ptr<char[]>& buffer, size_t& offset, size_t& size)
{
  mapped_region region(*p_, read_write);
  SharedMemoryBuffer* pBuffer = (SharedMemoryBuffer*)region.get_address();

  pBuffer->isReadyForData = true;
  while (pBuffer->isReadyForData) {};

  if (pBuffer->size) {
    host.assign( pBuffer->host );
    filename.assign( pBuffer->filename );
    offset    = pBuffer->offset;
    size      = pBuffer->size;

    buffer.reset( new char[size] );
    memcpy(buffer.get(), pBuffer->byte_array, pBuffer->size);

    pBuffer->write_mutex.unlock();

    return true;
  }
  
  return false;
}

size_t SharedMemoryProtocol::get_buffer_size()
{
  return SharedMemoryBuffer::get_size();
}
