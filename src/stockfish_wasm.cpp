#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <string>
#include <sstream>
#include <memory>

#include "bitboard.h"
#include "position.h"
#include "types.h"
#include "tune.h"
#include "uci.h"
#include "search.h"

using namespace emscripten;
using namespace Stockfish;

// Redirect stdout to a string buffer for WebAssembly
class StdoutRedirect {
    std::stringstream buffer;
    std::streambuf* old_buffer;
public:
    StdoutRedirect() : old_buffer(std::cout.rdbuf(buffer.rdbuf())) {}
    ~StdoutRedirect() { std::cout.rdbuf(old_buffer); }
    std::string str() const { return buffer.str(); }
    void clear() { buffer.str(""); }
};

// WebAssembly-specific engine wrapper
class WasmEngine {
public:
    WasmEngine() : stdoutRedirect(std::make_unique<StdoutRedirect>()) {}

    // Initialize the engine
    std::string init() {
        // Initialize all components
        Bitboards::init();
        Position::init();
        
        // Create UCI engine instance
        char* argv[] = {(char*)"stockfish"};
        uciEngine = std::make_unique<UCIEngine>(1, argv);
        
        // Initialize tuning parameters
        Tune::init(uciEngine->engine_options());

        // Send initial UCI commands
        executeCommand("uci");
        executeCommand("isready");

        return "initialized";
    }

    // Process a move in the current position
    std::string makeMove(const std::string& moveStr) {
        if (!uciEngine) return "Engine not initialized";

        stdoutRedirect->clear();
        std::string cmd = "position fen " + currentFEN + " moves " + moveStr;
        executeCommand(cmd);
        
        // Update current FEN after move
        executeCommand("d");  // This will print the position
        currentFEN = getCurrentFEN();
        
        return "ok";
    }

    // Set a position from FEN
    std::string setPosition(const std::string& fen) {
        if (!uciEngine) return "Engine not initialized";

        stdoutRedirect->clear();
        std::string cmd = "position fen " + fen;
        executeCommand(cmd);
        
        currentFEN = fen;
        return "ok";
    }

    // Start search and return best move
    std::string getBestMove(const std::string& fen, int depth) {
        if (!uciEngine) return "Engine not initialized";

        stdoutRedirect->clear();

        // Set position if provided
        if (!fen.empty() && fen != "current") {
            setPosition(fen);
        }
        
        // Start search
        std::string cmd = "go depth " + std::to_string(depth);
        executeCommand(cmd);
        
        return stdoutRedirect->str();
    }

private:
    std::unique_ptr<StdoutRedirect> stdoutRedirect;
    std::unique_ptr<UCIEngine> uciEngine;
    std::string currentFEN;

    void executeCommand(const std::string& cmd) {
        std::istringstream is(cmd);
        uciEngine->loop();  // This will process one command and return
    }

    std::string getCurrentFEN() {
        // Parse the FEN from the 'd' command output
        std::string output = stdoutRedirect->str();
        size_t fenStart = output.find("Fen: ") + 5;
        size_t fenEnd = output.find("\n", fenStart);
        return output.substr(fenStart, fenEnd - fenStart);
    }
};

// Global instance
std::unique_ptr<WasmEngine> wasmEngine;

// JavaScript interface functions
std::string init() {
    if (!wasmEngine)
        wasmEngine = std::make_unique<WasmEngine>();
    if (!wasmEngine)
        return "object no created";
    return wasmEngine->init();
}

std::string setPosition(const std::string& fen) {
    if (!wasmEngine) return "Engine not initialized";
    return wasmEngine->setPosition(fen);
}

std::string makeMove(const std::string& move) {
    if (!wasmEngine) return "Engine not initialized";
    return wasmEngine->makeMove(move);
}

std::string getBestMove(const std::string& fen, int depth) {
    if (!wasmEngine) return "Engine not initialized";
    return wasmEngine->getBestMove(fen, depth);
}

// Expose functions to JavaScript
EMSCRIPTEN_BINDINGS(stockfish_module) {
    function("init", &init);
    function("setPosition", &setPosition);
    function("makeMove", &makeMove);
    function("getBestMove", &getBestMove);
}
