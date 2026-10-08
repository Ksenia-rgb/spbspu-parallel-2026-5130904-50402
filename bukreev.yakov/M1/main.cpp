#include <iostream>
#include <vector>
#include "montecarlo.hpp"

namespace bukreev
{
    void parseArgs(int argc, char* argv[], size_t& threads, size_t& tries, size_t& seed);
    std::istream& getFigures(std::istream& in, std::vector< Figure >& figures);
}

int main(int argc, char* argv[])
{
    size_t threads, tries, seed;
    try
    {
        bukreev::parseArgs(argc, argv, threads, tries, seed);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }

    std::vector< bukreev::Figure > figures;
    try
    {
        bukreev::getFigures(std::cin, figures);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }

    bukreev::AreaResult res = bukreev::monteCarlo(figures, threads, tries, seed);
    std::cout << res.total << ' ' << res.intersection << '\n';
}

void bukreev::parseArgs(int argc, char* argv[], size_t& threads, size_t& tries, size_t& seed)
{
    if (argc != 3 && argc != 4)
    {
        throw std::logic_error("Invalid number of arguments");
    }

    int ithreads = std::atoi(argv[1]);
    if (ithreads < 0)
    {
        throw std::logic_error("Negative threads number");
    }

    int itries = std::atoi(argv[2]);
    if (itries <= 0)
    {
        throw std::logic_error("Non-positive tries number");
    }

    int iseed = 0;
    if (argc == 4)
    {
        iseed = std::atoi(argv[3]);
        if (iseed < 0)
        {
            throw std::logic_error("Negative generator seed");
        }
    }

    threads = ithreads;
    tries = itries;
    seed = iseed;
}

std::istream& bukreev::getFigures(std::istream& in, std::vector< Figure >& figures)
{
    int n;
    size_t i = 0;
    Figure f;
    while (in >> n)
    {
        switch (++i)
        {
        case 1:
            f.r = n;
            break;

        case 3:
            f.cx = n;
            break;

        case 4:
            f.cy = n;
            try
            {
                figures.push_back(f);
            }
            catch (...)
            {
                figures.clear();
                throw;
            }
            i = 0;
            break;

        default:
            break;
        }
    }

    if (i != 0)
    {
        figures.clear();
        throw std::logic_error("Cannot parse figure parameters");
    }
    return in;
}
