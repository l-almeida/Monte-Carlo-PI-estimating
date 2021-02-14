#include <cstdio>
#include <cstdint>
#include <random>
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

auto get_number_of_points() -> uint64_t
{
    uint64_t points = 0;
    bool success = false;
    while (!success)
    {
        double exp = 0;
        puts("Number of points to be generated is defined in a power of base 10 (10^x).");
        puts("x needs to be in range 1 <= x <= 19");
        printf("Define the value of x: ");
        scanf("%lf", &exp);
        if (exp < 1.0 || exp > 19.0)
        {
            puts("Error: x needs to greater than 0 and less than 20.");
            continue;
        }
        points = static_cast<uint64_t>(floorl(powl(10, exp)));
        printf("%llu points will be generated.\n", points);
        success = true;
    }
    return points;
}

auto get_number_of_threads() -> int
{
    int number_of_threads = 2;
    bool success = false;
    while (!success)
    {
        printf("Define the number of threads: ");
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

auto estimate_pi_monte_carlo_method(const uint64_t points) -> double
{
    Random rand;
    uint64_t num_point_circle = 0;
    uint64_t num_point_total = 0;
    for (auto i = points; i > 0; --i)
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

auto start_calculations(const uint64_t points, const int number_of_threads) -> double
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

auto main() -> int
{
    const uint64_t points = get_number_of_points();
    const int number_of_threads = get_number_of_threads();

    const auto start = std::chrono::steady_clock::now();
    const auto pi = start_calculations(points, number_of_threads);
    const auto end = std::chrono::steady_clock::now();
    const std::chrono::duration<double> elapsed_seconds = end - start;

    printf("\nElapsed time: %fs\n", elapsed_seconds.count());
    printf("%.8f estimated number of PI\n", pi);
    printf("%.8f real number of PI\n", 3.14159265);
}
