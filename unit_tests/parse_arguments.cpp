#include "bisect/bicla.h"

#include <array>
#pragma warning(push)
#pragma warning(disable : 4996)
#include "catch2/catch.hpp"
#pragma warning(pop)
using namespace bisect::bicla;

//------------------------------------------------------------------------------

SCENARIO("argument parsing")
{
    GIVEN("a config with 3 string arguments")
    {
        struct config
        {
            std::string s1;
            std::string s2;
            std::string s3;
        };

        WHEN("we provide 3 arguments")
        {
            const std::array<const char*, 4> argv = {"program name", "string 1", "string 2", "string 3"};

            THEN("they are correctly parsed")
            {
                const auto [parse_result, config] =
                    parse(static_cast<int>(argv.size()), argv.data(), argument(&config::s1, "s 1"),
                          argument(&config::s2, "s 2"), argument(&config::s3, "s 3"));

                REQUIRE(parse_result == true);
                REQUIRE(config.s1 == "string 1");
                REQUIRE(config.s2 == "string 2");
                REQUIRE(config.s3 == "string 3");
            }
        }

        WHEN("we only provide 2 arguments")
        {
            const std::array<const char*, 3> argv = {"program name", "string 1", "string 2"};

            THEN("parsing fails")
            {
                const auto [parse_result, _] =
                    parse(int(static_cast<int>(argv.size())), argv.data(), argument(&config::s1, "s 1"),
                          argument(&config::s2, "s 2"), argument(&config::s3, "s 3"));
                static_cast<void>(_);

                REQUIRE(parse_result == false);
            }
        }

        WHEN("we provide four arguments")
        {
            const std::array<const char*, 5> argv = {"program name", "string 1", "string 2", "string 3", "extra"};

            THEN("parsing fails")
            {
                const auto [parse_result, _] =
                    parse(int(static_cast<int>(argv.size())), argv.data(), argument(&config::s1, "s 1"),
                          argument(&config::s2, "s 2"), argument(&config::s3, "s 3"));
                static_cast<void>(_);

                REQUIRE(parse_result == false);
            }
        }
    }

    GIVEN("a config with 3 string arguments, 1 of which is optional")
    {
        struct config
        {
            std::string s1;
            std::string s2;
            std::optional<std::string> s3;
        };

        WHEN("we provide 3 arguments")
        {
            const std::array<const char*, 4> argv = {"program name", "string 1", "string 2", "string 3"};

            THEN("they are correctly parsed")
            {
                const auto [parse_result, config] =
                    parse(int(static_cast<int>(argv.size())), argv.data(), argument(&config::s1, "s 1"),
                          argument(&config::s2, "s 2"), argument(&config::s3, "s 3"));

                REQUIRE(parse_result == true);
                REQUIRE(config.s1 == "string 1");
                REQUIRE(config.s2 == "string 2");
                REQUIRE(config.s3 == "string 3");
            }
        }

        WHEN("we provide only 2 arguments")
        {
            const std::array<const char*, 3> argv = {"program name", "string 1", "string 2"};

            THEN("the first 2 are correctly parsed and the last one is not set")
            {
                const auto [parse_result, config] =
                    parse(int(static_cast<int>(argv.size())), argv.data(), argument(&config::s1, "s 1"),
                          argument(&config::s2, "s 2"), argument(&config::s3, "s 3"));

                REQUIRE(parse_result == true);
                REQUIRE(config.s1 == "string 1");
                REQUIRE(config.s2 == "string 2");
                REQUIRE(!config.s3);
            }
        }
    }
}

SCENARIO("heterogenous argument parsing 2")
{
    GIVEN("a config with four arguments, 2 of which are optional")
    {
        struct config
        {
            std::string s1;
            int i1;
            std::optional<std::string> s3;
            std::optional<int> i2;
        };

        WHEN("we provide 3 arguments")
        {
            const std::array<const char*, 4> argv = {"program name", "string 1", "2", "string 3"};

            THEN("the first 3 are correctly parsed and the last 1 is not set")
            {
                const auto [parse_result, config] =
                    parse(int(static_cast<int>(argv.size())), argv.data(), argument(&config::s1, "s 1"),
                          argument(&config::i1, "i 1"), argument(&config::s3, "s 3"));

                REQUIRE(parse_result == true);
                REQUIRE(config.s1 == "string 1");
                REQUIRE(config.i1 == 2);
                REQUIRE(config.s3 == "string 3");
                REQUIRE(!config.i2);
            }
        }

        WHEN("we provide four arguments")
        {
            const std::array<const char*, 5> argv = {"program name", "string 1", "2", "string 3", "42"};

            THEN("all of them are correctly parsed")
            {
                const auto [parse_result, config] =
                    parse(int(static_cast<int>(argv.size())), argv.data(), argument(&config::s1, "s 1"),
                          argument(&config::i1, "i 1"), argument(&config::s3, "s 3"), argument(&config::i2, "i 2"));

                REQUIRE(parse_result == true);
                REQUIRE(config.s1 == "string 1");
                REQUIRE(config.i1 == 2);
                REQUIRE(config.s3 == "string 3");
                REQUIRE(config.i2 == 42);
            }
        }
    }
}

SCENARIO("messages")
{
    GIVEN("a config with 3 string arguments")
    {
        struct config
        {
            std::string s1;
            std::string s2;
            std::string s3;
        };

        WHEN("we parse")
        {
            const std::array<const char*, 3> argv = {"program name", "string 1", "string 2"};

            const auto [parse_result, _] = parse(
                int(static_cast<int>(argv.size())), argv.data(), argument(&config::s1, "s 1", "a string named 1"),
                argument(&config::s2, "s 2", "a string named 2"), argument(&config::s3, "s 3", "a string named 3"));

            static_cast<void>(_);

            THEN("the usage message is correct")
            {
                const auto expected_usage_message = "<s 1> <s 2> <s 3>";
                REQUIRE(parse_result.usage_message == expected_usage_message);
            }

            THEN("the parameters description is correct")
            {
                const auto expected_parameters_description = detail::svector{
                    "s 1: a string named 1",
                    "s 2: a string named 2",
                    "s 3: a string named 3",
                };
                REQUIRE(parse_result.parameters_description == expected_parameters_description);
            }
        }
    }

    GIVEN("a config with 3 arguments, 2 of which are optional")
    {
        struct config
        {
            std::string s1;
            std::optional<std::string> s2;
            std::optional<std::string> s3;
        };

        WHEN("we parse")
        {
            const std::array<const char*, 1> argv = {"program name"};

            const auto [parse_result, _] = parse(
                int(static_cast<int>(argv.size())), argv.data(), argument(&config::s1, "s 1", "a string named 1"),
                argument(&config::s2, "s 2", "a string named 2"), argument(&config::s3, "s 3", "a string named 3"));
            static_cast<void>(_);

            THEN("the usage message is correct")
            {
                const auto expected_usage_message = "<s 1> [<s 2>] [<s 3>]";
                REQUIRE(parse_result.usage_message == expected_usage_message);
            }

            THEN("the parameters description is correct")
            {
                const auto expected_parameters_description =
                    detail::svector{"s 1: a string named 1", "s 2: a string named 2", "s 3: a string named 3"};
                REQUIRE(parse_result.parameters_description == expected_parameters_description);
            }
        }
    }
}

SCENARIO("option parsing")
{
    GIVEN("a config with 1 string option")
    {
        struct config
        {
            std::string s1;
        };

        WHEN("we provide suitable arguments")
        {
            const std::array<const char*, 3> argv = {"program name", "-s", "string 1"};

            THEN("they are correctly parsed")
            {
                const auto [parse_result, config] =
                    parse(int(static_cast<int>(argv.size())), argv.data(), option(&config::s1, "s", "s 1"));

                REQUIRE(parse_result == true);
                REQUIRE(config.s1 == "string 1");
            }
        }

        WHEN("we provide extra arguments")
        {
            const std::array<const char*, 4> argv = {"program name", "-s", "string 1", "extra"};

            THEN("parsing fails")
            {
                const auto [parse_result, _] =
                    parse(int(static_cast<int>(argv.size())), argv.data(), option(&config::s1, "s", "s 1"));
                static_cast<void>(_);

                REQUIRE(parse_result == false);
            }
        }
    }

    GIVEN("a config with 3 options")
    {
        struct config
        {
            std::string s1;
            int i2;
            std::optional<float> f3;
        };

        WHEN("we provide suitable arguments")
        {
            const std::array<const char*, 7> argv = {"program name", "-s", "string 1", "-i", "123", "-f", "2.2"};

            THEN("they are correctly parsed")
            {
                const auto [parse_result, config] =
                    parse(int(static_cast<int>(argv.size())), argv.data(), option(&config::s1, "s", "a string"),
                          option(&config::i2, "i", "an int"), option(&config::f3, "f", "a float"));

                REQUIRE(parse_result == true);
                REQUIRE(config.s1 == "string 1");
                REQUIRE(config.i2 == 123);
                REQUIRE(config.f3 == Approx(2.2));
            }
        }
    }

    GIVEN("a config with 1 boolean option")
    {
        struct config
        {
            bool b1 = false;
        };

        WHEN("we provide the flag")
        {
            const std::array<const char*, 2> argv = {"program name", "-b"};

            THEN("it is set")
            {
                const auto [parse_result, config] =
                    parse(int(static_cast<int>(argv.size())), argv.data(), option(&config::b1, "b", "a bool option"));

                REQUIRE(parse_result == true);
                REQUIRE(config.b1);
            }
        }

        WHEN("we do not provide the flag")
        {
            const std::array<const char*, 1> argv = {"program name"};

            THEN("it is not set")
            {
                const auto [parse_result, config] =
                    parse(int(static_cast<int>(argv.size())), argv.data(), option(&config::b1, "b", "a bool option"));

                REQUIRE(parse_result == true);
                REQUIRE(config.b1 == false);
            }
        }
    }

    GIVEN("a config with 2 boolean options")
    {
        struct config
        {
            bool b = false;
            bool c = false;
        };

        WHEN("we provide the flags in one order")
        {
            const std::array<const char*, 3> argv = {"program name", "-b", "-c"};

            THEN("both are set")
            {
                const auto [parse_result, config] =
                    parse(int(static_cast<int>(argv.size())), argv.data(), option(&config::b, "b", "a bool option"),
                          option(&config::c, "c", "another bool option"));

                REQUIRE(parse_result == true);
                REQUIRE(config.b);
                REQUIRE(config.b);
            }
        }

        WHEN("we provide the flags in the reverse order")
        {
            const std::array<const char*, 3> argv = {"program name", "-c", "-b"};

            THEN("both are set")
            {
                const auto [parse_result, config] =
                    parse(int(static_cast<int>(argv.size())), argv.data(), option(&config::b, "b", "a bool option"),
                          option(&config::c, "c", "another bool option"));

                REQUIRE(parse_result == true);
                REQUIRE(config.b);
                REQUIRE(config.b);
            }
        }
    }
}

SCENARIO("option and argument parsing")
{
    GIVEN("a config with 1 option and 2 arguments")
    {
        struct config
        {
            std::string s1;
            int i2;
            std::optional<float> f3;
        };

        WHEN("we provide the arguments in the natural order")
        {
            const std::array<const char*, 5> argv = {"program name", "-s", "string 1", "123", "2.2"};

            THEN("they are correctly parsed")
            {
                const auto [parse_result, config] =
                    parse(int(static_cast<int>(argv.size())), argv.data(), option(&config::s1, "s", "a string"),
                          argument(&config::i2, "an int"), argument(&config::f3, "a float"));

                REQUIRE(parse_result == true);
                REQUIRE(config.s1 == "string 1");
                REQUIRE(config.i2 == 123);
                REQUIRE(config.f3 == Approx(2.2));
            }
        }

        WHEN("we change the order")
        {
            const std::array<const char*, 5> argv = {"program name", "123", "-s", "string 1", "2.2"};

            THEN("they are correctly parsed")
            {
                const auto [parse_result, config] =
                    parse(int(static_cast<int>(argv.size())), argv.data(), option(&config::s1, "s", "a string"),
                          argument(&config::i2, "an int"), argument(&config::f3, "a float"));

                REQUIRE(parse_result == true);
                REQUIRE(config.s1 == "string 1");
                REQUIRE(config.i2 == 123);
                REQUIRE(config.f3 == Approx(2.2));
            }
        }
    }

    GIVEN("a config with multiple options, including optionals and a vector")
    {
        struct config
        {
            bool b                = false;
            std::optional<bool> v = false;
            std::optional<std::string> f;
            std::optional<int> n;
            std::vector<std::string> e;
        };

        const auto do_parse = [](auto argv) {
            return parse(int(static_cast<int>(argv.size())), argv.data(), option(&config::b, "b", "b", "b"),
                         option(&config::f, "f", "f", "f"), option(&config::n, "n", "n", "n"),
                         option(&config::v, "v", "v", "v"), option(&config::e, "e", "e", "e"));
        };

        WHEN("we provide no arguments")
        {
            const std::array<const char*, 1> argv = {
                "program name",
            };

            THEN("parsing is correct")
            {
                const auto [parse_result, config] = do_parse(argv);

                REQUIRE(parse_result == true);
                REQUIRE(config.b == false);
                REQUIRE(!config.f.has_value());
                REQUIRE(!config.n.has_value());
                REQUIRE(config.v.has_value());
                REQUIRE(config.v.value() == false);
                REQUIRE(config.e.empty());
            }
        }

        WHEN("we provide the bool option")
        {
            const std::array<const char*, 2> argv = {"program name", "-b"};

            THEN("parsing is correct")
            {
                const auto [parse_result, config] = do_parse(argv);

                REQUIRE(parse_result == true);
                REQUIRE(config.b == true);

                REQUIRE(parse_result == true);
                REQUIRE(!config.f.has_value());
                REQUIRE(!config.n.has_value());
                REQUIRE(config.v.has_value());
                REQUIRE(config.v.value() == false);
                REQUIRE(config.e.empty());
            }
        }

        // WHEN("we provide the argument to the bool option")
        //{
        //    const std::array<const char*, 3> argv = { "program name",
        //        "-b", "1"
        //    };

        //    THEN("parsing is correct")
        //    {
        //        const auto[parse_result, config] = do_parse(argv);

        //        REQUIRE(parse_result == true);
        //        REQUIRE(config.b == true);

        //        REQUIRE(parse_result == true);
        //        REQUIRE(!config.f.has_value());
        //        REQUIRE(!config.n.has_value());
        //        REQUIRE(config.v.has_value());
        //        REQUIRE(config.v.value() == false);
        //        REQUIRE(config.e.empty());
        //    }
        //}

        WHEN("we provide the integer option")
        {
            const std::array<const char*, 3> argv = {"program name", "-n", "10"};

            THEN("parsing is correct")
            {
                const auto [parse_result, config] = do_parse(argv);

                REQUIRE(parse_result == true);
                REQUIRE(config.n.has_value());
                REQUIRE(config.n.value() == 10);

                REQUIRE(parse_result == true);
                REQUIRE(config.b == false);
                REQUIRE(!config.f.has_value());
                REQUIRE(config.v.has_value());
                REQUIRE(config.v.value() == false);
                REQUIRE(config.e.empty());
            }
        }

        WHEN("we provide the integer and string options")
        {
            const std::array<const char*, 5> argv = {"program name", "-f", "abcd", "-n", "10"};

            THEN("parsing is correct")
            {
                const auto [parse_result, config] = do_parse(argv);

                REQUIRE(parse_result == true);
                REQUIRE(config.f.has_value());
                REQUIRE(config.f.value() == "abcd");
                REQUIRE(config.n.has_value());
                REQUIRE(config.n.value() == 10);

                REQUIRE(parse_result == true);
                REQUIRE(config.b == false);
                REQUIRE(config.v.has_value());
                REQUIRE(config.v.value() == false);
                REQUIRE(config.e.empty());
            }
        }
    }
}