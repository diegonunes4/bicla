#pragma once

#include <vector>
#include <sstream>
#include <iostream>
#include <optional>
#include <cassert>

//------------------------------------------------------------------------------

namespace bisect::bicla
{
    namespace detail
    {
        template<typename C, typename T>
        struct argument
        {
            using config_type = C;
            typedef T (C::*pmv);
            using value_type = T;

            const pmv p;
            const std::string short_description;
            const std::string long_description;
        };

        template<typename C, typename T>
        struct option
        {
            using config_type = C;
            typedef T (C::*pmv);
            using value_type = T;

            const pmv p;
            const std::string id;
            const std::string short_description;
            const std::string long_description;
        };

        using svector = std::vector<std::string>;

        //---------------------------------------------------------------------

        template<typename C, typename Option>
        struct is_option
        {
            using value_type = typename Option::value_type;
            static constexpr bool value = std::is_convertible_v<Option, detail::option<C, value_type>>;
        };

        template<typename C, typename Argument>
        struct is_argument
        {
            using value_type = typename Argument::value_type;
            static constexpr bool value = std::is_convertible_v<Argument, detail::argument<C, value_type>>;
        };

        template<typename T>
        constexpr bool is_optional(T)
        {
            return false;
        }

        template<typename T>
        constexpr bool is_optional(std::vector<T>)
        {
            return true;
        }

        template<typename T>
        constexpr bool is_optional(std::optional<T>)
        {
            return true;
        }

        template<typename T>
        constexpr bool is_boolean(T)
        {
            return false;
        }

        constexpr bool is_boolean(bool)
        {
            return true;
        }

        template<int N, typename... Ts> using nth_type_of =
            typename std::tuple_element<N, std::tuple<Ts...>>::type;

        template<typename... Ts>
        struct get_config_type
        {
            using first_t = nth_type_of<0, Ts...>;
            using type = typename first_t::config_type;
        };

        //---------------------------------------------------------------------

        template<typename T, typename U>
        void assign(const T& s, std::optional<U>& target)
        {
            if constexpr(std::is_assignable_v<std::optional<U>, T>)
            {
                target = s;
            }
            else
            {
                std::istringstream is(s);
                U n;
                is >> n;
                target = n;
            }
        }

        template<typename T, typename U>
        void assign(const T& s, std::vector<U>& target)
        {
            if constexpr(std::is_assignable_v<U, T>)
            {
                target.push_back(s);
            }
            else
            {
                std::istringstream is(s);
                U n;
                is >> n;
                target.push_back(n);
            }
        }

        template<typename T, typename U>
        void assign(const T& s, U& target)
        {
            if constexpr(std::is_assignable_v<U, T>)
            {
                target = s;
            }
            else
            {
                std::istringstream is(s);
                U n;
                is >> n;
                target = n;
            }
        }

        template<typename C, typename T>
        bool do_parse(svector& args, C& config,
            const detail::argument<C, T>& argument)
        {
            bool in_option = false;
            bool done = false;

            const auto save_args = args;
            args.clear();

            for (const auto& arg : save_args)
            {
                if (done)
                {
                    args.push_back(arg);
                    continue;
                }

                if (arg == "-")
                {
                    in_option = true;
                    args.push_back(arg);
                    continue;
                }

                if (in_option)
                {
                    in_option = false;
                    args.push_back(arg);
                }
                else
                {
                    done = true;
                    assign(arg, (config.*(argument.p)));
                }
            }

            if (in_option) return false;

            // TODO: this should be constexpr and not force an instantiation
            if (is_optional(T{}))
            {
                return true;
            }
            else
            {
                return done;
            }
        }

        // An option that has an argument of type T
        template<typename C, typename T>
        bool do_parse(svector& args, C& config,
            const detail::option<C, T>& option)
        {
            bool in_option = false;
            bool done = false;

            const auto option_marker = std::string("-") + option.id;
            const auto save_args = args;
            args.clear();

            for (const auto& arg : save_args)
            {
                if (arg == option_marker)
                {
                    in_option = true;
                    continue;
                }

                if (in_option)
                {
                    in_option = false;
                    done = true;
                    assign(arg, (config.*(option.p)));
                }
                else
                {
                    args.push_back(arg);
                }
            }

            if (in_option) return false;

            // TODO: this should be constexpr and not force an instantiation
            if (is_optional(T{}))
            {
                return true;
            }
            else
            {
                return done;
            }
        }

        // A boolean option, with no argument
        template<typename C>
        bool do_parse(svector& args, C& config,
            const detail::option<C, bool>& option)
        {
            bool done = false;

            const auto option_marker = std::string("-") + option.id;
            const auto save_args = args;
            args.clear();

            for (const auto& arg : save_args)
            {
                if (done)
                {
                    args.push_back(arg);
                    continue;
                }

                if (arg == option_marker)
                {
                    done = true;
                    config.*(option.p) = true;
                    continue;
                }

                args.push_back(arg);
            }

            return true;
        }

        template<typename C, typename T>
        bool do_parse(svector& args, C& /*config*/,
            const T& wanted)
        {
            if constexpr(is_argument<C, T>::value)
            {
                using argument_type = detail::argument<C, typename is_argument<C, T>::value_type>;
                const argument_type& argument = wanted;
                return do_parse(argument);
            }
            else if constexpr(is_option<C, T>::value)
            {
                using argument_type = detail::option<C, typename is_option<C, T>::value_type>;
                const argument_type& option = wanted;
                return do_parse(option);
            }
        }

        template<typename C, typename T, typename ...Ts>
        bool do_parse(svector& args, C& config,
            const T& option,
            Ts... options)
        {
            if (!do_parse(args, config, option))
            {
                return false;
            }

            return do_parse(args, config, options...);
        }

        //---------------------------------------------------------------------

        template<typename C, typename T>
        std::string get_full_short_description(const T& parameter)
        {
            if constexpr(is_argument<C, T>::value)
            {
                return "<" + parameter.short_description + ">";
            }
            else if constexpr(is_option<C, T>::value)
            {
                return "-" + parameter.id + " <" + parameter.short_description + ">";
            }
            else
            {
                assert(false);
                return "";
            }
            
        }

        template<typename C, typename T>
        std::string build_usage_message(const T& parameter)
        {
            const auto d = get_full_short_description<C>(parameter);

            if (is_optional(typename T::value_type{}) || is_boolean(typename T::value_type{}))
            {
                return "[" + d + "]";
            }
            else
            {
                return d;
            }
        }

        template<typename C, typename T, typename... Ts>
        std::string build_usage_message(const T& parameter, const Ts&... other)
        {
            // TODO: fold expressions would be nice, if VS supported them
            return build_usage_message<C>(parameter) + " " + build_usage_message<C>(other...);
        }

        //---------------------------------------------------------------------

        template<typename T>
        void do_build_parameters_description(svector& v, const T& parameter)
        {
            v.push_back(parameter.short_description + ": " + parameter.long_description);
        }

        template<typename T, typename... Ts>
        void do_build_parameters_description(svector& v, const T& parameter, const Ts&... other)
        {
            do_build_parameters_description(v, parameter);
            do_build_parameters_description(v, other...);
        }

        template<typename... Ts>
        svector build_parameters_description(const Ts&... parameters)
        {
            svector v;
            do_build_parameters_description(v, parameters...);
            return v;
        }
    }

    struct parse_result
    {
        const bool success;
        const std::string usage_message;
        const detail::svector parameters_description;

        operator bool() const noexcept
        {
            return success;
        }
    };

    // returns:
    //  {
    //      parse_result: 
    //          converts to true if successful
    //          usage message: only valid if parsing did not succeed
    //
    //      configuration: only valid if parsing succeeded
    //  }
    template<typename ...Ts>
    auto parse(int argc, const char* const argv[], Ts... options)
        -> std::tuple<parse_result, typename detail::get_config_type<Ts...>::type>
    {
        assert(argc > 0);
        // Skip program name
        std::vector<std::string> arguments(argv + 1, argv + argc);

        using ConfigType = typename detail::get_config_type<Ts...>::type;
        auto config = ConfigType{};
        auto parse_ok = detail::do_parse(arguments, config, options...);

        if (!arguments.empty())
        {
            parse_ok = false;
        }

        const auto usage_message = detail::build_usage_message<ConfigType>(options...);
        const auto parameters_description = detail::build_parameters_description(options...);
        return 
        { 
            parse_result{ parse_ok, usage_message, parameters_description },
            config 
        };
    }

    template<typename C, typename T>
    detail::argument<C, T> argument(T C::* _p, std::string _short_description, std::string _long_description = "")
    {
        return detail::argument<C, T> { _p, _short_description, _long_description == "" ? _short_description : _long_description };
    }

    template<typename C, typename T>
    detail::option<C, T> option(T C::* _p, std::string _id, std::string _short_description, std::string _long_description = "")
    {
        return detail::option<C, T> { _p, _id, _short_description, _long_description == "" ? _short_description : _long_description };
    }

    inline std::string to_string(const parse_result& r)
    {
        std::string out = r.usage_message + '\n';
        for (const auto& s : r.parameters_description)
        {
            out += s + '\n';
        }

        return out;
    }

    inline std::ostream& operator<<(std::ostream& os, const parse_result& r)
    {
        os << to_string(r);
        return os;
    }
}
