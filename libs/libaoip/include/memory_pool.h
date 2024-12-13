#pragma once
#include <memory>
#include <mutex>
#include <vector>

template <size_t BlockSize>
class MemoryPool {
   public:
    // 智能指针包装,自动归还内存
    class Buffer {
       public:
        Buffer(MemoryPool& pool) : pool_(pool), data_(static_cast<uint8_t*>(pool.allocate())) {}

        ~Buffer() {
            if (data_) {
                pool_.deallocate(data_);
            }
        }

        uint8_t* data() { return data_; }
        const uint8_t* data() const { return data_; }
        constexpr size_t size() const { return BlockSize; }

        // 禁止拷贝
        Buffer(const Buffer&) = delete;
        Buffer& operator=(const Buffer&) = delete;

        // 允许移动
        Buffer(Buffer&& other) noexcept : pool_(other.pool_), data_(other.data_) { other.data_ = nullptr; }

        Buffer& operator=(Buffer&& other) noexcept {
            if (this != &other) {
                if (data_) {
                    pool_.deallocate(data_);
                }
                pool_ = other.pool_;
                data_ = other.data_;
                other.data_ = nullptr;
            }
            return *this;
        }

       private:
        MemoryPool& pool_;
        uint8_t* data_;
    };

    static MemoryPool& instance() {
        static MemoryPool pool;
        return pool;
    }

    Buffer getBuffer() { return Buffer(*this); }

   private:
    friend class Buffer;

    MemoryPool() { expandPool(); }

    void* allocate() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (free_blocks_.empty()) {
            expandPool();
        }
        void* block = free_blocks_.back();
        free_blocks_.pop_back();
        return block;
    }

    void deallocate(void* ptr) {
        std::lock_guard<std::mutex> lock(mutex_);
        free_blocks_.push_back(ptr);
    }

    void expandPool() {
        constexpr size_t expand_size = 1024;
        auto new_block = std::make_unique<char[]>(BlockSize * expand_size);
        for (size_t i = 0; i < expand_size; i++) {
            free_blocks_.push_back(&new_block[i * BlockSize]);
        }
        blocks_.push_back(std::move(new_block));
    }

    std::mutex mutex_;
    std::vector<void*> free_blocks_;
    std::vector<std::unique_ptr<char[]>> blocks_;
};