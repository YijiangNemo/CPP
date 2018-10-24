// 1) Make this multithreaded (hint: standard algorithms are allowed, and are a great idea to use in an individual thread)
// 2) make it require a specific type of iterator using type traits
// possible questions

#include <algorithm>
#include <iostream>
#include <limits>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>

template <typename It>
std::vector<std::pair<It, It>> divide_work(It begin, It end, unsigned n) {
  auto dis = std::distance(begin, end);
  auto npercore = (dis / n) + 1;
  auto extras = dis % n;
  std::vector<std::pair<It, It>> result;
  for (auto i = 0U; i < n; ++i) {
    if (i == extras)
      --npercore;
    result.emplace_back(begin, begin + npercore);
    begin += npercore;
  }
  return result;
}

template <typename It, typename T>
It find_parallel(It begin, It end, T value) {
  int cores = 4;
  auto work = divide_work(begin, end, cores);
  std::vector<std::thread> threads;

  std::mutex m;
  It found = end;
  for (const auto& part : work)
    threads.emplace_back([part, &value, &found, &m] () {
        auto result = std::find(part.first, part.second, value);
        if (result != part.second) {
          std::lock_guard<std::mutex> lg{m};
          found = std::min(found, result);
        }
    });
  for (auto& thread : threads)
    thread.join();
  return found;
}

int main() {

  std::vector<int> vec;
  for (auto i = 0; i < 100003; ++i)
    vec.push_back(i);
  std::cout << find_parallel(vec.begin(), vec.end(), 90000) - vec.begin() << '\n';
  std::cout << find_parallel(vec.begin(), vec.end(), 100000) - vec.begin() << '\n';
  std::cout << find_parallel(vec.begin(), vec.end(), 100001) - vec.begin() << '\n';
  std::cout << find_parallel(vec.begin(), vec.end(), 99999) - vec.begin() << '\n';
  vec[12499] = 99999;
  std::cout << find_parallel(vec.begin(), vec.end(), 99999) - vec.begin() << '\n';
  vec[12499] = 87500;
  std::cout << find_parallel(vec.begin(), vec.end(), 87500) - vec.begin() << '\n';
}