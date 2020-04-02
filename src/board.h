typedef struct board* board_t;

struct board {
    uint32_t width;
    uint32_t height;
    uint64_t* players;
    bool* dfs_visited;
    uint64_t* funion_parent;
};
bool union_operation(board_t b,uint32_t x1,uint32_t y1,uint32_t x2,__uint32_t y2);
void set_funion_parent(board_t b,uint32_t funion_parent_x,uint32_t funion_parent_y,uint32_t x,uint32_t y);
void set_dfs_visited(board_t b,bool dfs_visited,uint32_t x,uint32_t y);
void set_player(board_t b,uint64_t player,uint32_t x,uint32_t y);
uint64_t get_funion_parent(board_t b,uint32_t x,uint32_t y);
bool get_dfs_visited(board_t b,uint32_t x,uint32_t y);
uint64_t get_player(board_t b,uint32_t x,uint32_t y);
void delete_board(board_t b);
board_t create_board(uint32_t width,uint32_t height);
