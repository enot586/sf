#ifndef ABSTRACTPROTOCOL_H
#define ABSTRACTPROTOCOL_H
#include <string>
#include <memory>

using namespace std;

struct AbstractProtocol
{
  virtual bool connect() = 0;
  virtual void disconnect() = 0;
  virtual bool isConnected() = 0;
  virtual bool send(const string& filename, const void* buffer, size_t offset, size_t size) = 0;
  virtual bool receive(string& host, string& filename, unique_ptr<char[]>& buffer, size_t& offset, size_t& size) = 0;
  virtual size_t get_buffer_size() = 0;
};

#endif // ABSTRACTPROTOCOL_H

