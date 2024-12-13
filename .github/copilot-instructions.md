# Modern C++ HTTP Server Development Guidelines

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