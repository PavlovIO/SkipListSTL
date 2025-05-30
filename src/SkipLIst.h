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

template<typename T,typename Compare = std::less<T>, typename Allocator = std::allocator<T>>
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
    //allocator
    Allocator get_allocator() const { return _alloc;}
    //iterators 
    class iterator;
    class const_iterator;
//ask if should be named cbegin cend
//!!!begin / end - WIP 
    iterator begin() noexcept;
    iterator end() noexcept;
    const_iterator cbegin() const noexcept;
    const_iterator cend() cont noexcept;
    // Constructor and destructor
//!!!Constructos WIP
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
          MAX_LVL(other.MAX_LVL)
    {
        srand(time(nullptr));

        std::vector<SkipNode<T>*> to_down;
        SkipNode<T> other_cur_level = other.head;
        while(other_cur_level != nullptr)
        {
            SkipNode<T> other_cur_node = other_cur_level->right;
            SkipNode<T> new_head;
            SkipNode<T> new_tail;
            if(other_cur_level == other.head)
            {
                head = new_head;
                tail = new_tail;
            }
            while(other_cur_node->right != nullptr)
            {
                
                if(other_cur_node->down != nullptr)
                {
                    to_down
                }
            }

        }


    };
    SkipList(Skiplist&& other) noexcept 
    {};

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

    void insert(T idata)
    {
        int level = random_level();
        std::vector<SkipNode<T>*> update(MAX_LVL + 1, nullptr);
        SkipNode<T>* current_node = head;

        for (int l = current_max_level; l >= 1; --l)
        {
            while (current_node->right != tail && current_node->right->data < idata)
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
    }

    bool erase(T idata)
    {
        SkipNode<T>* current_node = head;
        bool found = false;

        for (int l = current_max_level; l >= 1; --l)
        {
            SkipNode<T>* current_tail = getTailAtLevel(l);
            while (current_node->right != current_tail && current_node->right->data < idata) 
            {
                current_node = current_node->right;
            }
            if (current_node->right != current_tail && current_node->right->data == idata)
            {
                found = true;
                SkipNode<T>* to_delete = current_node->right;
                current_node->right = to_delete->right;
                to_delete->right->left = current_node;
                delete to_delete;
            }
            current_node = current_node->down;
        }
        return found;
    }

    SkipNode<T>* find(T idata)
    {
        SkipNode<T>* current_node = head;
        for (int l = current_max_level; l >= 1; --l)
        {
            SkipNode<T>* current_tail = getTailAtLevel(l);
            while (current_node->right != current_tail && current_node->right->data < idata)
            {
                current_node = current_node->right;
            }
            if (current_node->right != current_tail && current_node->right->data == idata)
            {
                return current_node->right;
            }
            current_node = current_node->down;
        }
        return nullptr;
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
    SkipNode<T>* getTailAtLevel(int level)
    {
        SkipNode<T>* current_tail = tail;
        while (level < current_max_level)
        {
            current_tail = current_tail->down;
            level++;
        }
        return current_tail;
    }
};

#endif