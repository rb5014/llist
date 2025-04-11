# llist - Lightweight Linked List Library  
*"Simple, efficient, and thread-safe linked list implementation in C"*  

[![Windows CI](https://github.com/yourusername/llist/actions/workflows/windows-ci.yml/badge.svg)](https://github.com/yourusername/llist/actions/workflows/windows-ci.yml)
[![Cross-Platform](https://github.com/yourusername/llist/actions/workflows/cross-platform.yml/badge.svg)](https://github.com/yourusername/llist/actions/workflows/cross-platform.yml)
[![Static Analysis](https://github.com/yourusername/llist/actions/workflows/analysis.yml/badge.svg)](https://github.com/yourusername/llist/actions/workflows/analysis.yml) 
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)

---

## 📦 Features  
- 🌐 **Cross-platform** (Windows/Linux/macOS)  
- 🔒 **Thread-safe** (optional via `MT_SUPPORT_TRUE`)  
- 📊 **Sorting** (ascending/descending via `SORT_LIST_ASC/DESC`)  
- 🧩 **Memory-safe** (custom destructors supported)  

---

## 🚀 Installation  

### Prerequisites
- **CMake 3.10+**
- **C11 compiler** (GCC, Clang, MSVC)
- **Threading library** (pthreads on Unix)

---

### Linux/macOS  
```bash
git clone https://github.com/rb5014/llist.git
cd llist
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release  # or Debug
make
sudo make install  # Default: /usr/local
```

### Windows

#### MinGW
```powershell
git clone https://github.com/rb5014/llist.git
cd llist
cmake -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release # or Debug
cmake --build build
cmake --install build  # Admin needed for default install
```

#### MSVC (Visual Studio)
```powershell
git clone https://github.com/rb5014/llist.git
cd llist
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release # or Debug
cmake --install build
```

---

## 🔧 Integration  

### CMake Projects

#### Add to your project's CMakeLists.txt:
```cmake
find_package(llist REQUIRED)
target_link_libraries(your_project PRIVATE llist)
```

#### Custom install path
```bash
cmake .. -DCMAKE_INSTALL_PREFIX=/custom/path
```

### Manual Linking
```bash
gcc your_app.c -o app -lllist -L/path/to/llist/lib
```

---

## 🔍 Verify Installation
```bash
# Unix
ls /usr/local/include/llist.h
ls /usr/local/lib/libllist.*

# Windows
dir "C:\Program Files (x86)\llist\bin\llist.dll"
```

---

## 🧠 Complete API Reference  

### 1. List Management  
| Function | Description |  
|----------|-------------|  
| `llist_t* llist_create(unsigned int flags)` | Creates new list (`MT_SUPPORT_TRUE` for thread-safety) |  
| `int llist_destroy(llist_t *list, int destroy_nodes, node_dtor_t destructor)` | Destroys list (optional node cleanup) |  
| `int llist_size(llist_t *list)` | Returns number of nodes |  
| `int llist_is_empty(llist_t *list)` | Returns 1 if empty, 0 otherwise |  

### 2. Node Operations  
| Function | Flags | Description |  
|----------|-------|-------------|  
| `int llist_add_node(llist_t *list, llist_node_t node, int flags)` | `ADD_NODE_FRONT`/`REAR` | Adds node to list |  
| `int llist_insert_node(llist_t *list, llist_node_t node, node_ident_t identifier, void *arg, int flags)` | `ADD_NODE_BEFORE`/`AFTER` | Inserts node relative to another |  
| `int llist_remove_node(llist_t *list, node_ident_t identifier, void *arg, int destroy_node, node_dtor_t destructor)` | - | Removes nodes matching criteria |  
| `llist_node_t llist_pop(llist_t *list)` | - | Removes and returns head node |  

### 3. Data Access  
| Function | Returns | Description |  
|----------|---------|-------------|  
| `llist_node_t llist_find_node(llist_t *list, node_ident_t identifier, void *arg)` | Matching node | Finds first matching node |  
| `llist_node_t llist_get_head(llist_t *list)` | Head node data | Returns head node |  
| `llist_node_t llist_get_tail(llist_t *list)` | Tail node data | Returns tail node |  
| `llist_node_t llist_get_node_at(llist_t *list, unsigned int index)` | Node at index | Gets node by position |  

### 4. List Operations  
| Function | Flags | Description |  
|----------|-------|-------------|  
| `int llist_sort(llist_t *list, node_cmp_t cmp_func, void *arg, int flags)` | `SORT_LIST_ASC`/`DESC` | Sorts list |  
| `int llist_reverse(llist_t *list)` | - | Reverses list order |  
| `int llist_append(llist_t *first, llist_t *second)` | - | Concatenates two lists |  
| `int llist_for_each(llist_t *list, node_func_t action, void *arg)` | - | Applies function to all nodes |  

### 5. Callback Typedefs  
```c
typedef int (*node_func_t)(llist_node_t node, unsigned int idx, void *arg);
typedef void (*node_dtor_t)(llist_node_t node);
typedef int (*node_cmp_t)(llist_node_t first, llist_node_t second, void *arg);
typedef int (*node_ident_t)(llist_node_t node, void *arg);
```

---

## 📚 Usage Examples  

### Basic Usage  
```c
#include <llist.h>

int main() {
    llist_t *list = llist_create(MT_SUPPORT_FALSE);
    int data = 42;
    
    llist_add_node(list, &data, ADD_NODE_FRONT);
    printf("List size: %d\n", llist_size(list));
    
    llist_destroy(list, 0, NULL);
    return 0;
}
```

### Thread-Safe Example  
```c
llist_t *thread_safe_list = llist_create(MT_SUPPORT_TRUE);
// Safe to use across threads...
```

### Custom Sorting  
```c
int compare_nodes(llist_node_t a, llist_node_t b, void *arg) {
    return *(int*)a - *(int*)b; 
}
llist_sort(list, compare_nodes, NULL, SORT_LIST_ASC);
```

---

## ⚠️ Error Handling  
Check `llist_errno` after operations:  
```c
typedef enum _LLIST {
    LLIST_SUCCESS,            // Operation succeeded  
    LLIST_NULL_ARGUMENT,      // NULL argument passed  
    LLIST_NODE_NOT_FOUND,     // Node not found  
    LLIST_OUT_OF_RANGE,       // Index out of bounds  
    LLIST_MALLOC_ERROR,       // Memory allocation failed  
    LLIST_MULTITHREAD_ISSUE   // Thread-safety error  
} E_LLIST;
```

---

## 📜 License  
MIT License - Free for commercial and personal use.  

---

## 🛠️ Development  

### Build Options  
```cmake
option(BUILD_TESTING "Build tests" ON)
option(BUILD_SHARED_LIBS "Build as shared library" ON)
```

### Running Tests  
```bash
cd build && ctest --output-on-failure
```

---

## 🤝 Contributing  
1. Fork the repository  
2. Create a feature branch (`git checkout -b feature/your-feature`)  
3. Commit changes (`git commit -m 'Add some feature'`)  
4. Push to branch (`git push origin feature/your-feature`)  
5. Open a Pull Request  

---

## 🚨 Troubleshooting  

### Common Issues  
**Error**: `llist.dll not found`  
✅ Solution:  
```powershell
$env:PATH += ";C:\path\to\llist\bin"
```

**Error**: `undefined reference to llist_*`  
✅ Ensure proper linking:  
```bash
gcc your_app.c -o app -lllist -L/path/to/llist/lib
```

**Error**: `Thread safety issues`  
✅ Create list with:  
```c
llist_create(MT_SUPPORT_TRUE);
```

---

## 📦 Package Managers  
### vcpkg (Windows)  
```powershell
vcpkg install llist
```

### Linux (Debian/Ubuntu)  
```bash
sudo apt install libllist-dev
```
