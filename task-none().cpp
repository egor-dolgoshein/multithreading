#include<iostream>
#include<chrono>
#include<vector>
#include<thread>
#include<ranges>


// (par_)none(data, predicate (, threads)) 
//  -- проверяет, что predicate ложен для каждого элемента data


using namespace std;

template<ranges::random_access_range R, typename K, relation<ranges::range_value_t<R>, K> Pred>

bool parallel_none_of(R range, K& key, Pred&& eq = equal_to<>{}, int threads = 0)
{
	if (threads < 1)
		threads = thread::hardware_concurrency();
	
	vector<thread> threads_group(threads);
	vector<size_t> results(threads, -1);

	size_t const n = ranges::size(range);
	size_t const chunk = n / threads;

	for (int i = 0; i < threads; ++i)
		threads_group[i] = thread([&, chunk, i, threads, n]
			{
				size_t beg = i * chunk;
				size_t end = i == threads - 1 ? n : beg + chunk;

				for (size_t j = beg; beg < end; ++beg)
					if (eq(range[j], key))
					{
						results[i] = j;
						return ;
					}
			});

	for (auto& th : threads_group)
		th.join();

	for (size_t item : results)
		if (item != size_t(-1))
			return false;
	return true;

}



int main()
{
	vector<int> data(1'000'000, 1);

	int key = 0;

	auto t0 = chrono::high_resolution_clock::now();
	if (parallel_none_of(data, key, equal_to<>{}))
		cout << "Correct test\n";
	auto t1 = chrono::high_resolution_clock::now();

	chrono::duration<double> delta_par = t1 - t0;

	t0 = chrono::high_resolution_clock::now();

	none_of(data.begin(), data.end(), [key](int item) {return item == key; });

	t1 = chrono::high_resolution_clock::now();

	chrono::duration<double> delta_us = t1 - t0;

	auto profit = delta_par - delta_us;

	cout << "With " << profit.count() << " s faster than usual none_of" << endl;

}