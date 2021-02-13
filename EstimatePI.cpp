#include <cstdio>
#include <cstdint>
#include <random>
#include <cstdint>
#include <thread>
#include <future>
#include <chrono>
#include <cmath>


class Random
{
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<> dis;

public:
    Random(double a = 0.0, double b = 1.0) : dis(a, b)
    {
        gen = std::mt19937(rd());
    }

    double get_uniform()
    {
        return dis(gen);
    }
};

uint64_t get_number_of_points()
{
    uint64_t points = 0llu;
    bool success = false;
    while (!success)
    {
        int exp = 0;
        puts("Number of points to be generated is defined in a power of base 10 (10^x).\n");
        puts("x needs to be in range 0 < x < 20\n");
        puts("Define the value of x: ");
        scanf("%d", &exp);
        if (exp <= 0 || exp >= 20)
        {
            puts("Error: x needs to greater than 0 and less than 20.");
            continue;
        }
        points = static_cast<uint64_t>(pow(10, exp));
        printf("%llu points will be generated.\n", points);
        success = true;
    }
    return points;
}

int get_number_of_threads()
{
    int number_of_threads = 2;
    bool success = false;
    while (!success)
    {
        puts("Define the number of threads: ");
        scanf("%d", &number_of_threads);
        if (number_of_threads < 0)
        {
            puts("Error: at least 1 thread is necessary.");
            continue;
        }
        success = true;
    }
    return number_of_threads;
}

double estimate_pi_monte_carlo_method(const int points)
{
    Random rand;
    int num_point_circle = 0;
    int num_point_total = 0;
    for (int i = 0; i < points; ++i)
    {
        auto x = rand.get_uniform();
        auto y = rand.get_uniform();
        auto distance = x * x + y * y;
        if (distance <= 1.0)
            num_point_circle += 1;
        num_point_total += 1;
    }
    const auto points_circle = static_cast<double>(num_point_circle);
    const auto points_total = static_cast<double>(num_point_total);
    return (points_circle / points_total);
}

double start_calculations(const uint64_t points, const int number_of_threads)
{
    const uint64_t points_per_thread = (points / number_of_threads);
    std::vector<std::future<double>> future_pi;
    for (int i = number_of_threads; i > 0; --i)
    {
        future_pi.push_back(std::async(&estimate_pi_monte_carlo_method, points_per_thread));
    }
    double points_factor = 0.0;
    for (auto& future : future_pi)
    {
        points_factor += future.get();
    }
    points_factor /= number_of_threads;

    auto pi = 4.0 * points_factor;
    return pi;
}

int main()
{
    uint64_t points = get_number_of_points();
    int number_of_threads = get_number_of_threads();

    auto start = std::chrono::steady_clock::now();
    auto pi = start_calculations(points, number_of_threads);
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;

    printf("Elapsed time: %fs\n", elapsed_seconds);
    printf("%.8f estimated number of PI\n", pi);
    printf("%.8f real number of PI\n", 3.14159265);
}