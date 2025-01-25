#include <iostream>
#include <functional>
#include <string>
#include <mutex>
#include "bitboard.h"
#include "misc.h"
#include "position.h"
#include "types.h"
#include "uci.h"
#include "tune.h"
#include "bridge.h"


using namespace Stockfish;


extern "C" {

    UCIEngine uci(0, nullptr);

    void Initialize(UnityCallback callback){
        Bitboards::init();
        Position::init();

        Tune::init(uci.engine_options());
        RegisterCallback(callback);
    }

    const char* ExecuteCommand(const char* cmd) {
        static std::string result;
        uci.ExecuteCommand(std::string(cmd));
    }
}


// Store the callback function pointer
static UnityCallback unity_callback = nullptr;
static std::mutex callback_mutex;

void RegisterCallback(UnityCallback callback) {
    std::lock_guard<std::mutex> lock(callback_mutex);
    unity_callback = callback;
} 

void TriggerEvent(const std::string& message) {
    std::lock_guard<std::mutex> lock(callback_mutex);
    if (unity_callback) {
        unity_callback(message.c_str()); // Call the Unity callback
    }
}