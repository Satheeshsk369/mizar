module mizar::algo::arena;

struct ArenaBlock {
    ArenaBlock* next;
    usz size;
    usz used;
    // Memory follows directly after struct in contiguous allocation
}

const usz ARENA_ALIGN = 16;

fn usz align_up(usz n, usz align) {
    return (n + align - 1) & ~(align - 1);
}

struct Arena {
    ArenaBlock* head;
    usz default_block_size;
    usz total_allocated;
}

fn void Arena.init(&this, usz default_block_size = 4096) {
    this.head = null;
    this.default_block_size = default_block_size < 1024 ? 4096 : default_block_size;
    this.total_allocated = 0;
}

fn ArenaBlock* new_block(usz size) {
    usz total_size = ArenaBlock::size + size;
    ArenaBlock* block = (ArenaBlock*)mem::malloc((sz)total_size);
    if (block == null) return null;
    block.next = null;
    block.size = size;
    block.used = 0;
    return block;
}

fn void* Arena.alloc(&this, usz size) {
    if (size == 0) return null;
    usz aligned_size = align_up(size, ARENA_ALIGN);

    if (this.head == null || (this.head.used + aligned_size > this.head.size)) {
        usz block_size = this.default_block_size;
        if (aligned_size > block_size) {
            block_size = aligned_size;
        }
        ArenaBlock* block = new_block(block_size);
        if (block == null) return null;
        block.next = this.head;
        this.head = block;
        this.total_allocated += block_size;
    }

    char* base = (char*)(this.head + 1);
    void* ptr = (void*)(base + this.head.used);
    this.head.used += aligned_size;
    return ptr;
}

fn void* Arena.calloc(&this, usz count, usz size) {
    usz total = count * size;
    void* ptr = this.alloc(total);
    if (ptr != null) {
        mem::set(ptr, 0, (sz)total);
    }
    return ptr;
}

fn String Arena.strdup(&this, String str) {
    if (str.len == 0) return "";
    char* mem = (char*)this.alloc((usz)str.len + 1);
    if (mem == null) return "";
    mem::copy(mem, str.ptr, (sz)str.len);
    mem[str.len] = '\0';
    return (String)mem[0..str.len - 1];
}

fn void Arena.reset(&this) {
    ArenaBlock* curr = this.head;
    while (curr != null) {
        curr.used = 0;
        curr = curr.next;
    }
}

fn void Arena.free(&this) {
    ArenaBlock* curr = this.head;
    while (curr != null) {
        ArenaBlock* next = curr.next;
        mem::free(curr);
        curr = next;
    }
    this.head = null;
    this.total_allocated = 0;
}
