#ifndef SIMPPL_DETAIL_PARAMETERDEDUCTION_H
#define SIMPPL_DETAIL_PARAMETERDEDUCTION_H


#include <type_traits>
#include <tuple>
#include <functional>

#include "simppl/if.h"
#include "simppl/typelist.h"

#include "simppl/callstate.h"


namespace simppl
{
   
namespace dbus
{
   
namespace detail
{

template<typename T>
struct is_in
{
   enum { value = false };
};

template<typename T>
struct is_in<simppl::dbus::in<T>>
{
   enum { value = true };
};

template<typename T>
struct is_out
{
   enum { value = false };
};

template<typename T>
struct is_out<simppl::dbus::out<T>>
{
   enum { value = true };
};

// ---------------------------------------------------------------------
 
// flatten typelist into std::tuple
template<typename ListT, typename TupleT>
struct make_tuple_from_list;

template<typename HeadT, typename TailT, typename... T>
struct make_tuple_from_list<TypeList<HeadT, TailT>, std::tuple<T...>>
{
   typedef typename make_tuple_from_list<TailT, std::tuple<T..., HeadT>>::type type;
};

template<typename HeadT, typename... T>
struct make_tuple_from_list<TypeList<HeadT, NilType>, std::tuple<T...>>
{
   typedef std::tuple<T..., HeadT> type;
};

template<typename... T>
struct make_tuple_from_list<TypeList<NilType, NilType>, std::tuple<T...>>
{
   typedef std::tuple<T...> type;
};

template<typename... T>
struct make_tuple_from_list<void, std::tuple<T...>>
{
   typedef std::tuple<T...> type;
};

// ---------------------------------------------------------------------
 
// flatten typelist into std::function
template<typename ListT, typename FunctT>
struct make_function_from_list;

template<typename HeadT, typename TailT, typename... T>
struct make_function_from_list<TypeList<HeadT, TailT>, std::function<void(CallState, T...)>>
{
   typedef typename make_function_from_list<TailT, std::function<void(CallState, T..., HeadT)>>::type type;
};

template<typename HeadT, typename... T>
struct make_function_from_list<TypeList<HeadT, NilType>, std::function<void(CallState, T...)>>
{
   typedef std::function<void(CallState, T..., HeadT)> type;
};

template<typename... T>
struct make_function_from_list<TypeList<NilType, NilType>, std::function<void(CallState, T...)>>
{
   typedef std::function<void(CallState, T...)> type;
};

template<typename... T>
struct make_function_from_list<void, std::function<void(CallState, T...)>>
{
   typedef std::function<void(CallState, T...)> type;
};

template<typename HeadT, typename TailT, typename... T>
struct make_function_from_list<TypeList<HeadT, TailT>, std::function<void(T...)>>
{
   typedef typename make_function_from_list<TailT, std::function<void(T..., HeadT)>>::type type;
};

template<typename HeadT, typename... T>
struct make_function_from_list<TypeList<HeadT, NilType>, std::function<void(T...)>>
{
   typedef std::function<void(T..., HeadT)> type;
};

template<typename... T>
struct make_function_from_list<TypeList<NilType, NilType>, std::function<void(T...)>>
{
   typedef std::function<void(T...)> type;
};

template<typename... T>
struct make_function_from_list<void, std::function<void(T...)>>
{
   typedef std::function<void(T...)> type;
};

// ---------------------------------------------------------------------

// remove tuple<> if just one type is contained
template<typename... T>
struct canonify;

template<typename T>
struct canonify<T>
{
   typedef T type;
};

template<typename... T>
struct canonify<std::tuple<T...>>
{
   typedef std::tuple<T...> type;
};

template<typename T>
struct canonify<std::tuple<T>>
{
   typedef T type;
};

template<>
struct canonify<std::tuple<>>
{
   typedef void type;
};

// ---------------------------------------------------------------------

template<template<typename> class FilterFunc, typename... T> 
struct filter;

template<template<typename> class FilterFunc, typename T1, typename... T> 
struct filter<FilterFunc, T1, T...>
{   
   typedef typename std::conditional<FilterFunc<T1>::value, typename T1::real_type, NilType>::type first_type;
   typedef typename PushFront<typename filter<FilterFunc, T...>::list_type, first_type>::type list_type;
};

template<template<typename> class FilterFunc, typename T> 
struct filter<FilterFunc, T>
{   
   typedef typename std::conditional<FilterFunc<T>::value, typename T::real_type, NilType>::type first_type;
   typedef typename make_typelist<first_type>::type list_type;
};

template<template<typename> class FilterFunc> 
struct filter<FilterFunc>
{   
   typedef void list_type;
};

// ---------------------------------------------------------------------

template<typename... T> 
struct generate_return_type
{
   typedef typename filter<is_out, T...>::list_type list_type;
   typedef typename make_tuple_from_list<list_type, std::tuple<>>::type type;
};

template<typename... T> 
struct generate_argument_type
{
   typedef typename filter<is_in, T...>::list_type list_type;
   typedef typename make_tuple_from_list<list_type, std::tuple<>>::type type;
};

template<typename... T>
struct generate_callback_function
{
   typedef typename filter<is_out, T...>::list_type list_type;
   typedef typename make_function_from_list<list_type, std::function<void(CallState)>>::type type;
};

template<typename... T>
struct generate_server_callback_function
{
   typedef typename filter<is_in, T...>::list_type list_type;
   typedef typename make_function_from_list<list_type, std::function<void()>>::type type;
};

// ---------------------------------------------------------------------

template<typename T>
struct gen_dummy_return_value
{
   static inline 
   T eval()
   {
      return T();
   }
};

template<>
struct gen_dummy_return_value<void>
{
   static inline 
   void eval()
   {
      // NOOP
   }
};


template<typename... ArgsT>
struct is_oneway_request
{
   enum { value = Find<simppl::dbus::Oneway, typename make_typelist<ArgsT...>::type>::value != -1 };
};



// --- introspection stuff ---------------------------------------------


template<typename... T>
struct ForEach;

template<typename T1, typename... T>
struct ForEach<T1, T...>
{
   template<typename FuncT>
   static void eval(int i, FuncT& f)
   {
      f.template eval<T1>(i);
      ForEach<T...>::eval(++i, f);
   }
};

template<typename T>
struct ForEach<T>
{
   template<typename FuncT>
   static void eval(int i, FuncT& f)
   {
      f.template eval<T>(i);
   }
};

template<>
struct ForEach<>
{
   template<typename FuncT>
   static void eval(int i, FuncT& f)
   {
      // NOOP
   }
};


template<typename T>
struct GetRealType
{
   typedef T type;
};

template<typename T>
struct GetRealType<in<T>>
{
   typedef T type;
};

template<typename T>
struct GetRealType<out<T>>
{
   typedef T type;
};


struct Introspector
{
   Introspector(std::ostream& os)
    : os_(os)
   {
      // NOOP
   }
   
   template<typename T>
   void eval(int i)
   {
      if (std::is_same<T, Oneway>::value == 0)
      {
         os_ << "<arg name=\"arg" << i << "\" type=\"";
         make_type_signature<typename GetRealType<T>::type>::eval(os_);
         os_ << "\" direction=\"" << (is_in<T>::value?"in":"out") << "\"/>\n";
      }
   }
   
   void eval(int, Oneway)
   {
      // NOOP
   }
   
   std::ostream& os_;
};


template<typename... T>
void introspect_args(std::ostream& os)
{
   Introspector i(os);
   int cnt = 0;
   ForEach<T...>::eval(cnt, i);
}


}   // namespace detail

}   // namespace dbus

}   // namespace simppl


#endif   // SIMPPL_DETAIL_PARAMETERDEDUCTION_H