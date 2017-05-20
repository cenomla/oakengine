#pragma once

#include <cstddef>

namespace oak::mpl {

	struct nulltype {};

	template<class H, class T>
	struct type_list {
		using first = H;
		using second = T;
	};

	template<class...>
	struct make_list;

	template<class T>
	struct make_list<T> {
		using type = type_list<T, nulltype>;
	};

	template<class T, class U, class... TList>
	struct make_list<T, U, TList...> {
		using type = type_list<T, typename make_list<U, TList...>::type>;
	};

	template<class... TArgs>
	using list = typename make_list<TArgs...>::type;

	template<typename F, typename S>
	struct match_type{
		static constexpr bool value = false;
	};

	template<class T>
	struct match_type<T, T> {
		static constexpr bool value = true;
	};

	template<class TList, class T>
	struct contains{
		static constexpr bool value = match_type<typename TList::first, T>::value || contains<typename TList::second, T>::value;
	};

	template<class T>
	struct contains<nulltype, T> {
		static constexpr bool value = false;
	};

	template<bool cond, class T=void>
	struct enable_if {
		
	};

	template<class T>
	struct enable_if<true, T> {
		using type = T;
	};

	template<class TList, class T>
	struct index_of {
		static constexpr size_t value = match_type<typename TList::first, T>::value ? 0 : 1 + index_of<typename TList::second, T>::value;
	};

	template<class T>
	struct index_of<nulltype, T> {
		static constexpr size_t value = 0;	
	};

	template<class TList, class... TArgs>
	struct append_list {
		using type = type_list<typename TList::first, typename append_list<typename TList::second, TArgs...>::type>;
	};

	template<class T, class... TArgs>
	struct append_list<nulltype, T, TArgs...> {
		using type = type_list<T, typename make_list<TArgs...>::type>;
	};

	template<class T>
	struct append_list<nulltype, T> {
		using type = type_list<T, nulltype>;
	};

	template<class TList, class... TArgs>
	using append = typename append_list<TList, TArgs...>::type;

	template<class T, bool>
	struct get_type_const {
		using type = T;
	};

	template<class T>
	struct get_type_const<T, true> {
		using type = const T;
	};

	template<class T, class U>
	using const_type = typename get_type_const<U, std::is_const<T>::value>::type;

}