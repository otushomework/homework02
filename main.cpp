#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>


using IpAddressStr = std::vector<std::string>;
using IpAddressInt = unsigned int;
using Filter = std::map<int, int>;
using FilterRes = std::map<int, bool>;

auto split(const std::string &str, char d)
{
    IpAddressStr r;

    std::string::size_type start = 0;
    std::string::size_type stop = str.find_first_of(d);
    while(stop != std::string::npos)
    {
        r.push_back(str.substr(start, stop - start));

        start = stop + 1;
        stop = str.find_first_of(d, start);
    }

    r.push_back(str.substr(start));

    return r;
}

//$ ip_filter < ip_filter.tsv
int main(int, char const *[])
{
    try
    {
        std::vector<IpAddressInt> ip_pool;

        for(std::string line; std::getline(std::cin, line);)
        {
            auto v = split(line, '\t');
            auto ip_val = split(v.at(0), '.');
            IpAddressInt converted = (((std::stoi(ip_val.at(0))) * 256 + std::stoi(ip_val.at(1))) * 256 + std::stoi(ip_val.at(2))) * 256 + std::stoi(ip_val.at(3));

            ip_pool.push_back(converted);
        }

        // reverse lexicographical sort
        std::sort( ip_pool.begin(), ip_pool.end(), std::greater<IpAddressInt>() );

        auto printer = [&](const Filter filter){
            for(std::vector<IpAddressInt>::const_iterator ip = ip_pool.cbegin(); ip != ip_pool.cend(); ++ip)
            {
                FilterRes filterRes;

                std::string buffer;
                for (int i = 3; i >= 0; --i)
                {
                    int distance = filter.find( -1 ) != filter.end() ? -1 : 3-i;

                    unsigned char octet = (*ip >> (8*i)) & 0xff;

                    if ( filter.find( distance ) != filter.end() )
                    {
                        filterRes.insert(std::pair<int, bool>(3-i, octet == filter.at(distance)));
                    }

                    if (i != 3)
                        buffer.append(".");

                    buffer.append(std::to_string(octet));
                }

                //print if we have equals
                bool match = true;
                for (FilterRes::const_iterator fi = filterRes.cbegin(); fi != filterRes.cend(); ++fi)
                {
                    match = fi->second;

                    if ( (filter.find( -1 ) == filter.end() && !match) || (filter.find( -1 ) != filter.end() && match) )
                        break;
                }

                if (match)
                    std::cout << buffer << std::endl;
            }
        };

        // simple filter
        printer(Filter{});
        printer(Filter{{0, 1}});
        printer(Filter{{0, 46}, {1, 70}});
        printer(Filter{{-1, 46}});
    }
    catch(const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
