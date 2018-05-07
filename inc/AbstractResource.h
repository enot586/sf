#ifndef RESOURCE_H
#define RESOURCE_H

#include <string>

class AbstractResource
{
public:
  virtual std::string GetSharedMemoryName() = 0;
};

#endif // RESOURCE_H
