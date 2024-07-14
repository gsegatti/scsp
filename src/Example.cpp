#include <gsegatti/SPSC.hpp>
#include <iostream>
#include <thread>

int main(int argc, char *argv[])
{
  (void)argc, (void)argv;

  using namespace gsegatti;

  SPSC<int, 2> q;
  q.push(1);
  q.push(1);
  auto t = std::thread([&]
    {
      while (!q.empty())
      {
        auto item = q.front();
        q.pop();
        std::cout << item.value() << std::endl;
      }
    });

  t.join();

  return 0;
}