module mizar::server::pool;
import std::thread;

alias JobFn = fn void(void* arg);

struct ThreadWorker {
    thread::Thread th;
    bool running;
}

struct WorkerPool {
    int thread_count;
}

fn void WorkerPool.init(&this, int thread_count = 4) {
    this.thread_count = thread_count;
}
