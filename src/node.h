typedef struct pawn* node;
struct pawn {
    uint32_t x;
    uint32_t y;
    node parent;
    node left;
    node right;
};

node create_node(uint32_t x,uint32_t y);

void add(node* root_ptr, node element);

node find(node root, uint32_t x, uint32_t y);

bool is_bigger(node a, uint32_t x, uint32_t y);
