#pragma once
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <sstream>
#include <climits>

using namespace std;


template <typename T>
T format(string element)
{
    return (T)stod(element);
}

template <>
string format(string element)
{
    return element;
}

template <typename T, T... S, typename F>
constexpr void for_sequence(std::integer_sequence<T, S...>, F&& f)
{
    (void(f(std::integral_constant<T, S>{})), ...);
}

template <typename... Ts>
ostream& operator<<(ostream& os, tuple<Ts...> const& t)
{
    for_sequence(std::make_index_sequence<sizeof...(Ts)>{}, [&](auto i)
        {
            auto element = std::get<i>(t);
    os << element << ((i != sizeof... (Ts) - 1) ? "," : ""); });
    return os;
}

template <typename... Ts>
class CSVParser
{
public:
    CSVParser(ifstream& file, int skip_lines)
    {
        this->file = &file;
        this->skip_lines = skip_lines;
        file_validate(*(this->file));
        for (int i = 0; i < skip_lines; i++)
            (this->file)->ignore(LONG_MAX, '\n');
    }

    class access_iterator
    {
    public:
        tuple<Ts...> tuple_result;
        ifstream* ifs;
        string line;
        int cur_line;

        access_iterator() = default;
        ~access_iterator() = default;
        access_iterator(ifstream* ifs) : ifs(ifs), cur_line(0) {};

        bool operator!=(access_iterator& other)
        {
            return !(ifs->eof());
        }
        access_iterator& operator++()
        {
            return *this;
        }

        tuple<Ts...> operator*()
        {
            getline(*ifs, line);
            cur_line++;
            stringstream ss_line(line);
            string element;
            int cur_row = 0;
            for_sequence(std::make_index_sequence<sizeof...(Ts)>{}, [&](auto i)
                {
                    if (getline(ss_line, element, ',')) {
                        auto tuple_i = std::get<i>(tuple_result);
                        using subtype = decltype(tuple_i);
                        auto& tuple_i_ref = std::get<i>(tuple_result);
                        cur_row++;
                        try { tuple_i_ref = format<subtype>(element); }
                        catch (const std::invalid_argument& e) { throw std::invalid_argument("Bad type at line: " + to_string(cur_line) + " and row: " + to_string(cur_row)); } //exception throws based on new line iteration (not including skipped lines)
                    } });
            cur_row = 0;
            return tuple_result;
        }
    };

    access_iterator begin()
    {
        return access_iterator(file);
    }
    access_iterator end()
    {
        return access_iterator();
    }

private:
    ifstream* file;
    int skip_lines;

    bool is_empty(ifstream& file)
    {
        return file.peek() == ifstream::traits_type::eof();
    }

    void file_validate(ifstream& file)
    {
        if (!file.is_open() || is_empty(file))
        {
            cout << "Wrong File Path" << endl;
            exit(0);
        }
    }
};
