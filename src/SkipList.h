#ifndef SKIPLIST_H
#define SKIPLIST_H

#include <vector>
#include <iostream>
#include <memory>
#include <functional>
#include <utility>
#include <random>

template<typename T>
class SkipNode
{
public:
    T data;
    SkipNode* left = nullptr;
    SkipNode* right = nullptr;
    SkipNode* down = nullptr;

    SkipNode() : data(T()), left(nullptr), right(nullptr), down(nullptr) {};
    SkipNode(const T& indata) : data(indata), left(nullptr), right(nullptr), down(nullptr) {}
    ~SkipNode() = default;
};

template<typename T,typename Compare = std::less<>, typename Allocator = std::allocator<T>>
class SkipList
{
public:
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
        const SkipNode<T>* current;
    };
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
    using allocator_type = Allocator;
    //allocator
    Allocator get_allocator() const { return _alloc; }
    
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
        : _alloc(alloc),
          _node_alloc(alloc),
          _comp(comp),
          MAX_LVL(DEFAULT_MAX_LVL)
    {
        head = create_node(T());
        tail = create_node(T());
        head->right = tail;
        tail->left = head;
        head->down = nullptr;
        tail->down = nullptr;

    }
    SkipList(const SkipList& other)
        : _alloc(std::allocator_traits<Allocator>::select_on_container_copy_construction(other._alloc)),
          _node_alloc(_alloc),
          _comp(other._comp),
          current_max_level(other.current_max_level),
          MAX_LVL(other.MAX_LVL),
          _size(other._size)
    {
        head = create_node(T());
        tail = create_node(T());
        head->right = tail;
        tail->left = head;


        SkipNode<T>* other_curr = other.getHeadAtLevel(1)->right;
        SkipNode<T>* prev_new_node = head;

        while (other_curr != other.getTailAtLevel(1))
        {
            SkipNode<T>* new_node = create_node(other_curr->data);
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
        : _alloc(std::allocator_traits<Allocator>::select_on_container_copy_construction(other._alloc)),
          _node_alloc(_alloc),
          _comp(std::move(other._comp)),
          head(other.head),
          tail(other.tail),
          current_max_level(other.current_max_level),
          MAX_LVL(other.MAX_LVL),
          _size(other._size)
    {
        other.head = create_node(T());
        other.tail = create_node(T());
        other.head->right = other.tail;
        other.tail->left = other.head;
        other.current_max_level = 1;
        other._size = 0;
    };
    
    ~SkipList() noexcept
    {
        SkipNode<T>* current_level = head;
        while (current_level != nullptr) {
            SkipNode<T>* current_node = current_level;
            current_level = current_level->down;
            while (current_node != nullptr) {
                SkipNode<T>* next_node = current_node->right;
                delete_node(current_node);
                current_node = next_node;
            }
        }
    }

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
        if (this != &other)
        {
            SkipList temp(std::move(other));
            swap(*this, temp);
        }

        other.head = create_node(T());
        other.tail = create_node(T());
        other.head->right = other.tail;
        other.tail->left = other.head;
        other.current_max_level = 1;
    }

    bool empty() const noexcept
    {
        SkipNode<T>* bottom_head = getHeadAtLevel(1);
        return bottom_head->right == getTailAtLevel(1);
    }
    std::pair<iterator, bool> insert(const T& idata)
    {
        if(contains(idata))
        {
            return {iterator(findNode(idata)), false};
        }
        int level = random_level();
        std::vector<SkipNode<T>*> update(MAX_LVL + 1, nullptr);
        
        //std::cout << "random_level: " << level << ", MAX_LVL: " << MAX_LVL << std::endl;//debug
        
        SkipNode<T>* current_node = head;

        for (int l = current_max_level; l >= 1; --l)
        {
                        
            while(current_node != nullptr && current_node->right != nullptr && current_node->right != getTailAtLevel(l) && _comp(current_node->right->data, idata))
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
                SkipNode<T>* new_head = create_node(T());
                SkipNode<T>* new_tail = create_node(T());
                
                if(!new_head || !new_tail)
                {
                    throw std::bad_alloc();
                }
                
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
            //std::cout << "Inserting " << idata << " at level " << l << std::endl;//debug
            
            SkipNode<T>* new_node = create_node(idata);
            if (!update[l] || !new_node)
            {
                throw std::runtime_error("Invalid insertion state");
            }
            new_node->right = update[l]->right;
            new_node->left = update[l];
            

            if (update[l]->right) {
                update[l]->right->left = new_node;
            }
            update[l]->right = new_node;
            
            new_node->down = lower_node;
            lower_node = new_node;
            

            if (new_node->left->right != new_node || 
                (new_node->right && new_node->right->left != new_node)) {
                std::cerr << "Link corruption detected!" << std::endl;
                throw std::runtime_error("Pointer corruption during insert");
            }
        }
        ++_size;
        SkipNode<T>* return_node = findLowerNode(idata, 1);
        return {iterator(return_node), true};
    }
    template <typename InputIt>
    void insert(InputIt first, InputIt last)
    {
        if(first == last) return;
        for (; first != last; ++first)
        {
            insert(*first);
        }
    }
    template <typename... Args>
    iterator emplace(Args&&... args)
    {
        T value(std::forward<Args>(args)...);
        auto [it, inserted] = insert(std::move(value));
        return it;
    }
    
    void merge(SkipList&& other)
    {
        if(this == &other) return;
        auto it = other.begin();
        while(it != other.end())
        {
            if(!contains(*it))
            {
                insert(std::move(*it));
                it = other.erase(it);
            }else
            {
                ++it;
            }
        }
    }

    template <typename K>
    bool erase(const K& value)
    {
        SkipNode<T>* node = findNode(value);
        if (node)
        {
            //std::cout << "Erasing node with data " << node->data << std::endl; //debug
            
            eraseNode(node);
            return true;
        }
        return false;
    }
    iterator erase(iterator pos)
    {
        if (pos == end())
        {
            throw std::out_of_range("Cannot erase end() iterator");
        }
        if(!validate_iterator(pos))
        {
            throw std::invalid_argument("Invalid iterator");
        }

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

    size_t max_size() const
    {
        size_t max_size = MAX_LVL;
        return max_size;
    }
    size_t size() const { return _size; }

    Compare key_comp() { return _comp; }
    allocator_type get_allocator() {return _alloc; };

    friend void swap(SkipList& a, SkipList& b) noexcept(
        std::allocator_traits<Allocator>::propagate_on_container_swap::value ||
        std::allocator_traits<Allocator>::is_always_equal::value
    )
    {
        using std::swap;
        swap(a.head, b.head);
        swap(a.tail, b.tail);
        swap(a.current_max_level, b.current_max_level);
        swap(a._comp, b._comp);
        swap(a._size, b._size);

        
        if constexpr (std::allocator_traits<Allocator>::propagate_on_container_swap::value)
        {
            swap(a._alloc, b._alloc);
            swap(a._node_alloc, b._node_alloc);
        }
        
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
                delete_node(current_node);
                current_node = next_node;
            }
        }
        head = create_node(T());
        tail = create_node(T());
        head->right = tail;
        tail->left = head;
        current_max_level = 1;
        _size = 0;
    }
    bool validate() const
    {
        for (int lvl = 1; lvl <= current_max_level; ++lvl) {
            SkipNode<T>* node = getHeadAtLevel(lvl);
            while (node != getTailAtLevel(lvl)) {

                if (node->right && node->right->left != node) {
                    std::cerr << "Pointer mismatch at level " << lvl << std::endl;
                    return false;
                }

                if (node->right != getTailAtLevel(lvl) && node->right && _comp(node->right->data, node->data)) {
                    std::cerr << "Order violation at level " << lvl 
                              << ": " << node->data << " > " << node->right->data << std::endl;
                    return false;
                }

                if (lvl > 1 && !node->down) {
                    std::cerr << "Missing down pointer at level " << lvl << " node: "<< node->data << std::endl;
                    return false;
                }
                
                node = node->right;
            }
        }
        return true;
    }
    bool validateLinks() const
    {
        for(int lvl = 1; lvl <= current_max_level; ++lvl)
        {
            SkipNode<T>* node = getHeadAtLevel(lvl);
            while(node && node != getTailAtLevel(lvl))
            {
                if(!node->right || node->right->left != node)
                {
                    std::cerr<< "Link broken at level " << lvl 
                            << " on node " << node->data << std::endl;
                    return false;
                }
                node = node->right;
            }
        }
        return true;
    }
    
    friend bool operator==(const SkipList& lsl, const SkipList& rsl)
    {
        if(lsl.size() != rsl.size()) return false;

        auto itl = lsl.begin();
        auto itr = rsl.begin();
        while(itl != lsl.end() && itr != rsl.end())
        {
            if(!(*itl == *itr)) return false;
            ++itl;
            ++itr;
        }
        return true;
    }
    friend bool operator!=(const SkipList& lsl, const SkipList& rsl)
    {
        return !(lsl == rsl);
    }
    friend bool operator<(const SkipList& lsl, const SkipList& rsl)
    {
        auto itl = lsl.begin();
        auto itr = rsl.begin();

        while(itl != lsl.end() && itr != rsl.end())
        {
            if(*itl < *itr) return true;
            if(*itr < *itl) return false;
            ++itl;
            ++itr;
        }
        return lsl.size() < rsl.size();
    }
    friend bool operator<=(const SkipList& lsl, const SkipList& rsl)
    {
        return !(rsl < lsl);
    }
    friend bool operator>(const SkipList& lsl, const SkipList& rsl)
    {
        return rsl < lsl;
    }
    friend bool operator>=(const SkipList& lsl, const SkipList& rsl)
    {
        return !(lsl < rsl);
    }


    void printLevel(int level) const
    {
        SkipNode<T>* node = getHeadAtLevel(level);
        std::cout << "Level " << level << ": ";
        while (node != getTailAtLevel(level)) {
            std::cout << node->data << " ";
            node = node->right;
        }
        std::cout << std::endl;
    }
    void printAllLevels() const
    {
        for (int lvl = current_max_level; lvl >= 1; --lvl) {
            printLevel(lvl);
        }
    }
    
    
private:
    Allocator _alloc;
    node_allocator _node_alloc;
    Compare _comp;
    SkipNode<T>* head;
    SkipNode<T>* tail;
    int current_max_level = 1;
    static constexpr int DEFAULT_MAX_LVL = 16;
    int MAX_LVL = DEFAULT_MAX_LVL;
    size_t _size = 0;

    SkipNode<T>* create_node(const T& value)
    {
        SkipNode<T>* node = _node_alloc.allocate(1);
        try
        {
            std::allocator_traits<node_allocator>::construct(_node_alloc, node, value);
            node->left = nullptr;
            node->right = nullptr;
            node->down = nullptr;
            //std::cout << "Node allocated at: " << node << std::endl; // debug
        }catch(...)
        {
            std::cout<<"caught error"<<std::endl;
            _node_alloc.deallocate(node, 1);
            throw;
        }
        return node;
    }
    void delete_node(SkipNode<T>* node)
    {
        if(node)
        {
            std::allocator_traits<node_allocator>::destroy(_node_alloc, node);
            _node_alloc.deallocate(node, 1);
        }        
    }
    int random_level()
    {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::bernoulli_distribution dist(0.5);

        int level = 1;
        while (dist(gen) && level < MAX_LVL) {
            level++;
        }
        return level;

    }
    SkipNode<T>* getHeadAtLevel(int level) const
    {
        if (level < 1 || level > current_max_level) return nullptr;
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
        if (level < 1 || level > current_max_level) return nullptr;
        SkipNode<T>* current_tail = tail;
        int current_level = current_max_level;
        while ((level < current_level) && (current_tail->down))
        {
            current_tail = current_tail->down;
            current_level--;
        }
        return current_tail;
    }
    void copyLevel(const SkipList& other, int level)
    {
        SkipNode<T>* other_head = other.getHeadAtLevel(level);
        SkipNode<T>* other_curr = other_head->right;
        SkipNode<T>* new_head = create_node(T());
        SkipNode<T>* new_tail = create_node(T());

        new_head->right = new_tail;
        new_tail->left = new_head;

        new_head->down = getHeadAtLevel(level - 1);
        new_tail->down = getTailAtLevel(level - 1);

        SkipNode<T>* prev_new_node = new_head;
        while(other_curr != other.getTailAtLevel(level))
        {
            SkipNode<T>* new_node = create_node(other_curr->data);
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
        SkipNode<T>* current = getHeadAtLevel(level);
        
        if (!current || !current->right)
        {
            std::cerr << "Invalid node traversal!" << std::endl;
            return nullptr;
        }
        
        SkipNode<T>* level_tail = getTailAtLevel(level);
        while (current && current != level_tail && current->data < data)
        {
            current = current->right;
        }
        return current ? current : nullptr;
    }
    template <typename K>
    SkipNode<T>* findNode(const K& key) const
    {
        SkipNode<T>* current_node = head;
        if (!current_node || !current_node->right)
        {
            std::cerr << "Invalid node traversal!" << std::endl;
            return nullptr;
        }
        for (int l = current_max_level; l >= 1; --l)
        {
            SkipNode<T>* current_tail = getTailAtLevel(l);
            while (current_node->right != current_tail && _comp(current_node->right->data, key))
            {
                current_node = current_node->right;
            }
            if (current_node->right != current_tail && !_comp(key, current_node->right->data) && !_comp(current_node->right->data, key))
            {
                return current_node->right;
            }
            current_node = current_node->down;
        }
        return nullptr;
    }
    void eraseNode(SkipNode<T>* node)
    {
        while (node) {
            SkipNode<T>* next_level = node->down;
            
            if (node->left) node->left->right = node->right;  
            if (node->right) node->right->left = node->left;  
            
            delete_node(node);
            node = next_level;
        }
        --_size;

        trimEmptyLevels();
        
        if (!validate()) {
            std::cerr << "Structure corrupted after erase!" << std::endl;
        }
    }
    void trimEmptyLevels()
    {
        while(current_max_level > 1 && head->right == tail)
        {
            SkipNode<T>* old_head = head;
            SkipNode<T>* old_tail = tail;
            
            head = head->down;
            tail = tail->down;
            
            delete_node(old_head);
            delete_node(old_tail);

            --current_max_level;
        }
    }
    bool validate_iterator(const_iterator it) const
    {
        for (auto curr = begin(); curr != end(); ++curr) {
            if (curr == it) return true;
        }
        return false;
    }
};

#endif
