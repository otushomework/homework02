#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

using IpAddress = std::vector<std::string>;
using Filter = std::map<int, std::string>;
using FilterRes = std::map<int, bool>;

// ("",  '.') -> [""]
// ("11", '.') -> ["11"]
// ("..", '.') -> ["", "", ""]
// ("11.", '.') -> ["11", ""]
// (".11", '.') -> ["", "11"]
// ("11.22", '.') -> ["11", "22"]
auto split(const std::string &str, char d)
{
    IpAddress r;

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

inline auto lex_sort( const IpAddress& rval, const IpAddress& lval )
{
    for (int i = 0; i < 3; ++i)
    {
        if ( std::stoi( rval.at(i) ) != std::stoi( lval.at(i) ) )
            return std::stoi( rval.at(i) ) > std::stoi( lval.at(i) );
    }
    return false;
}

//$ ip_filter < ip_filter.tsv
int main(int argc, char const *argv[])
{
    //unused warnings
    (void)argc;
    (void)argv;

    try
    {
        std::vector<IpAddress> ip_pool;

        for(std::string line; std::getline(std::cin, line);)
        {
            auto v = split(line, '\t');
            ip_pool.push_back(split(v.at(0), '.'));
        }

        // reverse lexicographical sort
        std::sort( ip_pool.begin(), ip_pool.end(), lex_sort );

        auto printer = [&](const Filter filter){
            for(std::vector<IpAddress>::const_iterator ip = ip_pool.cbegin(); ip != ip_pool.cend(); ++ip)
            {
                FilterRes filterRes;

                std::string buffer;
                for(IpAddress::const_iterator ip_part = ip->cbegin(); ip_part != ip->cend(); ++ip_part)
                {
                    int distance = filter.find( -1 ) != filter.end() ? -1 : std::distance(ip->cbegin(), ip_part);

                    if ( filter.find( distance ) != filter.end() )
                    {
                        filterRes.insert(std::pair<int, bool>(std::distance(ip->cbegin(), ip_part), *ip_part == filter.at(distance)));
                    }

                    if (ip_part != ip->cbegin())
                        buffer.append(".");

                    buffer.append(*ip_part);
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
        printer(Filter{{0, "1"}});
        printer(Filter{{0, "46"}, {1, "70"}});
        printer(Filter{{-1, "46"}});
    }
    catch(const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
