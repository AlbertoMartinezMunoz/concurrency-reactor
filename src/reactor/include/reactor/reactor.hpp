#ifndef SRC_REACTOR_INCLUDE_REACTOR_REACTOR_H_
#define SRC_REACTOR_INCLUDE_REACTOR_REACTOR_H_
#ifndef CONCURRENCY_PATTERNS_INCLUDE_CONCURRENCY_PATTERNS_REACTOR_H_
#define CONCURRENCY_PATTERNS_INCLUDE_CONCURRENCY_PATTERNS_REACTOR_H_

#include <fcntl.h>
#include <sys/select.h>
#include <unistd.h>
#include <vector>

class EventHandler {
  public:
    virtual int getId() = 0;
    virtual void execute() = 0;
};

class Reactor {
  public:
    Reactor() {
        pipe(wakeuppfd);
        int flags = fcntl(wakeuppfd[0], F_GETFL);
        flags |= O_NONBLOCK;
        fcntl(wakeuppfd[0], F_SETFL, flags);
        flags = fcntl(wakeuppfd[1], F_GETFL);
        flags |= O_NONBLOCK;
        fcntl(wakeuppfd[1], F_SETFL, flags);
    }

    void subscribe(EventHandler *h) { handlers.push_back(h); }

    void unsubscribe(EventHandler *h) { std::erase(handlers, h); }

    void start() {
        run = true;
        fd_set readfds;

        while (run) {
            FD_ZERO(&readfds);
            FD_SET(wakeuppfd[0], &readfds);

            for (auto h : handlers)
                FD_SET(h->getId(), &readfds);

            select(FD_SETSIZE, &readfds, NULL, NULL, NULL);
            for (auto h : handlers) {
                if (FD_ISSET(h->getId(), &readfds))
                    h->execute();
            }
        }
    }

    void stop() {
        run = false;
        write(wakeuppfd[1], "x", 1);
    }

  private:
    bool run;
    int wakeuppfd[2];
    std::vector<EventHandler *> handlers;
};

#endif // CONCURRENCY_PATTERNS_INCLUDE_CONCURRENCY_PATTERNS_REACTOR_H_


#endif  // SRC_REACTOR_INCLUDE_REACTOR_REACTOR_H_
