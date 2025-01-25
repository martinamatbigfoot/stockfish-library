// Define a type for the callback
using UnityCallback = void (*)(const char* message);

extern "C" {
    void Initialize(UnityCallback callback);

    void ExecuteCommand(const char* cmd);
}



void RegisterCallback(UnityCallback callback);
void TriggerEvent(const std::string& message); 