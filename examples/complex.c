#include <stc/cstr.h>

void check_drop(float* v) {printf("destroy %g\n", *v);}

#define i_type FloatStack
#define i_val float
#define i_valdrop check_drop
#define i_valclone(x) x  // required to allow cloning when i_valdrop is defined
                         // (not for carc as it does not use i_valclone to clone).
#include <stc/cstack.h>

#define i_type StackList
#define i_val_bind FloatStack
#define i_opt c_no_cmp
#include <stc/clist.h>

#define i_type ListMap
#define i_key int
#define i_val_bind StackList
#include <stc/cmap.h>

#define i_type MapMap
#define i_key_str
#define i_val_bind ListMap
#include <stc/cmap.h>

// c++:
// using FloatStack = std::stack<float>;
// using map_lst = std::unordered_map<int, std::forward_list<array2f>>;
// using map_map = std::unordered_map<std::string, map_lst>;

int main() {
    int xdim = 4, ydim = 6;
    int x = 1, tableKey = 42;
    const char* strKey = "first";

    c_auto (MapMap, mmap)
    {
        FloatStack stack = FloatStack_with_size(xdim * ydim, 0);

        // Put in some data in stack array
        stack.data[x] = 3.1415927f;
        printf("stack size: %" c_zu "\n", FloatStack_size(&stack));

        StackList list = StackList_init();
        StackList_push_back(&list, stack);

        ListMap lmap = ListMap_init();
        ListMap_insert(&lmap, tableKey, list);
        MapMap_insert(&mmap, cstr_from(strKey), lmap);

        // Access the data entry
        const ListMap* lmap_p = MapMap_at(&mmap, strKey);
        const StackList* list_p = ListMap_at(lmap_p, tableKey);
        const FloatStack* stack_p = StackList_back(list_p);
        printf("value (%d) is: %f\n", x, *FloatStack_at(stack_p, x));

        stack.data[x] = 1.41421356f; // change the value in array
    }
}
