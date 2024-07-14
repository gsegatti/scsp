#include <gsegatti/SPSC.hpp>
#include <iostream>
#include <thread>

int main(int argc, char *argv[])
{
  (void)argc, (void)argv;

  using namespace gsegatti;

  SPSC<int, 1> q;
  q.push(1);
  auto t = std::thread([&]
    {
      while (!q.front())
        ;
      std::cout << *q.front() << std::endl;
      q.pop();
    });

  t.join();

  return 0;
}