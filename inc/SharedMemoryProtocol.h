#ifndef SHAREDMEMORYPROTOCOL_H
#define SHAREDMEMORYPROTOCOL_H

#include <string>
#include <memory>
#include <utility>
#include <iostream>

#include <boost/interprocess/windows_shared_memory.hpp>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>

#include "AbstractProtocol.h"
#include "SharedMemoryBuffer.h"

#define WINDOWS

using namespace std;
using namespace boost::interprocess;

class SharedMemoryProtocol : public AbstractProtocol
{
public:
  //for client
  SharedMemoryProtocol(const std::string& name, const std::string& host, bool isServer=false);
  ~SharedMemoryProtocol();

  virtual bool connect() override;
  virtual void disconnect() override;
  virtual bool isConnected() override;
  virtual bool send(const string& filename, const void* buffer, size_t offset, size_t size) override;
  virtual bool receive(string& host, string& filename, unique_ptr<char>& buffer, size_t& offset, size_t& size) override;
  virtual size_t get_buffer_size() override;

private:
  string host_;
  string filename_;
  string name_;
  bool isServer_;

#ifdef WINDOWS
  unique_ptr<windows_shared_memory> p_;
#else
  unique_ptr<shared_memory_object> p_;
#endif

};

#endif // SHAREDMEMORYPROTOCOL_H
