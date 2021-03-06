#ifndef THREADQUEUE_H
#define THREADQUEUE_H

#include <queue>
#include <mutex>

namespace Threads {

template <typename T>
class ThreadQueue {
public:
    inline bool push(T const & value) {
        std::unique_lock<std::mutex> lock(this->mutex);
        this->q.push(value);
        return true;
    }
    // deletes the retrieved element, do not use for non integral types
    inline bool pop(T & v) {
        std::unique_lock<std::mutex> lock(this->mutex);
        if (this->q.empty())
            return false;
        v = this->q.front();
        this->q.pop();
        return true;
    }
    inline bool empty() {
        std::unique_lock<std::mutex> lock(this->mutex);
        return this->q.empty();
    }
    inline std::size_t size() {
        std::unique_lock<std::mutex> lock(this->mutex);
        return q.size();
    }
private:
    std::queue<T> q;
    std::mutex mutex;
};

} // namespace Threads

#endif // THREADQUEUE_H
