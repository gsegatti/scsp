#include <gsegatti/SPSC.hpp>
#include <iostream>
#include <thread>

int main()
{

  using namespace gsegatti;

  // Create a queue of size 1 and insert elements over and over again.
  SPSC<int, 1> q;
  size_t iterations = 1000000;

  auto t = std::thread([&]
                       {
      for(int i = 0; i < iterations; ++i){
        // Wait while write thread has yet to begin writing.
        while (!q.front())
          ;
        auto frontELement = q.front();
        // Ensure our structure is working.
        if(!frontELement || *frontELement != i){
          throw std::runtime_error("Wrong ordering of elements being popped!");
        }
        q.pop();
      } });

  for (size_t i = 0; i < iterations; ++i)
  {
    q.push(i);
  }

  t.join();
  return 0;
}