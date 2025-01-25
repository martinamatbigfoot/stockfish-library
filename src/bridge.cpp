#include <iostream>
#include <functional>
#include <string>
#include <mutex>
#include <queue>
#include "bitboard.h"
#include "misc.h"
#include "position.h"
#include "types.h"
#include "uci.h"
#include "tune.h"
#include "bridge.h"


using namespace Stockfish;

// Declare a pointer for UCIEngine 

// Store the callback function pointer
std::queue<std::string>* eventQueue;
std::atomic<UnityCallback> unity_callback_atomic = nullptr;

UCIEngine* uci = nullptr;
bool initialized = false;
extern "C" {
    void Initialize(UnityCallback callback) { 
        if (!initialized) {
            // Initialize Stockfish components
            Bitboards::init();
            Position::init();
            eventQueue = new std::queue<std::string>();
            
            // Initialize UCIEngine
            uci = new UCIEngine(0, nullptr);
            
            Tune::init(uci->engine_options());

            // Register the Unity callback
            RegisterCallback(callback);

            TriggerEvent("initialized");

            initialized = true;
        }
    }

    void ExecuteCommand(const char* cmd) {
        if (!uci) {
            return;
        }
        uci->ExecuteCommand(std::string(cmd));
    }

    void Shutdown() {
        if (uci) {
            delete uci;
            uci = nullptr;
        }
    }

    std::string ProcessEventsFromNative() {
        if (!eventQueue->empty()) {
            std::string msg = eventQueue->front();
            eventQueue->pop();
            return msg;
        }
        return "not useful";
    }
}

void RegisterCallback(UnityCallback callback) {
    unity_callback_atomic.store(callback, std::memory_order_relaxed);
}

void TriggerEvent(const std::string message) {
    eventQueue->push(message);
    UnityCallback callback = unity_callback_atomic.load(std::memory_order_relaxed);
    if (callback) {
        callback(message.c_str()); // Directly invoke the Unity callback
    }
}