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