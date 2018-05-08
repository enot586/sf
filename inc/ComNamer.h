#pragma once

#include <iostream>

#include "AbstractResource.h"

class ComNamer :
  public AbstractResource
{
public:
  ComNamer();
  virtual ~ComNamer();

  virtual std::string GetSharedMemoryName() override;
private:
  std::string shared_memory_name;
};

