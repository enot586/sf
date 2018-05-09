#ifndef SHAREDMEMORYBUFFER
#define SHAREDMEMORYBUFFER

#include <string>
#include <memory>
#include <vector>
#include <atomic>

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>

using namespace std;

struct SharedMemoryBuffer
{
  char host[255];
  char filename[255];
  size_t offset;
  size_t size;

  atomic_bool isReadyForData;
  boost::interprocess::interprocess_mutex write_mutex;
  boost::interprocess::interprocess_mutex start_read;

  char byte_array[1024*1024];

  static size_t get_size()
  {
    return sizeof(byte_array);
  }

  SharedMemoryBuffer()
  {
    memset( host, 0x00, sizeof(host) );
    memset( filename, 0x00, sizeof(filename) );
    offset = 0;
    size = 0;
    isReadyForData = false;
  }
};

#endif // SHAREDMEMORYBUFFER

