#ifndef TripleBuf_HPP
#define TripleBuf_HPP

#include <algorithm>
#include <atomic>
#include <functional>
#include <memory>
#include <span>
#include <cassert>

template < typename T >
class TripleBuf
{
public:
    TripleBuf() = default;

    template < typename FUN > void produce(const FUN& fun);
    template < typename FUN > void consume(const FUN& fun);

private:
    std::atomic< int > published_shared{-1};
    int published = 0, swap = 1, busy = 2;
    T obj[3];
};

template < typename T >
template < typename FUN >
void TripleBuf< T >::produce(const FUN& producer)
{
    producer(obj[swap]);
    const int published_prev = published_shared.exchange(swap, std::memory_order_release);
    if (published_prev == -1){
        int tmp = busy;
        busy = published;
        published = swap;
        swap = tmp;
    } else {
        assert(published_prev == published);
        int tmp = published;
        published = swap;
        swap = tmp;
    }
        
}

template < typename T >
template < typename FUN >
void TripleBuf< T >::consume(const FUN& consumer)
{
    const int published = published_shared.exchange(-1, std::memory_order_acquire);
    if (published != -1) consumer(obj[published]);
}

#endif // TripleBuf_HPP
