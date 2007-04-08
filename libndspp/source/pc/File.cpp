#include "File.h"
#include <string>
#include <stdio.h>
#include <unistd.h>

using namespace nds;

class nds::FileImplementation
{
  public:
    FileImplementation();
    void open(const char * name, const char * mode);
    ~FileImplementation();

    int read(char * buffer, int amount=-1);
    int size();
    bool is_open();
    void close();

  private:
    FILE * m_stream;
  
};

// delegate class:
FileImplementation::FileImplementation(): m_stream(0)
{}

void FileImplementation::open(const char * name, const char * mode)
{
  if (m_stream) {
    close();
  }
  m_stream = fopen(name, mode);
}


FileImplementation::~FileImplementation()
{
  if (m_stream)
    fclose(m_stream);
}

int FileImplementation::read(char * buffer, int amount)
{
  if (not m_stream)
  {
    return 0;
  }

  int bytesExpected = amount;
  if (amount < 0) {
    // read all bytes
    rewind(m_stream);
    bytesExpected = this->size();
  }
  return fread(buffer, 1, bytesExpected, m_stream);
}

int FileImplementation::size()
{
  if (not m_stream)
    return 0;

  int current = ftell(m_stream);
  fseek(m_stream, 0, SEEK_END);
  int fileSize = ftell(m_stream);
  fseek(m_stream, current, SEEK_SET);
  return fileSize;
}

bool FileImplementation::is_open()
{
  return m_stream != 0;
}
void FileImplementation::close()
{
  if (m_stream) 
    fclose(m_stream);
  m_stream = 0;
}

// File proxy as follows:
File::File():m_details(new FileImplementation())
{}

void File::open(const char * name, const char * mode)
{
  // append name to cwd to emulate FAT behaviour.
  char * dirname = get_current_dir_name();
  std::string fullpath(dirname);
  free(dirname);
  fullpath += "/";
  fullpath += name;
  m_details->open(name, mode);
}

File::~File()
{
  delete m_details;
}

int File::read(char * buffer, int amount)
{
  return m_details->read(buffer, amount);
}

int File::size()
{
  return m_details->size();
}

bool File::is_open()
{
  return m_details->is_open();
}

void File::close()
{
  m_details->close();
}
