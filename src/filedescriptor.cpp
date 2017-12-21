#include "simppl/filedescriptor.h"

#include <cassert>

#include <unistd.h>


namespace simppl
{
   
namespace dbus
{


FileDescriptor::FileDescriptor()
 : fd_(-1)
{
   // NOOP
}


FileDescriptor::FileDescriptor(int fd)
 : fd_(fd)
{
   assert(fd >= 0);
}

   
FileDescriptor::~FileDescriptor()
{
   if (fd_ != -1)
      close(fd_);
}

   
FileDescriptor::FileDescriptor(const FileDescriptor& rhs)
 : fd_(-1)
{
   if (rhs.fd_ != -1)
      fd_ = dup(rhs.fd_);
}


FileDescriptor& FileDescriptor::operator=(const FileDescriptor& rhs)
{
   if (this != &rhs)
   {
      if (rhs.fd_ != -1)
      {
         fd_ = dup(rhs.fd_);
      }
      else
         fd_ = -1;
   }
   
   return *this;
}


FileDescriptor::FileDescriptor(FileDescriptor&& rhs)
{
   fd_ = rhs.fd_;
   rhs.fd_ = -1;
}


FileDescriptor& FileDescriptor::operator=(FileDescriptor&& rhs)
{
   fd_ = rhs.fd_;
   rhs.fd_ = -1;
   
   return *this;
}


int FileDescriptor::native_handle() const
{
   return fd_;
}


int FileDescriptor::release()
{
   int fd = fd_;
   fd_ = -1;
   
   return fd;
}


/*static*/ 
void FileDescriptorCodec::encode(DBusMessageIter& iter, const FileDescriptor& fd)
{
   int _fd = fd.native_handle();
   dbus_message_iter_append_basic(&iter, DBUS_TYPE_UNIX_FD, &_fd);
}


/*static*/ 
void FileDescriptorCodec::decode(DBusMessageIter& iter, FileDescriptor& fd)
{
   int _fd;
   dbus_message_iter_get_basic(&iter, &_fd);
   fd = FileDescriptor(_fd);
}


/*static*/ 
std::ostream& FileDescriptorCodec::make_type_signature(std::ostream& os)
{
   return os << DBUS_TYPE_UNIX_FD_AS_STRING;
}
   
   
}   // namespace dbus

}   // namespace simppl
