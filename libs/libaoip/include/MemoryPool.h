#pragma once
#include <memory>
#include <mutex>
#include <vector>

template <size_t blockSize>
class MemoryPool {
   public:
    class Buffer {
       public:
        Buffer(MemoryPool& pool) : pool_(pool), data_(static_cast<uint8_t*>(pool.Allocate())) {}

        ~Buffer() {
            if (data_) {
                pool_.Deallocate(data_);
            }
        }

        uint8_t* GetData() { return data_; }
        const uint8_t* GetData() const { return data_; }
        constexpr size_t GetSize() const { return blockSize; }

        Buffer(const Buffer&) = delete;
        Buffer& operator=(const Buffer&) = delete;

        Buffer(Buffer&& other) noexcept : pool_(other.pool_), data_(other.data_) { other.data_ = nullptr; }

        Buffer& operator=(Buffer&& other) noexcept {
            if (this != &other) {
                if (data_) {
                    pool_.Deallocate(data_);
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

    static MemoryPool& GetInstance() {
        static MemoryPool pool;
        return pool;
    }

    Buffer GetBuffer() { return Buffer(*this); }

   private:
    friend class Buffer;

    MemoryPool() { ExpandPool(); }

    void* Allocate() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (freeBlocks_.empty()) {
            ExpandPool();
        }
        void* block = freeBlocks_.back();
        freeBlocks_.pop_back();
        return block;
    }

    void Deallocate(void* ptr) {
        std::lock_guard<std::mutex> lock(mutex_);
        freeBlocks_.push_back(ptr);
    }

    void ExpandPool() {
        constexpr size_t expandSize = 1024;
        auto newBlock = std::make_unique<char[]>(blockSize * expandSize);
        for (size_t i = 0; i < expandSize; i++) {
            freeBlocks_.push_back(&newBlock[i * blockSize]);
        }
        blocks_.push_back(std::move(newBlock));
    }

    std::mutex mutex_;
    std::vector<void*> freeBlocks_;
    std::vector<std::unique_ptr<char[]>> blocks_;
};