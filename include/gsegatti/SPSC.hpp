// #ifndef SPSC_HPP
// #define SPSC_HPP

#include <type_traits>
#include <concepts>
#include <new> // std::hardware_destructive_interference_size
#include <atomic>
#include <optional>

template <size_t N>
concept PowerOfTwo = (N > 1) &&
                     ((N & (N - 1)) == 0);

namespace gsegatti
{
  template <typename T, size_t queueSize, size_t cacheLineSize = 64>
    requires PowerOfTwo<queueSize> && PowerOfTwo<cacheLineSize> && std::is_nothrow_copy_assignable_v<T> // covers std::is_nothrow_assignable
  class SPSC
  {

  public:
    // SPSC(const SPSC &) = delete;
    // SPSC &operator=(const SPSC &) = delete;

    explicit SPSC() {}

    void push(const T &t) noexcept
    {
      size_t nextWriteIdx = (writeIdx_.load(std::memory_order_relaxed) + 1) % queueSize;
      // size_t currentReadIdx = readIdx_.load(std::memory_order_acquire);
      while (nextWriteIdx == readIdxCached)
      {
        readIdxCached = readIdx_.load(std::memory_order_relaxed);
      }
      block[nextWriteIdx] = t;
      writeIdx_.store(nextWriteIdx, std::memory_order_release);
    }

    void push(const T &&t) noexcept
    {
      size_t nextWriteIdx = (writeIdx_.load(std::memory_order_relaxed) + 1) % queueSize;
      size_t currentReadIdx = readIdx_.load(std::memory_order_acquire);
      while (nextWriteIdx == currentReadIdx)
      {
        currentReadIdx = readIdx_.load(std::memory_order_relaxed);
      }
      block[nextWriteIdx] = t;
      writeIdx_.store(nextWriteIdx, std::memory_order_release);
    }

    std::optional<T> pop() noexcept
    {
      size_t const currentReadIdx = readIdx_.load(std::memory_order_relaxed);
      if (currentReadIdx == writeIdx_.load(std::memory_order_relaxed))
      {
        return std::nullopt;
      }
      size_t nextReadIdx = (currentReadIdx + 1) % queueSize;
      readIdx_.store(nextReadIdx, std::memory_order_release);
      return std::optional<T>{std::in_place, block[currentReadIdx]};
    }

    [[nodiscard]] bool empty() const noexcept
    {
      return writeIdx_.load(std::memory_order_acquire) ==
             readIdx_.load(std::memory_order_acquire);
    }

    [[nodiscard]] constexpr size_t capacity() const noexcept { return queueSize; }

  private:
    T block[queueSize];
    // Reduce False Sharing.
    alignas(cacheLineSize) std::atomic<size_t> writeIdx_ = {0};
    alignas(cacheLineSize) std::atomic<size_t> readIdx_ = {0};
    alignas(cacheLineSize) size_t readIdxCached = 0;
  };

}

// #endif SPSC_HPP

// _ 1 -> 1 1 -> 1    *1->READ
//              WRITE