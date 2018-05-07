#ifndef NAMER
#define NAMER

#include <string>

#include "AbstractResource.h"

using namespace std;

class Namer : AbstractResource
{
public:
  virtual std::string GetSharedMemoryName() override
  {
    return string("hello");
  }

};

#endif // NAMER

