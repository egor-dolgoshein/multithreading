//find_first_of, rotate, merge, reduce

#include <algorithm>
#include <numeric>
#include <execution>
#include <chrono>
#include <vector>
#include <tuple>
#include <iostream>
#include <random>

using namespace std;

double timing(auto && action)
{
  auto t0 = chrono::high_resolution_clock::now();
  action();
  auto t1 = chrono::high_resolution_clock::now();
  return (t1 - t0).count() * 1e-9;
}

namespace algorithms{
    struct Find_first_of{
        template<typename EP>
        void operator()(EP && ep, size_t n = 10'000'000) const
        {
            cout << "Find_first_of: n = " << n << ", ";
            vector<int> data(n);
            vector<int> subdata;
            iota(begin(data), end(data), 0);
            subdata.push_back(*(prev(end(data))));
            subdata.push_back(*(prev(end(data), 2)));
            cout << timing([ep, &data, &subdata]()
            {
                auto it = find_first_of(ep, begin(data), end(data), begin(subdata), end(subdata));

                if(it == end(data))
                    cout << "not found, ";
                else
                    cout << "found: " << *it << ", ";
            }) << "s\n";

        }
    };

    struct Rotate{
        template<typename EP>
        void operator()(EP && ep, size_t n = 10'000'000)const
        {
            cout << "Rotate: n = " << n << ", ";
            vector<int> data(n);
            iota(begin(data), end(data), 0);
            int rand;
            random_device rd;
			mt19937 gen(rd());
			uniform_int_distribution<> dist(0, n);
			rand = dist(gen);
            auto new_begin = begin(data);
            for(int i = 0; i < rand; ++i)
                new_begin = next(new_begin);
            cout << timing([ep, &data, new_begin](){
                auto it = rotate(ep, begin(data), new_begin, end(data));
            }) << "s\n";
        }
    };

    struct Merge{
        template<typename EP>
        void operator()(EP && ep, size_t n = 10'000'000) const
        {
            cout << "Merge: n = " << n << ", ";
            vector<int> data1(n), data2(n), out(2*n);
            iota(begin(data1), end(data1), 0);
            iota(begin(data2), end(data2), n / 2);
            cout << timing([ep, &data1, &data2, &out](){
                merge(ep, begin(data1), end(data1), begin(data2), end(data2), begin(out));
            }) << "s\n";
        }
    };

    struct Reduce{
        template<typename EP>
        void operator()(EP && ep, size_t n = 10'000'000) const
        {
            cout << "Reduce: n = " << n << ", ";
            vector<int> data(n);
            iota(begin(data), end(data), 0);
            cout << timing([ep, &data](){
                reduce(begin(data), end(data));
            }) << "s\n";
        }
    };
}


template <typename Algorithm>
void test_one_algorithm()
{
  cout << endl;
}

#define REPORT(pol) \
  void report(execution::pol) \
  { cout << #pol << endl; }

REPORT(sequenced_policy)
REPORT(parallel_policy)
REPORT(parallel_unsequenced_policy)
REPORT(unsequenced_policy)
#undef REPORT

template <typename Algorithm, typename Head, typename... Tail>
void test_one_algorithm()
{
  report(Head{});
  Algorithm{}(Head{});
  test_one_algorithm<Algorithm, Tail...>();
}

template <typename Algorithm, typename... Args>
void test_one_algorithm_trampoline(Algorithm, tuple<Args...>)
{
  test_one_algorithm<Algorithm, Args...>();
}

template <typename Policies>
void test_all_algorithms()
{
  cout << "\ndone" << endl;
}

template <typename Policies, typename Head, typename... Tail>
void test_all_algorithms()
{
  test_one_algorithm_trampoline(Head{}, Policies{});
  test_all_algorithms<Policies, Tail...>();
}

template <typename Policies, typename... Algorithms>
void test_all_algorithms_trampoline(Policies, tuple<Algorithms...>)
{
  test_all_algorithms<Policies, Algorithms...>();
}


int main()
{
  // Algorithms x Policies
  using namespace algorithms;
  tuple<Find_first_of, Rotate, Merge, Reduce> algorithms;

  using namespace execution;
  tuple policies{seq, unseq, par, par_unseq};

  test_all_algorithms_trampoline(policies, algorithms);
  return 0;
}