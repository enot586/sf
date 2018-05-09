#pragma once

#include <string>

#include "AbstractProtocol.h"

using namespace std;

class TestProtocol : public AbstractProtocol
{

public:
  TestProtocol(string host) : host_(host) { }
  virtual ~TestProtocol() {  }

  bool connect()  override { return true; }
  void disconnect() override {};
  bool isConnected() override { return true; }
  size_t get_buffer_size() override { return sizeof(buffer_); }

  bool send(const string& filename, const void* buffer, size_t offset, size_t size) override
  {
    filename_ = filename;
    offset_ = offset;
    size_ = size;
    memcpy(buffer_, buffer, size);

    return true;
  }

  bool receive(string& host, string& filename, unique_ptr<char>& buffer, size_t& offset, size_t& size) override
  {
    host = host_;
    filename = filename_;
    buffer.reset(new char[sizeof(buffer_)]);
    memcpy(buffer.get(), buffer_, sizeof(buffer_));
    offset = offset_;
    size = size_;

    return true;
  }

private:
  string host_;
  string filename_;
  char buffer_[1024];
  size_t offset_;
  size_t size_;

};