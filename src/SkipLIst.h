#ifndef SKIPLIST_H
#define SKIPLIST_H

#include <cstdlib>
#include <ctime>
#include <vector>
#include <iostream>
#include <memory>
#include <functional>

template<typename T>
class SkipNode
{
public:
    T data;
    SkipNode* left = nullptr;
    SkipNode* right = nullptr;
    SkipNode* down = nullptr;

    SkipNode() = default;
    explicit SkipNode(T indata) : data(indata) {}
    ~SkipNode() = default;
};

template<typename T,typename Compare = std::less<>, typename Allocator = std::allocator<T>>
class SkipList
{
public:
    //types initialization
    using value_type = T;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using node_allocator = typename std::allocator_traits<Allocator>::template rebind_alloc<SkipNode<T>>;
    //allocator
    Allocator get_allocator() const { return _alloc;}
    //iterators 
    class iterator
    {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        iterator(SkipNode<T>* node = nullptr) : current(node) {}

        reference operator*() const { return current->data; }
        pointer operator->() const { return &current->data; }

        iterator& operator++()
        {
            if (current)
            {
                current = current->right;
            }
            return *this;
        }

        iterator operator++(int)
        {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const iterator& other) const { return current == other.current; }
        bool operator!=(const iterator& other) const { return current != other.current; }
        
    private:
        SkipNode<T>* current;
    };
    //add const
    class const_iterator
    {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = const T;
        using difference_type = std::ptrdiff_t;
        using pointer = const T*;
        using reference = const T&;

        const_iterator(const SkipNode<T>* node = nullptr) : current(node) {}

        reference operator*() const { return current->data; }
        pointer operator->() const { return &current->data; }

        const_iterator& operator++()
        {
            if (current) current = current->right;
            return *this;
        }

        const_iterator operator++(int)
        {
            const_iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const const_iterator& other) const { return current == other.current; }
        bool operator!=(const const_iterator& other) const { return current != other.current; }
        
    private:
        SkipNode<T>* current;
    };
    iterator begin() noexcept
    {
        SkipNode<T>* node = head;
        while (node->down) node = node->down;
        return iterator(node->right);
    };
    iterator end() noexcept
    {
        SkipNode<T>* node = tail;
        while(node->down) node = node->down;
        return iterator(node);
    };
    const_iterator cbegin() const noexcept
    {
        const SkipNode<T>* node = head;
        while (node->down) node = node->down;
        return const_iterator(node->right); 
    };
    const_iterator cend() const noexcept
    {
        const SkipNode<T>* node = tail;
        while(node->down) node = node->down;
        return const_iterator(node);
    };
    const_iterator begin() const noexcept { return cbegin(); }
    const_iterator end() const noexcept { return cend(); }
    reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
    reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
    const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(cend()); }
    const_reverse_iterator crend() const noexcept { return const_reverse_iterator(cbegin()); }
    const_reverse_iterator rbegin() const noexcept { return crbegin(); }
    const_reverse_iterator rend() const noexcept { return crend(); }
    
    // Constructor and destructor
    explicit SkipList(const Compare& comp = Compare(), const Allocator& alloc = Allocator()) 
    {
        head = new SkipNode<T>();
        tail = new SkipNode<T>();
        head->right = tail;
        tail->left = head;
        srand(time(nullptr));
    }
    SkipList(const SkipList& other)
        : _comp(other._comp),
          _alloc(other._alloc),
          current_max_level(other.current_max_level),
          MAX_LVL(other.MAX_LVL),
          _size(other._size)
    {
        srand(time(nullptr));

        if (other.empty())
        {
            head = new SkipNode<T>();
            tail = new SkipNode<T>();
            head->right = tail;
            tail->left = head;
        }
        //start with creating bottom layer
        SkipNode<T>* other_curr = other.getHeadAtLevel(1)->right;
        SkipNode<T>* prev_new_node = head;

        while (other_curr != other.getTailAtLevel(1))
        {
            SkipNode<T>* new_node = new SkipNode<T>(other_curr->data);
            new_node->left = prev_new_node;
            prev_new_node->right = new_node;
            prev_new_node = new_node;
            other_curr = other_curr->right;
        }

        prev_new_node->right = tail;
        tail->left = prev_new_node;

        for (int lvl = 2; lvl <= current_max_level; lvl++)
        {
            copyLevel(other, lvl);
        }
    };
    SkipList(SkipList&& other) noexcept 
        : _comp(std::move(other._comp)),
          _alloc(std::move(other._alloc)),
          head(other.head),
          tail(other.tail),
          current_max_level(other.current_max_level),
          MAX_LVL(other.MAX_LVL),
          _size(other._size)
    {
        other.head = SkipNode<T>();
        other.tail = SkipNode<T>();
        other.head->right = other.tail;
        other.tail->left = other.head;
        other.current_max_level = 1;
    };
    
    ~SkipList() noexcept
    {
        SkipNode<T>* current_level = head;
        while (current_level != nullptr) {
            SkipNode<T>* current_node = current_level;
            current_level = current_level->down;
            while (current_node != nullptr) {
                SkipNode<T>* next_node = current_node->right;
                delete current_node;
                current_node = next_node;
            }
        }
    }

    SkipNode<T>* head;
    SkipNode<T>* tail;
    int current_max_level = 1;
    const int MAX_LVL = 16;
    size_t _size = 0;

    SkipList& operator=(SkipList& other) noexcept
    {
        if (this != &other)
        {
            SkipList temp(other);
            swap(*this, temp);
        }
        return *this;
    }
    SkipList& operator=(SkipList&& other) noexcept
    {
        clear();

        head = other.head;
        tail = other.tail;
        current_max_level = other.current_max_level;
        MAX_LVL = other.MAX_LVL;
        _comp = std::move(other._comp);
        _alloc = std::move(other._alloc);
        _size = other._size;

        other.head = SkipNode<T>();
        other.tail = SkipNode<T>();
        other.head->right = other.tail;
        other.tail->left = other.head;
        other.current_max_level = 1;
    }

    bool empty() const noexcept
    {
        SkipNode<T>* bottom_head = getHeadAtLevel(1);
        return bottom_head == getTailAtLevel(1);
    }
    void insert(T idata)
    {
        int level = random_level();
        std::vector<SkipNode<T>*> update(MAX_LVL + 1, nullptr);
        SkipNode<T>* current_node = head;

        for (int l = current_max_level; l >= 1; --l)
        {
            while (current_node->right != tail && _comp(current_node->right->data, idata))
            {
                current_node = current_node->right;
            }
            update[l] = current_node;
            current_node = current_node->down;
        }

        if (level > current_max_level)
        {
            for (int l = current_max_level + 1; l <= level; ++l)
            {
                SkipNode<T>* new_head = new SkipNode<T>();
                SkipNode<T>* new_tail = new SkipNode<T>();
                new_head->right = new_tail;
                new_tail->left = new_head;
                new_head->down = head;
                new_tail->down = tail;
                head = new_head;
                tail = new_tail;
                update[l] = head;
            }
            current_max_level = level;
        }

        SkipNode<T>* lower_node = nullptr;
        for (int l = 1; l <= level; ++l)
        {
            SkipNode<T>* new_node = new SkipNode<T>(idata);
            new_node->right = update[l]->right;
            new_node->left = update[l];
            update[l]->right->left = new_node;
            update[l]->right = new_node;
            new_node->down = lower_node;
            lower_node = new_node;
        }
        ++_size;
    }
    template <typename InputIt>
    void insert(InputIn first, InputIt last)
    {
        for (; first != last; ++first)
        {
            insert(*first);
        }
    }
    template <typename... Args>
    iterator emplace(Args&&... args)
    {
        T value(std::froward<Args>(args)...);
        return insert(std::move(value)).first;
    }
    
    void merge(SkipList&& other)
    {
        for(auto it = other.begin(); it != other.end(); )
        {
            auto next = std::next(it);
            if(!contains(*it))
            {
                insert(std::move(*it));
                other.erase(it);
            }
            it = next;
        }
    }

    template <typename K>
    bool erase(const K& value)
    {
        SkipNode<T>* node = findNode(value);
        if (node)
        {
            eraseNode(node);
            return true;
        }
        return false;
    }
    iterator erase(iterator pos)
    {
        if (pos == end()) return end();

        SkipNode<T>* node = pos.current;
        ++pos;
        eraseNode(node);
        return pos;
    }
    iterator erase(const_iterator pos)
    {
        return erase(iterator(pos.current));
    }
    iterator erase(const_iterator first, const_iterator last)
    {
        if(first == begin() && last == end())
        {
            clear();
        }
        while(first != last)
        {
            first = erase(first);
        }
        return iterator(first.current);
    }

    template <typename K>
    iterator find(const K& key)
    {
        SkipNode<T>* node = findNode(key);
        return (node) ? iterator(node) : end();
    }
    template <typename K>
    const_iterator find(const K& key) const
    {
        const SkipNode<T>* node = findNode(key);
        return (node) ? const_iterator(node) : cend();
    }
    bool contains(const T& value) const 
    {
        return findNode(value) != nullptr;
    }
    template <typename K>
    iterator lower_bound(const K& key)
    {
        SkipNode<T>* node = head;

        for(int lvl = current_max_level; lvl >= 1; --lvl)
        {
            SkipNode<T>* curr_tail = getTailAtLevel(lvl);
            while(node->right != curr_tail && _comp(node->right->data, key))
            {
                node = node->right;
            }
            if(lvl > 1) node = node->down;
        }
        return iterator(node->right);
    }
    template <typename K>
    iterator upper_bound(const K& key)
    {
        iterator it = lower_bound(key);
        return (it != end() && !_comp(key, *it)) ? ++it : it;
    }


    size_t size() const { return _size; }

    friend void swap(SkipList& a, SkipList& b)
    {
        std::swap(a.head, b.head);
        std::swap(a.tail, b.tail);
        std::swap(a.current_max_level, b.current_max_level);
        std::swap(a.MAX_LVL, b.MAX_LVL);
        std::swap(a._comp, b._comp);
        std::swap(a._alloc, b._alloc);
        std::swap(a._size, b._size)
    }
    void clear()
    {
        SkipNode<T>* current_level = head;
        while(current_level != nullptr)
        {
            SkipNode<T>* current_node = current_level;
            while(current_node != nullptr)
            {
                SkipNode<T>* next_node = current_node->right;
                delete current_node;
                current_node = next_node;
            }
        }
        head = new SkipNode<T>();
        tail = new SkipNode<T>();
        head->right = tail;
        tail-left = head;
        current_max_level = 1;
        _size = 0;
    }
    bool validate() const
    {
        for(int lvl = 1; lvl <= current_max_level; ++lvl)
        {
            SkipNode<T>* node = getHeadAtLevel(lvl)->right;
            while(node != getTailAtLevel(lvl))
            {
                if(_comp(node->right->data, node->data)) return false;
                node = node->right;
            }
        }
        return true;
    }
private:
    //allocator and comparator
    Allocator _alloc;
    Compare _comp;
    
    int random_level()
    {
        int level = 1;
        while (rand() % 2 == 0 && level < MAX_LVL)
        {
            level++;
        }
        return level;
    }
    SkipNode<T>* getHeadAtLevel(int level) const
    {
        SkipNode<T>* current_head = head;
        int current_level = current_max_level;
        while ((level < current_level) && (current_head->down))
        {
            current_head = current_head->down;
            current_level--;
        }
        return current_head;
    }
    SkipNode<T>* getTailAtLevel(int level) const
    {
        SkipNode<T>* current_tail = tail;
        int current_level = current_max_level;
        while ((level < current_level) && (current_tail->down))
        {
            current_tail = current_tail->down;
            current_level--;
        }
        return current_tail;
    }
    void copyLevel(const SkipNode<T>& other, int level)
    {
        SkipNode<T>* other_head = other.getHeadAtLevel(level);
        SkipNode<T>* other_curr = other_head->right;
        SkipNode<T>* new_head = new SkipNode<T>();
        SkipNode<T>* new_tail = new SkipNode<T>();

        new_head->right = tail;
        new_tail->left = head;

        new_head->down = getHeadAtLevel(level - 1);
        new_tail->down = getTailAtLevel(level - 1);

        SkipNode<T>* prev_new_node = new_head;
        while(other_curr != other.getTailAtLevel(level))
        {
            SkipNode<T>* new_node = new SkipNode<T>(other_curr->data);
            new_node->left = prev_new_node;
            prev_new_node->right = new_node;
            prev_new_node = new_node;

            SkipNode<T>* lower_node = findLowerNode(new_node->data, level-1);
            new_node->down = lower_node;

            other_curr = other_curr->right;
        }

        prev_new_node->right = new_tail;
        new_tail->left = prev_new_node;
    }
    SkipNode<T>* findLowerNode(const T& data, int level)
    {
        SkipNode<T>* current = getHeadAtLevel(level)->right;
        SkipNode<T>* level_tail = getTailAtLevel(level);
        while (current != level_tail && current->data < data)
        {
            current = current->right;
        }
        return current->left;
    }
    template <typename K>
    SkipNode<T>* findNode(const K& key) const
    {
        SkipNode<T>* current_node = head;
        for (int l = current_max_level; l >= 1; --l)
        {
            SkipNode<T>* current_tail = getTailAtLevel(l);
            while (current_node->right != current_tail && _comp(current_node->right->data, key))
            {
                current_node = current_node->right;
            }
            if (current_node->right != current_tail && !_comp(_Deduce_key, current_node->right->data))
            {
                return current_node->right;
            }
            current_node = current_node->down;
        }
        return nullptr;
    }
    void eraseNode(SkipNode<T>* node)
    {
        while(node)
        {
            SkipNode<T>* next_level = node->down;

            node->left->right = node->right;
            node->right->left = node->left;

            delte node;
            node = next_level;
        }
        --_size();

        trimEmptyLevels();
    }
    void trimEmptyLevels()
    {
        while(current_max_level > 1 && head->right == tail)
        {
            SkipNode<T>* old_head = head;
            SkipNode<T>* old_tail = tail;
            
            head = head->down;
            tail = tail->down;
            
            delete old_head;
            delete old_tail;

            --current_max_level;
        }
    }
};

#endif