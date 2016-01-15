#ifndef SIMPPL_STUBBASE_H
#define SIMPPL_STUBBASE_H


#include <cassert>
#include <functional>
#include <map>

#include <dbus/dbus.h>

#include "simppl/detail/constants.h"
#include "simppl/detail/parented.h"

#include "simppl/connectionstate.h"


namespace simppl
{

namespace ipc
{

// forward decls
struct Dispatcher;
struct ClientResponseBase;
struct ClientSignalBase;

namespace detail
{
   struct Serializer;
};


struct StubBase
{
   template<typename... T> friend struct ClientSignal;
   template<typename... T> friend struct ClientRequest;
   template<typename, typename> friend struct ClientAttribute;
   friend struct Dispatcher;

protected:

   virtual ~StubBase();

public:

   std::function<void(ConnectionState)> connected;

   StubBase(const char* iface, const char* role, const char* boundname);

   inline
   const char* iface() const
   {
      return iface_;
   }

   inline
   const char* role() const
   {
      return role_;
   }

   Dispatcher& disp();

   /// FIXME protected?!
   DBusHandlerResult try_handle_signal(DBusMessage* msg);

   inline
   const char* destination()
   {
       return boundname_;
   }

   DBusConnection* conn();

protected:

   void sendSignalRegistration(ClientSignalBase& sigbase);
   void sendSignalUnregistration(ClientSignalBase& sigbase);

   void getProperty(const char* name, void(*callback)(DBusPendingCall*, void*), void* user_data);

   char iface_[128];
   const char* role_;

   char boundname_[80];     ///< where to find the server

   Dispatcher* disp_;
   std::map<std::string, ClientSignalBase*> signals_;
};

}   // namespace ipc

}   // namespace simppl


#endif   // SIMPPL_STUBBASE_H
