#include <iostream>
#include "SkipList.h"

int main() {
    
    SkipList<int> sl;
    std::cout<<"create list"<<std::endl;
    // Insert some elements
    sl.insert(10);
    std::cout<<"insert 10"<<std::endl;
    sl.insert(20);
    std::cout<<"insert 20"<<std::endl;
    sl.insert(30);
    std::cout<<"insert 30"<<std::endl;
    sl.insert(15);
    std::cout<<"insert 15"<<std::endl;

    std::cout << "SkipList contents after insertion:\n";
    for (auto it = sl.begin(); it != sl.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << "\n";
    
    std::cout<<sl.validateLinks()<<std::endl;
    
    // Check contains
    std::cout << "Contains 30? " << (sl.contains(30) ? "Yes" : "No") << "\n";
    std::cout << "Contains 100? " << (sl.contains(100) ? "Yes" : "No") << "\n";

    // Test find
    auto it = sl.find(15);
    if (it != sl.end()) {
        std::cout << "Found 15 in SkipList.\n";
    } else {
        std::cout << "Did not find 15 in SkipList.\n";
    }

    // Erase an element
    std::cout << "Erasing 20...\n";
    sl.erase(20);

    std::cout << "SkipList contents after erasing 20:\n";
    for (auto val : sl) {
        std::cout << val << " ";
    }
    std::cout << "\n";

    // Validate skip list structure
    std::cout << "Structure valid? " << (sl.validate() ? "Yes" : "No") << "\n";
    sl.printAllLevels();
    
    return 0;
}

