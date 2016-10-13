#pragma once

namespace oak { namespace mpl {

	struct nulltype {};

	template<typename H, typename T>
	struct type_list {
		using first = H;
		using second = T;
	};

	template<typename...>
	struct make_list;

	template<typename T>
	struct make_list<T> {
		using type = type_list<T, nulltype>;
	};

	template<typename T, typename U, typename... TList>
	struct make_list<T, U, TList...> {
		using type = type_list<T, typename make_list<U, TList...>::type>;
	};

	template<typename... TArgs>
	using list = typename make_list<TArgs...>::type;

	template<typename F, typename S>
	struct match_type{
		static constexpr bool value = false;
	};

	template<typename T>
	struct match_type<T, T> {
		static constexpr bool value = true;
	};

	template<typename TList, typename T>
	struct contains{
		static constexpr bool value = match_type<typename TList::first, T>::value || contains<typename TList::second, T>::value;
	};

	template<typename T>
	struct contains<nulltype, T> {
		static constexpr bool value = false;
	};

	template<bool cond, typename T=void>
	struct enable_if {
		
	};

	template<typename T>
	struct enable_if<true, T> {
		using type = T;
	};

	template<typename TList, typename T>
	struct index_of {
		static constexpr size_t value = match_type<typename TList::first, T>::value ? 0 : 1 + index_of<typename TList::second, T>::value;
	};

	template<typename T>
	struct index_of<nulltype, T> {
		static constexpr size_t value = 0;	
	};

	template<typename TList, typename... TArgs>
	struct append_list {
		using type = type_list<typename TList::first, typename append_list<typename TList::second, TArgs...>::type>;
	};

	template<typename T, typename... TArgs>
	struct append_list<nulltype, T, TArgs...> {
		using type = type_list<T, typename make_list<TArgs...>::type>;
	};

	template<typename T>
	struct append_list<nulltype, T> {
		using type = type_list<T, nulltype>;
	};

	template<typename TList, typename... TArgs>
	using append = typename append_list<TList, TArgs...>::type;

} }