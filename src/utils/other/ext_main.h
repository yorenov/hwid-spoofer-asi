#pragma once

#include <iostream>

namespace std
{
	template<class _Ty, class _To>
	struct delegate_qualifiers
	{
		using type = _To;
	};

	template<class _Ty, class _To>
	struct delegate_qualifiers<_Ty const, _To>
	{
		using type = const _To;
	};

	template<class _Ty, class _To>
	struct delegate_qualifiers<_Ty volatile, _To>
	{
		using type = volatile _To;
	};

	template<class _Ty, class _To>
	struct delegate_qualifiers<_Ty const volatile, _To>
	{
		using type = const volatile _To;
	};

	template<class _Ty, class _To>
	struct delegate_qualifiers<_Ty*, _To>
	{
		using type = delegate_qualifiers<_Ty, _To>::type;
	};


	template<class _Ty, class _To>
	using delegate_qualifiers_t = typename delegate_qualifiers<_Ty, _To>::type;

	//////////////////////

	template<class _Ty>
	struct remove_all_qualifiers
	{
		using type = _Ty;
	};

	template<class _Ty>
	struct remove_all_qualifiers<_Ty const>
	{
		using type = _Ty;
	};

	template<class _Ty>
	struct remove_all_qualifiers<_Ty volatile>
	{
		using type = _Ty;
	};

	template<class _Ty>
	struct remove_all_qualifiers<_Ty const volatile>
	{
		using type = _Ty;
	};

	template<class _Ty>
	struct remove_all_qualifiers<_Ty*>
	{
		using type = remove_all_qualifiers<_Ty>::type*;
	};

	template<class _Ty>
	using remove_all_qualifiers_t = typename remove_all_qualifiers<_Ty>::type;

	template<typename T>
	constexpr std::remove_all_qualifiers<T>::type remove_qualifiers(T var) noexcept
	{
		return const_cast<std::remove_all_qualifiers_t<T>>(var);
	}

	////////////////
	template<class _Ty>
	struct to_default
	{
		using type = remove_all_qualifiers<_Ty>::type;
	};

	template<class _Ty>
	struct to_default<_Ty*>
	{
		using type = to_default<_Ty>::type;
	};

	template<class _Ty>
	using to_default_t = typename to_default<_Ty>::type;


	///////////////////


	inline void* malloc_s(const size_t allocSize)
	{
		void* ptr = malloc(allocSize);

		if (ptr == nullptr)
		{
			throw std::exception("Failed to allocate memory (using malloc).\n");
		}

		return ptr;
	}

	inline void* calloc_s(const size_t blocksCount, const size_t allocSize)
	{
		void* ptr = calloc(blocksCount, allocSize);

		if (ptr == nullptr)
		{
			throw std::exception("Failed to allocate memory (using calloc).\n");
		}

		return ptr;
	}

	template<class ReqT, class SrcT>
	constexpr ReqT offset(SrcT pointer, const uintptr_t offset) noexcept
	{
		static_assert(std::is_pointer_v<ReqT> , "Required type isn't a pointer type!");
		static_assert(std::is_pointer_v<SrcT>, "Source type isn't a pointer type!");

		return reinterpret_cast<ReqT>(reinterpret_cast<uintptr_t>(std::remove_qualifiers(pointer)) + offset);
	}

	template<class T>
	constexpr T offset(T pointer, const uintptr_t offset) noexcept
	{
		return std::offset<T, T>(pointer, offset);
	}

	template<class T>
	constexpr uintptr_t get_offset(T pointer, const uintptr_t offset) noexcept
	{
		static_assert(std::is_pointer_v<T>, "It isn't a pointer type!");

		return reinterpret_cast<uintptr_t>(std::remove_qualifiers(pointer)) + offset;
	}
}