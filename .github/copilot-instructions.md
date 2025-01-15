Please Use Chinese Answer Me! Thinks~

# Modern C++ HTTP Server Development Guidelines

## Chinese Language Requirements
- Code comments must be in Chinese
- Documentation must be in Chinese

## POCO Library Usage Guidelines

### Priority Principle
- When functionality overlaps, prefer POCO library over standard library
- Only consider standard library when POCO doesn't support the feature or has significant performance issues

### POCO Library Priority Scenarios
1. Network Programming
   - Use Poco::Net instead of standard socket libraries
   - Use Poco::Net::HTTPClientSession for HTTP requests
   - Use Poco::Net::ServerSocket for server creation

2. File Operations
   - Use Poco::File instead of standard file operations
   - Use Poco::FileStream instead of std::fstream
   - Use Poco::TemporaryFile for temporary file handling

3. String Processing
   - Use Poco::String instead of std::string operations
   - Use Poco::Format for string formatting

4. Date and Time
   - Use Poco::DateTime instead of std::chrono
   - Use Poco::Timestamp for timestamp handling

5. Multithreading
   - Use Poco::Thread instead of std::thread
   - Use Poco::Mutex instead of std::mutex
   - Use Poco::Event for thread synchronization

6. Configuration Files
   - Use Poco::Util::PropertyFileConfiguration for configuration handling
   - Use Poco::Util::JSONConfiguration for JSON configuration

7. Data Formatting
   - Use Poco::JSON for JSON data handling
   - Use Poco::XML for XML data handling

### Code Example Standards
```cpp
// Recommended POCO Usage
class DataProcessor {
private:
    Poco::Logger& logger_;
    Poco::Net::HTTPClientSession session_;
    Poco::JSON::Object::Ptr config_;

public:
    DataProcessor() : 
        logger_(Poco::Logger::get("DataProcessor")) 
    {
        logger_.information("数据处理器初始化");
    }

    void ProcessData() {
        try {
            // Using POCO file operations
            Poco::File dataFile("data.json");
            if (dataFile.exists()) {
                // Using POCO JSON parsing
                Poco::FileInputStream fis(dataFile.path());
                Poco::JSON::Parser parser;
                config_ = parser.parse(fis).extract<Poco::JSON::Object::Ptr>();
            }
        }
        catch (Poco::Exception& ex) {
            logger_.error("数据处理失败：%s", ex.displayText());
        }
    }
};
```

### Error Handling Standards
- Use POCO's exception system for error handling
- Error messages must be in Chinese
- Maintain reasonable exception granularity
- Ensure exception messages are readable and understandable

## Code Style Guidelines

### 1. File Names
- Use PascalCase
- No underscores
- Examples:
```cpp
NetworkManager.h
AudioDevice.cpp
StreamProcessor.h
ConfigurationManager.cpp
```

### 2. Class Names, Function Names and Enum Type Names
- Use PascalCase
- No underscores
- This applies to:
  * Class names
  * Global functions
  * Member functions (both public and private)
  * Enum type names
- Examples:
```cpp
// Class names
class NetworkManager;
class AudioDevice;

// Function names
void ProcessData();
bool ValidateInput();
int CalculateChecksum();

class AudioDevice {
public:
    void ProcessAudio();          // Public methods use PascalCase
    bool ValidateFormat();
    void SetConfiguration();
    
private:
    void HandleError();           // Private methods also use PascalCase
    bool CheckStatus();
};

// Enum type names
enum class AudioState;
enum class ConnectionState;
```

### 3. Member Variables and Parameters
- Use camelCase
- Private member variables must end with underscore
- Parameters use camelCase without underscore
- Examples:
```cpp
class AudioDevice {
private:
    int sampleRate_;              // Private members end with underscore
    bool isActive_;
    std::string deviceName_;
    std::vector<float> audioBuffer_;
    
public:
    // Parameters use camelCase without underscore
    void ProcessAudio(int bufferSize, float* inputData);
    void Configure(const std::string& configName);
};
```

### 4. Constants, Enum Values and Macros
- Enum type names use PascalCase
- Enum values use UPPER_CASE with underscores
- Constants and macros use UPPER_CASE with underscores
- Examples:
```cpp
const int MAX_BUFFER_SIZE = 1024;
#define NETWORK_MTU_SIZE 1500

enum class AudioState {
    AUDIO_INITIAL,
    AUDIO_PLAYING,
    AUDIO_PAUSED,
    AUDIO_STOPPED
};

enum class ConnectionState {
    CONN_DISCONNECTED,
    CONN_CONNECTING,
    CONN_CONNECTED,
    CONN_ERROR
};
```

### 5. Include Paths
- Use absolute paths
- No relative paths
- Examples:
```cpp
// Correct
#include "Source/Core/AudioDevice.h"
#include "Source/Utils/Logger.h"

// Incorrect
#include "../Core/AudioDevice.h"
#include "../../Utils/Logger.h"
```

### 6. Indentation
- Use 4 spaces for indentation
- No tabs
- Example:
```cpp
class AudioProcessor {
public:
    void ProcessBuffer() {
        if (isActive_) {
            for (int i = 0; i < bufferSize_; i++) {
                // Processing logic
            }
        }
    }
    
private:
    bool isActive_;
    int bufferSize_;
};
```

### 7. Enum Usage
- Prefer enum class over enum
- Follow naming conventions as specified above
- Example:
```cpp
// Preferred
enum class DeviceState {
    DEVICE_NOT_INITIALIZED,
    DEVICE_INITIALIZING,
    DEVICE_RUNNING,
    DEVICE_ERROR
};

// Not preferred
enum OldDeviceState {
    NOT_INITIALIZED,
    INITIALIZING,
    RUNNING,
    ERROR
};
```

### Complete Example

```cpp
// NetworkManager.h
#include "Source/Core/Common.h"
#include "Source/Utils/Logger.h"

const int MAX_RETRY_COUNT = 3;
const char* DEFAULT_HOST = "LOCALHOST";

enum class ConnectionState {
    CONN_DISCONNECTED,
    CONN_CONNECTING,
    CONN_CONNECTED,
    CONN_ERROR
};

enum class DeviceType {
    DEVICE_AUDIO,
    DEVICE_VIDEO,
    DEVICE_NETWORK
};

class NetworkManager {
public:
    void Initialize();
    bool ConnectToServer();
    void ProcessPacket();
    void HandleConnection(const std::string& serverName, int portNumber);
    
private:
    bool isConnected_;
    int retryCount_;
    std::string serverAddress_;
    ConnectionState currentState_;
    DeviceType deviceType_;
    
    void HandleError();
    bool ValidateConnection() {
        if (currentState_ == ConnectionState::CONN_CONNECTED) {
            // Handle connection
            return true;
        }
        return false;
    }
};

// Global function examples
void ProcessGlobalData();
bool ValidateSystemInput();
int CalculateGlobalChecksum();
```

## Project Context
- Modern C++17 server application
- Embedded platform (RK3308, 512MB RAM)
- Network-centric application
- Hardware interaction requirements

## Architecture Principles
- Modular design with clear boundaries
- Interface-based programming
- Dependency injection pattern
- Plugin architecture for extensibility
- Event-driven design where appropriate
- Service-oriented architecture
- Platform abstraction layer
- Hardware abstraction layer

## Modern C++ Guidelines
- Leverage C++17 features appropriately
- Use RAII for resource management
- Prefer value semantics
- Utilize type safety features
- Follow the rule of zero/five
- Use smart pointers for dynamic memory
- Embrace standard library features
- Avoid platform-specific code in core logic

## Code Quality Standards
- Clear and consistent naming conventions
- Single Responsibility Principle
- DRY (Don't Repeat Yourself)
- SOLID principles
- Comprehensive error handling
- Self-documenting code
- Unit test coverage
- Maximum cyclomatic complexity: 10
- Maximum function length: 50 lines
- Maximum file length: 500 lines

## Performance Considerations
- Memory constraints awareness
- Stack vs heap allocation
- Zero-copy operations when possible
- Efficient string handling
- Move semantics optimization
- Cache-friendly data structures
- Lock-free programming where appropriate
- Memory budget per component
- CPU usage monitoring
- Latency requirements tracking

## Network Programming Patterns
- Asynchronous I/O
- Connection pooling
- Buffer management
- Protocol abstraction
- Error recovery mechanisms
- Timeout handling
- Rate limiting
- Network metrics collection
- Connection lifecycle management

## Data Management
- Efficient serialization/deserialization
- State management patterns
- Configuration handling
- Data validation
- Cache strategies
- Transaction safety
- Data integrity verification
- Schema versioning

## Security Best Practices
- Input validation
- Authentication/Authorization
- Secure communication
- Resource protection
- Error message safety
- Rate limiting
- Session management
- Security audit logging
- Vulnerability scanning integration

## System Integration
- Hardware abstraction layer
- Inter-process communication
- System service integration
- External API integration
- Event handling
- Resource sharing
- Platform-specific adaptation layer
- Feature toggles support

## Build System & Dependencies
- CMake best practices
- Dependency management strategy
- Conditional compilation guidelines
- Cross-platform compatibility
- Build configurations
- Package management
- Version compatibility matrix
- Third-party library policies

## Development Tools Integration
- Static code analyzers (e.g., clang-tidy)
- Dynamic analysis tools (e.g., valgrind)
- Code coverage tools
- Performance profilers
- Memory leak detectors
- Documentation generators
- Automated formatting tools
- IDE integration guidelines

## Operational Excellence
- Comprehensive logging
- Performance metrics
- Health monitoring
- Diagnostic capabilities
- Graceful degradation
- Recovery mechanisms
- Telemetry collection
- Performance baselines
- Resource utilization tracking

## Development Workflow
- Version control best practices
- CI/CD integration
- Code review guidelines
- Documentation requirements
- Testing strategy
- Release management
- Branch management strategy
- Semantic versioning
- Change log maintenance

## Testing Requirements
- Unit testing framework
- Mock object patterns
- Integration testing
- Performance testing
- Memory leak detection
- Coverage requirements (minimum 80%)
- Benchmark suite
- Regression testing
- Load testing criteria

## Debug Support
- Debug log levels
- Debug build configurations
- Troubleshooting helpers
- Debug visualization tools
- Core dump analysis
- Remote debugging support
- Performance tracing
- Memory tracking

## Maintenance Guidelines
- Technical debt tracking
- Refactoring criteria
- Deprecation policy
- API versioning
- Breaking changes policy
- Legacy code handling
- Documentation updates
- Upgrade path planning

Remember to:
- Write maintainable and readable code
- Consider resource constraints
- Design for extensibility
- Implement proper error handling
- Document critical decisions
- Consider edge cases
- Optimize for the target platform
- Plan for future maintenance
- Keep security in mind
- Monitor performance metrics