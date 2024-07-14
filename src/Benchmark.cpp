#include <gsegatti/SPSC.hpp>
#include <chrono>
#include <iostream>
#include <thread>

// #if __has_include(<boost/lockfree/spsc_queue.hpp> )
#include <boost/lockfree/spsc_queue.hpp>
// #endif

#if __has_include(<folly/ProducerConsumerQueue.h>)
#include <folly/ProducerConsumerQueue.h>
#endif

void pinThread(int cpu) {
  if (cpu < 0) {
    return;
  }
  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);
  CPU_SET(cpu, &cpuset);
  if (pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset) ==
      -1) {
    perror("pthread_setaffinity_no");
    exit(1);
  }
}

int main() {
//   (void)argc, (void)argv;

  using namespace gsegatti;

  int cpu1 = 1;
  int cpu2 = 0;

//   if (argc == 3) {
//     cpu1 = std::stoi(argv[1]);
//     cpu2 = std::stoi(argv[2]);
//   }

  const size_t queueSize = 1048576;
  const int64_t iters = 1048576*2;

  std::cout << "My SPSCQueue:" << std::endl;

  {
    SPSC<int, queueSize, 64> q;
    auto t = std::thread([&] {
      pinThread(cpu1);
      for (int i = 0; i < iters; ++i) {
        auto _ = q.front();
        q.pop();
      }
    });

    pinThread(cpu2);

    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < iters; ++i) {
      q.push(i);
    }
    t.join();
    auto stop = std::chrono::steady_clock::now();
    std::cout << iters * 1000000 /
                     std::chrono::duration_cast<std::chrono::nanoseconds>(stop -
                                                                          start)
                         .count()
              << " ops/ms" << std::endl;
  }

  std::cout << "boost::lockfree::spsc:" << std::endl;
  {
    boost::lockfree::spsc_queue<int> q(queueSize);
    auto t = std::thread([&] {
      pinThread(cpu1);
      for (int i = 0; i < iters; ++i) {
        int val;
        while (q.pop(&val, 1) != 1)
          ;
        if (val != i) {
          throw std::runtime_error("");
        }
      }
    });

    pinThread(cpu2);

    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < iters; ++i) {
      while (!q.push(i))
        ;
    }
    t.join();
    auto stop = std::chrono::steady_clock::now();
    std::cout << iters * 1000000 /
                     std::chrono::duration_cast<std::chrono::nanoseconds>(stop -
                                                                          start)
                         .count()
              << " ops/ms" << std::endl;
  }

// 

  return 0;
}