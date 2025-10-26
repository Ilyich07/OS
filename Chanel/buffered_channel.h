#ifndef BUFFERED_CHANNEL_H_
#define BUFFERED_CHANNEL_H_

#include <mutex>
#include <queue>
#include <condition_variable>
#include <utility>
#include <stdexcept>

template<class T>
class BufferedChannel {
public:
    explicit BufferedChannel(int size) : buffer_size(size), isOpen(true) {}

    void Send(T value) {
        std::unique_lock<std::mutex> lk(mutex_);
        
        if (!isOpen) {
            throw std::runtime_error("Channel is closed!");
        }

        if (buffer_.size() >= buffer_size) {
            send_.wait(lk, [this]() {
                return buffer_.size() < buffer_size || !isOpen;
            });

            if (!isOpen) {
                throw std::runtime_error("Channel is closed!");
            }
        }
        
        buffer_.push(std::move(value));
        recv_.notify_one();
    }

    std::pair<T, bool> Recv() {
        std::unique_lock<std::mutex> lk(mutex_);
        
        if (!buffer_.empty()) {
            T value = std::move(buffer_.front());
            buffer_.pop();
            send_.notify_one();
            return std::make_pair(std::move(value), true);
        }
        
        if (!isOpen) {
            return std::make_pair(T(), false);
        }
        
        recv_.wait(lk, [this]() { 
            return !buffer_.empty() || !isOpen; 
        });
        
        if (!buffer_.empty()) {
            T value = std::move(buffer_.front());
            buffer_.pop();
            send_.notify_one();
            return std::make_pair(std::move(value), true);
        }
        return std::make_pair(T(), false);
    }

    void Close() {
        std::unique_lock<std::mutex> lk(mutex_);
        if (isOpen) {
            isOpen = false;
            lk.unlock();
            send_.notify_all();
            recv_.notify_all();
        }
    }

private:
    std::queue<T> buffer_;
    int buffer_size;
    std::mutex mutex_;
    std::condition_variable send_;
    std::condition_variable recv_;
    bool isOpen;
};

#endif // BUFFERED_CHANNEL_H_