extern "C" {
    void Initialize();

    void ExecuteCommand(const char* cmd);
}


// Define a type for the callback
using UnityCallback = void (*)(const char* message);

RegisterCallback(UnityCallback callback);
void TriggerEvent(const std::string& message); 