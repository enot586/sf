#ifndef FILETRANSMITTER_H
#define FILETRANSMITTER_H

#include <iostream>
#include <vector>
#include <string>

#include <boost/filesystem.hpp>

#include "AbstractProtocol.h"
#include "AbstractResource.h"


using namespace std;
using namespace boost::filesystem;


class FileTransmitter
{
public:
  FileTransmitter(AbstractProtocol& protocol);
  ~FileTransmitter();

  bool transmit(const path& file_name);
  bool receive();

private:
  AbstractProtocol& protocol_;
};

#endif // FILETRANSMITTER_H
