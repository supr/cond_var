#include <atomic>
#include <condition_variable>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

typedef struct thread_ctx thread_ctx_t;

struct thread_ctx {
  std::mutex m;
  std::condition_variable cv;
  bool ready;
  int buffer[8];
  int run;

  thread_ctx() : m(), cv(), ready(false), buffer{0}, run(1) {}
};

void producer(const std::shared_ptr<thread_ctx_t> &ctx) {
  while (true) {
    std::unique_lock<std::mutex> lg(ctx->m);
    ctx->cv.wait(lg, [&] { return !(ctx->ready); });
    if (ctx->run >= 5) {
      ctx->ready = true;
      ctx->cv.notify_one();
      return;
    }
    std::cout << "producer(" << ctx->run << "): ";
    for (int i = 0; i < 8; i++) {
      auto val = ctx->run * 10 + i;
      ctx->buffer[i] = val;
      std::cout << val << " ";
    }
    std::cout << "\b." << std::endl;
    ctx->ready = true;
    ctx->cv.notify_one();
  }
}

void consumer(const std::shared_ptr<thread_ctx_t> &ctx) {
  while (true) {
    std::unique_lock<std::mutex> lg(ctx->m);
    ctx->cv.wait(lg, [&] { return ctx->ready; });
    if (ctx->run >= 5) {
      ctx->ready = false;
      ctx->cv.notify_one();
      return;
    }
    std::cout << "consumer(" << ctx->run << "): ";
    for (int i = 0; i < 8; i++) {
      std::cout << ctx->buffer[i] << " ";
    }
    std::cout << "\b." << std::endl;
    ctx->ready = false;
    ctx->run++;
    ctx->cv.notify_one();
  }
}

int main(int argc, char **argv) {
  std::shared_ptr<thread_ctx_t> ctx = std::make_shared<thread_ctx_t>();
  std::vector<std::thread> threads;
  threads.push_back(std::thread(producer, ctx));
  threads.push_back(std::thread(consumer, ctx));

  for (auto &th : threads) {
    th.join();
  }
  return 0;
}
