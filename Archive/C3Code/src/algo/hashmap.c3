module mizar::algo::hashmap;
import mizar::algo::arena;
import mizar::algo::siphash;

struct HashMapEntry {
    String key;
    void* val;
    ulong hash;
    bool occupied;
    bool deleted;
}

struct HashMap {
    HashMapEntry* entries;
    usz capacity;
    usz count;
    bool case_insensitive;
    arena::Arena* arena;
}

fn bool str_eq_ci(String a, String b) {
    if (a.len != b.len) return false;
    for (int i = 0; i < a.len; i++) {
        char ca = a[i];
        char cb = b[i];
        if (ca >= 'A' && ca <= 'Z') ca += 32;
        if (cb >= 'A' && cb <= 'Z') cb += 32;
        if (ca != cb) return false;
    }
    return true;
}

fn void HashMap.init(&this, usz initial_cap = 16, bool case_insensitive = false, arena::Arena* arena = null) {
    usz cap = 16;
    while (cap < initial_cap) {
        cap *= 2;
    }
    this.capacity = cap;
    this.count = 0;
    this.case_insensitive = case_insensitive;
    this.arena = arena;

    usz bytes = HashMapEntry::size * cap;
    if (arena != null) {
        this.entries = (HashMapEntry*)arena.calloc(cap, HashMapEntry::size);
    } else {
        this.entries = (HashMapEntry*)mem::malloc((sz)bytes);
        if (this.entries != null) {
            mem::set(this.entries, 0, (sz)bytes);
        }
    }
}

fn void HashMap.free(&this) {
    if (this.arena == null && this.entries != null) {
        mem::free(this.entries);
        this.entries = null;
    }
    this.capacity = 0;
    this.count = 0;
}

fn ulong HashMap.calc_hash(&this, String key) {
    return this.case_insensitive ? siphash::hash_ci(key) : siphash::hash(key);
}

fn bool HashMap.keys_match(&this, String a, String b) {
    if (this.case_insensitive) {
        return str_eq_ci(a, b);
    }
    return a == b;
}

fn bool HashMap.resize(&this, usz new_cap) {
    HashMapEntry* old_entries = this.entries;
    usz old_cap = this.capacity;

    usz bytes = HashMapEntry::size * new_cap;
    HashMapEntry* new_entries = null;
    if (this.arena != null) {
        new_entries = (HashMapEntry*)this.arena.calloc(new_cap, HashMapEntry::size);
    } else {
        new_entries = (HashMapEntry*)mem::malloc((sz)bytes);
        if (new_entries != null) {
            mem::set(new_entries, 0, (sz)bytes);
        }
    }
    if (new_entries == null) return false;

    this.entries = new_entries;
    this.capacity = new_cap;
    this.count = 0;

    usz mask = new_cap - 1;
    for (usz i = 0; i < old_cap; i++) {
        if (old_entries[i].occupied && !old_entries[i].deleted) {
            ulong h = old_entries[i].hash;
            usz idx = (usz)(h & (ulong)mask);
            while (this.entries[idx].occupied) {
                idx = (idx + 1) & mask;
            }
            this.entries[idx].key = old_entries[i].key;
            this.entries[idx].val = old_entries[i].val;
            this.entries[idx].hash = h;
            this.entries[idx].occupied = true;
            this.entries[idx].deleted = false;
            this.count++;
        }
    }

    if (this.arena == null && old_entries != null) {
        mem::free(old_entries);
    }
    return true;
}

fn bool HashMap.set(&this, String key, void* val) {
    if (this.capacity == 0) {
        this.init();
    }
    // Load factor > 0.70
    if ((this.count + 1) * 10 >= this.capacity * 7) {
        if (!this.resize(this.capacity * 2)) return false;
    }

    ulong h = this.calc_hash(key);
    usz mask = this.capacity - 1;
    usz idx = (usz)(h & (ulong)mask);
    usz first_deleted = (usz)-1;

    while (this.entries[idx].occupied) {
        if (this.entries[idx].deleted) {
            if (first_deleted == (usz)-1) {
                first_deleted = idx;
            }
        } else if (this.entries[idx].hash == h && this.keys_match(this.entries[idx].key, key)) {
            this.entries[idx].val = val;
            return true;
        }
        idx = (idx + 1) & mask;
    }

    usz target = first_deleted != (usz)-1 ? first_deleted : idx;

    String stored_key = key;
    if (this.arena != null) {
        stored_key = this.arena.strdup(key);
    }

    this.entries[target].key = stored_key;
    this.entries[target].val = val;
    this.entries[target].hash = h;
    this.entries[target].occupied = true;
    this.entries[target].deleted = false;
    this.count++;
    return true;
}

fn void* HashMap.get(&this, String key) {
    if (this.capacity == 0 || this.count == 0) return null;
    ulong h = this.calc_hash(key);
    usz mask = this.capacity - 1;
    usz idx = (usz)(h & (ulong)mask);

    while (this.entries[idx].occupied) {
        if (!this.entries[idx].deleted && this.entries[idx].hash == h && this.keys_match(this.entries[idx].key, key)) {
            return this.entries[idx].val;
        }
        idx = (idx + 1) & mask;
    }
    return null;
}

fn bool HashMap.remove(&this, String key) {
    if (this.capacity == 0 || this.count == 0) return false;
    ulong h = this.calc_hash(key);
    usz mask = this.capacity - 1;
    usz idx = (usz)(h & (ulong)mask);

    while (this.entries[idx].occupied) {
        if (!this.entries[idx].deleted && this.entries[idx].hash == h && this.keys_match(this.entries[idx].key, key)) {
            this.entries[idx].deleted = true;
            this.count--;
            return true;
        }
        idx = (idx + 1) & mask;
    }
    return false;
}
