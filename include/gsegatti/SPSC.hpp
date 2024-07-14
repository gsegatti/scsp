// #ifndef SPSC_HPP
// #define SPSC_HPP

#include <type_traits>
#include <concepts>
#include <atomic>

template <size_t N>
concept PowerOfTwo = ((N & (N - 1)) == 0);

namespace gsegatti
{
  template <typename T, size_t queueSize, size_t cacheLineAlignment = 64>
    requires PowerOfTwo<queueSize> && PowerOfTwo<cacheLineAlignment> && std::is_nothrow_copy_assignable_v<T> // covers std::is_nothrow_assignable
  class SPSC
  {

  public:
    void push(const T &t) noexcept
    {
      size_t writeIdx = writeIdx_.load(std::memory_order_relaxed);
      size_t nextWriteIdx = (writeIdx + 1) % actualSize;
      while (nextWriteIdx == readIdxCached_)
      {
        readIdxCached_ = readIdx_.load(std::memory_order_relaxed);
      }
      block[writeIdx] = t;
      writeIdx_.store(nextWriteIdx, std::memory_order_release);
    }

    void push(const T &&t) noexcept
    {
      size_t writeIdx = writeIdx_.load(std::memory_order_relaxed);
      size_t nextWriteIdx = (writeIdx + 1) % actualSize;
      while (nextWriteIdx == readIdxCached_)
      {
        readIdxCached_ = readIdx_.load(std::memory_order_relaxed);
      }
      block[writeIdx] = t;
      writeIdx_.store(nextWriteIdx, std::memory_order_release);
    }

    [[nodiscard]] T *front() noexcept
    {
      size_t const currentReadIdx = readIdx_.load(std::memory_order_relaxed);
      if (currentReadIdx == writeIdxCached_)
      {
        writeIdxCached_ = writeIdx_.load(std::memory_order_relaxed);
        if (currentReadIdx == writeIdxCached_)
        {
          return nullptr;
        }
      }
      return &block[currentReadIdx];
    }

    void pop() noexcept
    {
      size_t const currentReadIdx = readIdx_.load(std::memory_order_relaxed);
      if (currentReadIdx == writeIdx_.load(std::memory_order_relaxed))
      {
        return;
      }
      size_t nextReadIdx = (currentReadIdx + 1) % actualSize;
      readIdx_.store(nextReadIdx, std::memory_order_release);
    }

    [[nodiscard]] bool empty() const noexcept
    {
      return writeIdx_.load(std::memory_order_acquire) ==
             readIdx_.load(std::memory_order_acquire);
    }

    [[nodiscard]] constexpr size_t capacity() const noexcept { return queueSize; }

  private:
    alignas(cacheLineAlignment) static constexpr size_t actualSize = queueSize + 1;
    alignas(cacheLineAlignment) T block[actualSize];
    // Reduce False Sharing.
    alignas(cacheLineAlignment) std::atomic<size_t> writeIdx_ = {0};
    alignas(cacheLineAlignment) std::atomic<size_t> readIdx_ = {0};
    alignas(cacheLineAlignment) size_t readIdxCached_ = 0;
    alignas(cacheLineAlignment) size_t writeIdxCached_ = 0;
  };
}