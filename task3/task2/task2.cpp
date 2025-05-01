#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <unordered_map>
#include <cmath>
#include <fstream>
#include <random>
#include <sstream>

using namespace std;

template<typename T>
class Server {
public:
    Server() : running(false) {}

    void start() {
        running = true;
        server_thread = thread(&Server::process_tasks, this);
    }

    void stop() {
        running = false;
        cv.notify_all();
        if (server_thread.joinable()) {
            server_thread.join();
        }
    }

    size_t add_task(function<T()> task) {
        unique_lock<mutex> lock(mtx);
        size_t id = next_id++;
        task_queue.push({id, async(launch::deferred, task)});
        cv.notify_one();
        return id;
    }

    T request_result(size_t id) {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, [this, id] { return results.find(id) != results.end(); });
        return results[id];
    }

private:
    void process_tasks() {
        while (running) {
            unique_lock<mutex> lock(mtx);
            cv.wait(lock, [this] { return !task_queue.empty() || !running; });

            if (!task_queue.empty()) {
                auto task = move(task_queue.front()); 
                task_queue.pop();

                lock.unlock();
                T result = task.second.get();

                lock.lock();
                results[task.first] = result;
                cv.notify_all();
            }
        }
    }

    thread server_thread;
    bool running;
    queue<pair<size_t, future<T>>> task_queue;
    unordered_map<size_t, T> results;
    mutex mtx;
    condition_variable cv;
    size_t next_id = 0;
};

void client_sin(Server<double>& server, int task_count, const string& filename) {
    ofstream file(filename);
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(-3.14, 3.14);

    for (int i = 0; i < task_count; ++i) {
        double arg = dis(gen);
        size_t id = server.add_task([arg] { return sin(arg); });
        double result = server.request_result(id);
        file << "sin(" << arg << ") = " << result << endl;
    }
}

void client_sqrt(Server<double>& server, int task_count, const string& filename) {
    ofstream file(filename);
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0, 100);

    for (int i = 0; i < task_count; ++i) {
        double arg = dis(gen);
        size_t id = server.add_task([arg] { return sqrt(arg); });
        double result = server.request_result(id);
        file << "sqrt(" << arg << ") = " << result << endl;
    }
}

void client_pow(Server<double>& server, int task_count, const string& filename) {
    ofstream file(filename);
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(1, 10);

    for (int i = 0; i < task_count; ++i) {
        double base = dis(gen);
        double exp = dis(gen);
        size_t id = server.add_task([base, exp] { return pow(base, exp); });
        double result = server.request_result(id);
        file << base << "^" << exp << " = " << result << endl;
    }
}

void test_results(const string& filename, const string& task_type) {
    ifstream file(filename);
    string line;
    int errCounter = 0;
    while (getline(file, line)) {
        istringstream iss(line);
        double arg1, arg2, result;
        char eq;

        if (task_type == "sin") {
            string sin_label;
            iss >> sin_label >> arg1 >> eq >> result;
            double expected = sin(arg1); 
            if (abs(result - expected) > 1e-8) {
                cerr << "Error in " << filename << ": " << line << " expected " << expected << endl;
            }
        } else if (task_type == "sqrt") {
            string sqrt_label;
            iss >> sqrt_label >> arg1 >> eq >> result;
            double expected = sqrt(arg1); 
            if (abs(result - expected) > 1e-8) {
                cerr << "Error in " << filename << ": " << line << " expected " << expected << endl;
            }
        } else if (task_type == "pow") {
            
            char caret;
            iss >> arg1 >> caret >> arg2 >> eq >> result;
            double expected = pow(arg1, arg2);
            if (abs(result - expected) > 1e-0) {
                cerr << "Error " << errCounter <<" in " << filename << ": " << line << " expected " << expected << endl;
                errCounter+=1;
            }
        } else {
            cerr << "Unknown task type: " << task_type << endl;
            return;
        }
    }
}

int main() {
    Server<double> server;
    server.start();

    thread client1(client_sin, ref(server), 1000, "sin_results.txt");
    thread client2(client_sqrt, ref(server), 1000, "sqrt_results.txt");
    thread client3(client_pow, ref(server), 1000, "pow_results.txt");

    client1.join();
    client2.join();
    client3.join();

    server.stop();

    test_results("sin_results.txt", "sin");
    test_results("sqrt_results.txt", "sqrt");
    test_results("pow_results.txt", "pow");

    cout << "Testing completed." << endl;
    return 0;
}