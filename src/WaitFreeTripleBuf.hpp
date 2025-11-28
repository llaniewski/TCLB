// Wait-free triple buffer: https://github.com/kubagalecki/wf3b
// Copyright (c) 2025 Jakub Gałecki

#ifndef WAITFREETRIPLEBUF_HPP
#define WAITFREETRIPLEBUF_HPP

#include <algorithm>
#include <atomic>
#include <concepts>
#include <functional>
#include <memory>
#include <span>

namespace wf3b
{
template < typename T >
class WaitFreeTripleBuf
{
public:
    //WaitFreeTripleBuf() = default;
    WaitFreeTripleBuf(size_t size = 1) : size_{size}, alloc_{std::make_unique_for_overwrite< T[] >(3 * size)} {}

    template < std::invocable< const std::span< T > > Producer >
    void produce(Producer&& producer);
    auto consume() -> std::span< T >;

private:
    static constexpr size_t        mask     = 0b11;
    static constexpr size_t        shift    = 2;
    static constexpr unsigned char consumed = mask;
    static constexpr size_t        getSize(size_t state) { return state >> shift; }
    static constexpr unsigned char getIndex(size_t state) { return state & mask; }
    static constexpr size_t        makeState(size_t size, unsigned char index) { return (size << shift) | index; }

    auto getBuf(unsigned char i) const -> std::span< T > { return {std::next(alloc_.get(), size_ * i), size_}; }

    size_t                         size_ = 0;
    std::unique_ptr< T[] >         alloc_;
    std::array< unsigned char, 3 > indices_{0, 1, 2};
    alignas(64) std::atomic< size_t > state_{makeState(0, consumed)};
};

template < typename T >
template < std::invocable< const std::span< T > > Producer >
void WaitFreeTripleBuf< T >::produce(Producer&& producer)
{
    const auto [published, swap, busy] = indices_;
    const auto buf                     = getBuf(swap);
    const auto size                    = std::invoke([&] -> size_t {
        if constexpr (std::convertible_to< std::invoke_result_t< Producer, std::span< T > >, size_t >)
            return std::invoke(std::forward< Producer >(producer), buf);
        else
        {
            std::invoke(std::forward< Producer >(producer), buf);
            return size_;
        }
    });
    const auto new_state               = makeState(size, swap);
    const int  prev_state              = state_.exchange(new_state, std::memory_order_release);
    if (getIndex(prev_state) == consumed)
        std::ranges::rotate(indices_, std::next(indices_.begin()));
    else
        std::swap(indices_[0], indices_[1]);
}

template < typename T >
auto WaitFreeTripleBuf< T >::consume() -> std::span< T >
{
    const auto prev_state = state_.exchange(consumed, std::memory_order_acquire);
    const auto index = getIndex(prev_state);
    return index != consumed ? getBuf(index).first(getSize(prev_state)) : std::span< T >{};
}
} // namespace wf3b
#endif // WAITFREETRIPLEBUF_HPP
