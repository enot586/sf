#ifndef SHAREDMEMORYBUFFER
#define SHAREDMEMORYBUFFER

#include <string>
#include <memory>
#include <vector>

#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>

using namespace std;

//#pragma pack(push, 1)
struct SharedMemoryBuffer
{
  char host[20];
  char filename[255];
  size_t offset;
  size_t size;

  boost::interprocess::interprocess_mutex      write_mutex;
  boost::interprocess::interprocess_mutex      read_mutex;
  boost::interprocess::interprocess_mutex      cond_mutex;
  boost::interprocess::interprocess_condition  start_read;

  char byte_array[1024*1024];
};
//#pragma pack(pop)

#endif // SHAREDMEMORYBUFFER

