#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <cstddef>
#include <stdexcept>
#include <utility>

template <typename T>
class LinkedList
{
private:
    struct Node
    {
        T value;
        Node *prev = nullptr;
        Node *next = nullptr;

        explicit Node(const T &v) : value(v) {}
        explicit Node(T &&v) : value(std::move(v)) {}
    };

    Node *head_ = nullptr;
    Node *tail_ = nullptr;
    std::size_t size_ = 0;

    Node *nodeAt(std::size_t index) const
    {
        if (index >= size_)
            throw std::out_of_range("LinkedList index out of range");

        if (index < size_ / 2)
        {
            Node *cur = head_;
            for (std::size_t i = 0; i < index; ++i)
                cur = cur->next;
            return cur;
        }

        Node *cur = tail_;
        for (std::size_t i = size_ - 1; i > index; --i)
            cur = cur->prev;
        return cur;
    }

public:
    class iterator
    {
        friend class LinkedList<T>;

    private:
        Node *node_ = nullptr;

        explicit iterator(Node *node) : node_(node) {}

    public:
        iterator() = default;

        T &operator*() const { return node_->value; }
        T *operator->() const { return &node_->value; }

        iterator &operator++()
        {
            if (node_)
                node_ = node_->next;
            return *this;
        }

        iterator operator++(int)
        {
            iterator old = *this;
            ++(*this);
            return old;
        }

        iterator &operator--()
        {
            if (node_)
                node_ = node_->prev;
            return *this;
        }

        iterator operator--(int)
        {
            iterator old = *this;
            --(*this);
            return old;
        }

        iterator operator+(std::ptrdiff_t offset) const
        {
            iterator it = *this;
            if (offset >= 0)
            {
                while (offset-- > 0 && it.node_)
                    it.node_ = it.node_->next;
            }
            else
            {
                while (offset++ < 0 && it.node_)
                    it.node_ = it.node_->prev;
            }
            return it;
        }

        iterator operator-(std::ptrdiff_t offset) const
        {
            return *this + (-offset);
        }

        iterator &operator+=(std::ptrdiff_t offset)
        {
            *this = *this + offset;
            return *this;
        }

        iterator &operator-=(std::ptrdiff_t offset)
        {
            *this = *this - offset;
            return *this;
        }

        bool operator==(const iterator &other) const { return node_ == other.node_; }
        bool operator!=(const iterator &other) const { return node_ != other.node_; }
    };

    class const_iterator
    {
        friend class LinkedList<T>;

    private:
        const Node *node_ = nullptr;

        explicit const_iterator(const Node *node) : node_(node) {}

    public:
        const_iterator() = default;
        const_iterator(const iterator &it) : node_(it.node_) {}

        const T &operator*() const { return node_->value; }
        const T *operator->() const { return &node_->value; }

        const_iterator &operator++()
        {
            if (node_)
                node_ = node_->next;
            return *this;
        }

        const_iterator operator++(int)
        {
            const_iterator old = *this;
            ++(*this);
            return old;
        }

        const_iterator operator+(std::ptrdiff_t offset) const
        {
            const_iterator it = *this;
            if (offset >= 0)
            {
                while (offset-- > 0 && it.node_)
                    it.node_ = it.node_->next;
            }
            else
            {
                while (offset++ < 0 && it.node_)
                    it.node_ = it.node_->prev;
            }
            return it;
        }

        const_iterator operator-(std::ptrdiff_t offset) const
        {
            return *this + (-offset);
        }

        bool operator==(const const_iterator &other) const { return node_ == other.node_; }
        bool operator!=(const const_iterator &other) const { return node_ != other.node_; }
    };

    LinkedList() = default;

    LinkedList(const LinkedList &other)
    {
        for (const auto &value : other)
            push_back(value);
    }

    LinkedList(LinkedList &&other) noexcept
        : head_(other.head_), tail_(other.tail_), size_(other.size_)
    {
        other.head_ = nullptr;
        other.tail_ = nullptr;
        other.size_ = 0;
    }

    LinkedList &operator=(const LinkedList &other)
    {
        if (this != &other)
        {
            clear();
            for (const auto &value : other)
                push_back(value);
        }
        return *this;
    }

    LinkedList &operator=(LinkedList &&other) noexcept
    {
        if (this != &other)
        {
            clear();
            head_ = other.head_;
            tail_ = other.tail_;
            size_ = other.size_;
            other.head_ = nullptr;
            other.tail_ = nullptr;
            other.size_ = 0;
        }
        return *this;
    }

    ~LinkedList()
    {
        clear();
    }

    bool empty() const { return size_ == 0; }
    std::size_t size() const { return size_; }

    iterator begin() { return iterator(head_); }
    iterator end() { return iterator(nullptr); }
    const_iterator begin() const { return const_iterator(head_); }
    const_iterator end() const { return const_iterator(nullptr); }
    const_iterator cbegin() const { return const_iterator(head_); }
    const_iterator cend() const { return const_iterator(nullptr); }

    void clear()
    {
        Node *cur = head_;
        while (cur)
        {
            Node *next = cur->next;
            delete cur;
            cur = next;
        }
        head_ = nullptr;
        tail_ = nullptr;
        size_ = 0;
    }

    void push_back(const T &value)
    {
        Node *node = new Node(value);
        if (!tail_)
        {
            head_ = node;
            tail_ = node;
        }
        else
        {
            node->prev = tail_;
            tail_->next = node;
            tail_ = node;
        }
        ++size_;
    }

    void push_back(T &&value)
    {
        Node *node = new Node(std::move(value));
        if (!tail_)
        {
            head_ = node;
            tail_ = node;
        }
        else
        {
            node->prev = tail_;
            tail_->next = node;
            tail_ = node;
        }
        ++size_;
    }

    iterator erase(iterator pos)
    {
        Node *node = pos.node_;
        if (!node)
            return end();

        Node *next = node->next;
        if (node->prev)
            node->prev->next = node->next;
        else
            head_ = node->next;

        if (node->next)
            node->next->prev = node->prev;
        else
            tail_ = node->prev;

        delete node;
        --size_;
        return iterator(next);
    }

    T &operator[](std::size_t index) { return nodeAt(index)->value; }
    const T &operator[](std::size_t index) const { return nodeAt(index)->value; }
};

#endif // LINKED_LIST_H
