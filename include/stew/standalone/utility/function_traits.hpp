/*
 * Copyright (C) 2024 bitWelder
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see
 * <http://www.gnu.org/licenses/>
 */

#ifndef STEW_FUNCTION_TRAITS_HPP
#define STEW_FUNCTION_TRAITS_HPP

#include <tuple>
#include <type_traits>

namespace stew
{

enum class FunctionType
{
    Functor,
    Method,
    Function
};

namespace detail
{

template <typename Functor>
struct functor_traits : public functor_traits<decltype(&Functor::operator())>
{
};

/// Method traits.
template <class TObject, typename TRet, typename... Args>
struct functor_traits<TRet(TObject::*)(Args...)>
{
    using object = void;
    using return_type = TRet;
    typedef TRet(TObject::*function_type)(Args...);

    static constexpr bool is_const = false;
    static constexpr FunctionType type = FunctionType::Functor;

    struct arg
    {
        using types = std::tuple<Args...>;
        static constexpr std::size_t arity = std::tuple_size_v<types>;

        template <std::size_t N>
        struct get
        {
            static_assert(N < arity, "error: invalid parameter index.");
            using type = typename std::tuple_element<N, types>::type;
        };
    };

    struct signature : arg
    {
    };
};

/// Const method traits.
template <class TObject, typename TRet, typename... Args>
struct functor_traits<TRet(TObject::*)(Args...) const>
{
    using object = void;
    using return_type = TRet;
    typedef TRet(TObject::*function_type)(Args...) const;

    static constexpr bool is_const = true;
    static constexpr FunctionType type = FunctionType::Functor;

    struct arg
    {
        using types = std::tuple<Args...>;
        static constexpr std::size_t arity = std::tuple_size_v<types>;

        template <std::size_t N>
        struct get
        {
            static_assert(N < arity, "error: invalid parameter index.");
            using type = typename std::tuple_element<N, types>::type;
        };
    };

    struct signature : arg
    {
    };
};

}

/// Traits for functors and function objects.
template <typename Function>
struct function_traits : public detail::functor_traits<decltype(&Function::operator())>
{
};

/// Method traits.
template <class TObject, typename TRet, typename... Args>
struct function_traits<TRet(TObject::*)(Args...)>
{
    using object = TObject;
    using return_type = TRet;
    typedef TRet(TObject::*function_type)(Args...);

    static constexpr bool is_const = false;
    static constexpr FunctionType type = FunctionType::Method;

    struct arg
    {
        using types = std::tuple<Args...>;
        static constexpr std::size_t arity = std::tuple_size_v<types>;

        template <std::size_t N>
        struct get
        {
            static_assert(N < arity, "error: invalid parameter index.");
            using type = typename std::tuple_element<N, types>::type;
        };
    };

    struct signature
    {
        using types = std::tuple<object*, Args...>;
        static constexpr std::size_t arity = std::tuple_size_v<types>;

        template <std::size_t N>
        struct get
        {
            static_assert(N < arity, "error: invalid parameter index.");
            using type = typename std::tuple_element<N, types>::type;
        };
    };
};

/// Const method traits.
template <class TObject, typename TRet, typename... Args>
struct function_traits<TRet(TObject::*)(Args...) const>
{
    using object = TObject const;
    using return_type = TRet;
    typedef TRet(TObject::*function_type)(Args...) const;

    static constexpr bool is_const = true;
    static constexpr FunctionType type = FunctionType::Method;

    struct arg
    {
        using types = std::tuple<Args...>;
        static constexpr std::size_t arity = std::tuple_size_v<types>;

        template <std::size_t N>
        struct get
        {
            static_assert(N < arity, "error: invalid parameter index.");
            using type = typename std::tuple_element<N, types>::type;
        };
    };

    struct signature
    {
        using types = std::tuple<object*, Args...>;
        static constexpr std::size_t arity = std::tuple_size_v<types>;

        template <std::size_t N>
        struct get
        {
            static_assert(N < arity, "error: invalid parameter index.");
            using type = typename std::tuple_element<N, types>::type;
        };
    };
};

/// Function and static member function traits.
template <typename TRet, typename... Args>
struct function_traits<TRet(*)(Args...)>
{
    using return_type = TRet;
    typedef TRet(*function_type)(Args...);

    static constexpr bool is_const = false;
    static constexpr FunctionType type = FunctionType::Function;

    struct arg
    {
        using types = std::tuple<Args...>;
        static constexpr std::size_t arity = std::tuple_size_v<types>;

        template <std::size_t N>
        struct get
        {
            static_assert(N < arity, "error: invalid parameter index.");
            using type = typename std::tuple_element<N, types>::type;
        };
    };

    struct signature : arg
    {
    };
};

/// Checks whether the ArgType is the base type of the argument of a function at a given position.
/// \tparam BaseType The base type to check
/// \tparam Function The function signature.
/// \tparam N The position of the argument within the function signature to check.
template <typename BaseType, typename Function, std::size_t N>
class is_base_arg_of
{
    static constexpr auto test()
    {
        if constexpr (function_traits<Function>::arg::arity > 0u)
        {
            return std::is_base_of_v<std::decay<BaseType>, std::decay<typename function_traits<Function>::arg::template get<N>::type>>;
        }
        else
        {
            return false;
        }
    }

public:
    static constexpr bool value = test();
};

} // namespace stew

#endif // STEW_FUNCTION_TRAITS_HPP
