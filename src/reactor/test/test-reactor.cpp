#include <reactor/reactor.hpp>

#include <memory>
#include <sys/socket.h>
#include <thread>

#include <gtest/gtest.h>

class ConcreteEventHandler : public EventHandler {
  public:
    ConcreteEventHandler(int id, char *buffer, size_t buffer_size) : id(id), buffer(buffer), buffer_size(buffer_size) {
        memset(buffer, 0, buffer_size);
    }

    int getId() override { return id; }

    void execute() override { read(id, buffer, buffer_size); }

  private:
    int id;
    char *buffer;
    size_t buffer_size;
};

class TestReactorPattern : public ::testing::Test {
  public:
    virtual void SetUp() {
        socketpair(AF_LOCAL, SOCK_STREAM, 0, sv);

        reactor = std::make_unique<Reactor>();
        concrete_event_handler_a = std::make_unique<ConcreteEventHandler>(sv[0], buffer_a, sizeof(buffer_a));
        concrete_event_handler_b = std::make_unique<ConcreteEventHandler>(sv[1], buffer_b, sizeof(buffer_b));
        reactor->subscribe(concrete_event_handler_a.get());
        reactor->subscribe(concrete_event_handler_b.get());
        monitoring_thread = std::make_unique<std::thread>([&] {
            reactor_stopped = false;
            reactor->start();
            reactor_stopped = true;
        });
    }

    virtual void TearDown() {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        reactor->stop();
        if (monitoring_thread->joinable())
            monitoring_thread->join();
        close(sv[0]);
        close(sv[1]);
    }

  protected:
    std::unique_ptr<Reactor> reactor;
    std::unique_ptr<EventHandler> concrete_event_handler_a, concrete_event_handler_b;
    std::unique_ptr<std::thread> monitoring_thread;
    bool reactor_stopped;
    int sv[2];
    char buffer_a[16], buffer_b[16];
    const char *inputpath = "./tmp.0.socket";
    const char expected_buffer_a[9] = "BUFFER_A", expected_buffer_b[9] = "BUFFER_B";
};

TEST_F(TestReactorPattern, GivenReactorStartedThenRunInLoop) {
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    EXPECT_FALSE(reactor_stopped);
}

TEST_F(TestReactorPattern, GivenReactorStartedWhenStopThenReactorStops) {
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    reactor->stop();
    monitoring_thread->join();
    EXPECT_TRUE(reactor_stopped);
}

TEST_F(TestReactorPattern, GivenReactorStartedWhenEventSusbcribedThenEventExecute) {
    EXPECT_STREQ("", buffer_a);
    write(concrete_event_handler_b->getId(), expected_buffer_a, sizeof(expected_buffer_a));
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    EXPECT_STREQ(expected_buffer_a, buffer_a);
}

TEST_F(TestReactorPattern, GivenReactorStartedWhenBothEventSusbcribedThenBothEventExecute) {
    EXPECT_STREQ("", buffer_a);
    EXPECT_STREQ("", buffer_b);
    write(concrete_event_handler_b->getId(), expected_buffer_a, sizeof(expected_buffer_a));
    write(concrete_event_handler_a->getId(), expected_buffer_b, sizeof(expected_buffer_b));
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    EXPECT_STREQ(expected_buffer_a, buffer_a);
    EXPECT_STREQ(expected_buffer_b, buffer_b);
}

TEST_F(TestReactorPattern, GivenReactorStartedWhenEventUnSusbcribedThenEventNotExecute) {
    EXPECT_STREQ("", buffer_a);
    EXPECT_STREQ("", buffer_b);
    reactor->unsubscribe(concrete_event_handler_a.get());
    write(concrete_event_handler_b->getId(), expected_buffer_a, sizeof(expected_buffer_a));
    write(concrete_event_handler_a->getId(), expected_buffer_b, sizeof(expected_buffer_b));
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    EXPECT_STREQ("", buffer_a);
    EXPECT_STREQ(expected_buffer_b, buffer_b);
}
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
