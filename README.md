# Single Consumer Single Producer Queue

Lock Free queue built on top of a contiguous Ring Buffer using C++ 20.

# Interface

This structure contains a very minimalistic interface:

### `SPSC<T, size_t queueSize, size_t cacheLineAlignment = 64>;`

Create an `SPSC` queue holding items of type `T` with a fixed `queueSize`. `queueSize` must be a power of two. `cacheLineAlignment` must also be a power of two and defaults to 64.

### `void push(const T &t) noexcept;`

Enqueue an item using copy assignment. This method blocks if queue is full. Ensure that the queue has space before calling this method.

### `template<typename TT> void push(TT &&t) noexcept;`

Enqueue an item using move assignment. This method also blocks if queue is full. Requires from that type of object is `std::is_nothrow_move_assignable_v<TT>`.

### `T *front() noexcept;`

Return a pointer to the front item of the queue if existing, otherwise, `nullptr`.

### `void pop() noexcept;`

Dequeue the first item of the queue. Previous item at `front()` can now be overwritten by `push()` calls.

### `bool empty() const noexcept;`

Return `true` if the queue is currently empty, false otherwise.

### `constexpr size_t capacity() const noexcept;`

Return the fixed capacity of the queue.

# Implementation

Based on Erik Rigtorp's strategy of avoiding false sharing, as well as diminishing cache coherency traffic by storing cached local copies of the write and read indexes for each writer-consumer threads.
See:
- https://rigtorp.se/ringbuffer/
- https://github.com/rigtorp/SPSCQueue?tab=readme-ov-file#implementation
- https://en.wikipedia.org/wiki/Circular_buffer
