#pragma once

#include <chrono>
#include <map>
#include <string>

class timer
{
    std::chrono::steady_clock::time_point _t1;
    std::chrono::steady_clock::time_point _t2;

public:
    timer()
    {
        _t1 = std::chrono::steady_clock::now();
    }

    // Return elapsed time (s) since the last call to this function, or since the timer was created
    double tick()
    {
        _t2 = std::chrono::steady_clock::now();
        std::chrono::duration<double> time_span =
            std::chrono::duration_cast<std::chrono::duration<double>>(
                _t2 - _t1);
        _t1 = _t2;
        return time_span.count();
    }
};

class json_logger
{
    std::vector<std::pair<std::string, std::string>> _data;

public:
    void add(std::string key, std::string value)
    {
        _data.emplace_back(key, value);
    }
    template <typename T>
    void add(std::string key, T value)
    {
        _data.emplace_back(key, std::to_string(value));
    }
    void write(std::ostream& os = std::cout)
    {
        os << "{";
        for (auto it = _data.begin(); it != _data.end(); ++it)
        {
            os << "\"" << it->first << "\": \"" << it->second << "\"";
            if (std::next(it) != _data.end())
            {
                os << ", ";
            }
        }
        os << "}" << std::endl;
    }

    ~json_logger()
    {
        write();
    }
};

static json_logger global_json_logger;
