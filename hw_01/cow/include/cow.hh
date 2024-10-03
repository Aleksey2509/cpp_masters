#include <cstddef>
#include <cstring>
#include <iterator>
#include <iostream>
#include <limits>
#include <string>
#include <memory>
#include <string_view>

namespace cow {

template <typename CharT, typename CharTraits = std::char_traits<CharT>>
struct tokenizer final {
    tokenizer(std::basic_string_view<CharT, CharTraits> str, std::basic_string_view<CharT, CharTraits> delims,
              std::shared_ptr<const CharT[]> handle)
        : remaining_str_(str), delims_(delims), handle_(handle)
    {
    }

    class iterator final {
    public:
        using difference_type = std::ptrdiff_t;
        using value_type = std::basic_string<CharT, CharTraits>;
        using pointer = value_type*;
        using reference = value_type&;
        // after its exhaustion cannot run through the remaining_str_, so not forward_iterator_tag
        using iterator_category = std::input_iterator_tag;

    private:
        void get_next_token()
        {
            auto next_token_start = remaining_str_.find_first_not_of(delims_);
            if (next_token_start == remaining_str_.npos)
            {
                // std::cout << "Remained final: |" << remaining_str_ << "|" << std::endl;
                next_token_start = remaining_str_.size();
            }
            remaining_str_.remove_prefix(next_token_start);
            if (remaining_str_ == "")
            {
                delims_ = "";
                current_token_ = "";
                return;
            }
            auto next_token_end = remaining_str_.find_first_of(delims_, next_token_start);
            if (next_token_end == remaining_str_.npos)
            {
                // std::cout << "Remained final: |" << remaining_str_ << "|" << std::endl;
                next_token_end = remaining_str_.size();
            }
            // std::cout << "start, end: " << next_token_start << " " << next_token_end << std::endl;
            current_token_ = remaining_str_.substr(0, next_token_end);
            // std::cout << "Current: |" << current_token_ << "|" << std::endl;
            remaining_str_.remove_prefix(next_token_end);
            // std::cout << "Remained: |" << remaining_str_ << "|" << std::endl;
        }

    public:
        iterator(const std::string_view str, const std::string_view delims) : remaining_str_(str), delims_(delims)
        {
            get_next_token();
        }

        bool operator==(const iterator& rhs)
        {
            return (remaining_str_ == rhs.remaining_str_) && (delims_ == rhs.delims_) &&
                   (current_token_ == rhs.current_token_);
        }
        bool operator!=(const iterator& rhs)
        {
            return !(*this == rhs);
        }

        iterator& operator++()
        {
            get_next_token();
            return *this;
        }

        iterator operator++(int)
        {
            iterator tmp(*this);
            ++(*this);
            return tmp;
        }

        value_type operator*()
        {
            return std::string(current_token_);
        }

    private:
        std::basic_string_view<CharT, CharTraits> current_token_;
        std::basic_string_view<CharT, CharTraits> remaining_str_;
        std::basic_string_view<CharT, CharTraits> delims_;
    };

    iterator begin()
    {
        return iterator {remaining_str_, delims_};
    }
    iterator end()
    {
        return iterator {"", ""};
    }

private:
    std::basic_string_view<CharT, CharTraits> remaining_str_;
    std::basic_string_view<CharT, CharTraits> delims_;
    std::shared_ptr<const CharT[]> handle_;  // get shared_ptr to data so the pointer does not dangle
};

// actually use CharT
template <typename CharT, typename CharTraits = std::char_traits<CharT>>
class cow_string final {
public:
    cow_string() = default;

    cow_string(std::basic_string_view<CharT, CharTraits> str) : cow_string(str.data(), str.size()) {}

    cow_string(const CharT* ptr) : cow_string(ptr, CharTraits::length(ptr)) {}

    cow_string(const CharT* ptr, size_t size) : size_(size + 1), capacity_(size_)
    {
        data_ = copy_data(ptr, size, capacity_);
    }

    std::shared_ptr<CharT[]> copy_data(const CharT* ptr, size_t size, size_t capacity)
    {
        std::shared_ptr<CharT[]> data = std::shared_ptr<char[]>(new CharT[capacity]);
        std::copy(ptr, ptr + size, data.get());
        data[size] = 0;

        return data;
    }

    void make_unique_owner()
    {
        auto new_data = copy_data(data_.get(), size_, capacity_);
        std::swap(new_data, data_);
    }

    char& operator[](size_t pos)
    {
        if (!data_.unique())
            make_unique_owner();

        return data_[pos];
    }

    const char& operator[](size_t pos) const
    {
        return data_[pos];
    }

    // iterators
    using iterator = char*;
    using const_iterator = const char*;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    // clang-format off
    reverse_iterator rbegin() { return reverse_iterator(end()); }
    reverse_iterator rend() { return reverse_iterator(begin()); }

    const_reverse_iterator rbegin() const { return const_reverse_iterator(cend()); }
    const_reverse_iterator rend() const { return const_reverse_iterator(cbegin()); }

    const_reverse_iterator crbegin() const { return const_reverse_iterator(cend()); }
    const_reverse_iterator crend() const { return const_reverse_iterator(cbegin()); }

    iterator begin() { return data_.get(); }
    iterator end() { return data_.get() + size(); }

    const_iterator begin() const { return data_.get(); }
    const_iterator end() const { return data_.get() + size(); }

    const_iterator cbegin() const { return data_.get(); }
    const_iterator cend() const { return data_.get() + size(); }
    // clang-format on

    // observers
    size_t size() const noexcept
    {
        return size_ - 1;  // return without the terminating null
    }
    size_t capacity() const noexcept
    {
        return capacity_;
    }
    CharT* data() noexcept
    {
        return data_.get();
    }
    const CharT* data() const noexcept
    {
        return data_.get();
    }

    void print(std::basic_ostream<CharT, CharTraits>& os) const
    {
        os << data_.get();
    }

    void reserve(size_t capacity)
    {
        if (capacity <= capacity_)
            return;
        auto enlarged = copy_data(data_.get(), size(), capacity);
        std::swap(enlarged, data_);
    }

    void resize(size_t new_size, CharT ch = CharT {})
    {
        auto old_size = size();
        auto enlarged = copy_data(data_.get(), std::min(size_, new_size), new_size + 1);
        if (new_size > old_size)
        {
            std::fill(enlarged.get() + old_size, enlarged.get() + new_size, ch);
            enlarged[new_size] = '\0';
            size_ = new_size + 1;
        }
        std::swap(enlarged, data_);
    }

    tokenizer<CharT, CharTraits> tokenize(std::basic_string_view<CharT, CharTraits> delims)
    {
        return {{data_.get(), size()}, delims, data_};
    }

    size_t find_substr(std::basic_string_view<CharT, CharTraits> substr) const
    {
        auto substr_size = substr.size();
        auto* begin_ptr = begin();
        auto* end_ptr = end();
        auto first_elem = substr[0];
        auto* pos_of_first = CharTraits::find(data_.get(), size_, first_elem);
        auto offset = pos_of_first - begin_ptr;
        while (pos_of_first + substr_size < end_ptr)
        {
            if (!CharTraits::compare(pos_of_first, substr.data(), substr_size))
                return offset;
            pos_of_first = CharTraits::find(pos_of_first + substr_size, size_ - offset - substr_size, first_elem);
            offset = pos_of_first - begin_ptr;
        }
        return npos;
    }

private:
    std::shared_ptr<CharT[]> data_;
    size_t size_ {};
    size_t capacity_ {};

    static constexpr size_t npos = std::numeric_limits<size_t>::max();
};

template <typename CharT, typename CharTraits>
std::basic_ostream<CharT, CharTraits>& operator<<(std::basic_ostream<CharT, CharTraits>& os,
                                                  const cow_string<CharT, CharTraits>& str)
{
    str.print(os);
    return os;
}
}  // namespace cow
