#ifndef BRIDGE_H
#define BRIDGE_H

#include <string>

// Unity callback function type
using UnityCallback = void (*)(const char* message);

// External functions for Unity to call
extern "C" {
    // Initializes Stockfish and registers the Unity callback
    void Initialize(UnityCallback callback);

    // Executes a command in Stockfish
    void ExecuteCommand(const char* cmd);

    // Shuts down Stockfish and cleans up resources
    void Shutdown();

    std::string ProcessEventsFromNative();
}

// Functions for handling Unity callbacks (internal use within the library)
void RegisterCallback(UnityCallback callback);
void TriggerEvent(const std::string& message);

#endif // BRIDGE_H