// ServiceContainer.h - Simple dependency injection container
#pragma once

#include <memory>
#include <functional>
#include <unordered_map>
#include <QString>
#include "../interfaces/IComponent.h"
#include "../ErrorHandler.h"

namespace BabyMonitor {

/**
 * Simple service container for dependency injection
 * Follows Dependency Inversion Principle
 */
class ServiceContainer {
public:
    static ServiceContainer& getInstance();
    
    /**
     * Register a service factory
     */
    template<typename Interface>
    void registerService(const QString& name, std::function<std::shared_ptr<Interface>()> factory) {
        factories_[name] = [factory]() -> std::shared_ptr<void> {
            return std::static_pointer_cast<void>(factory());
        };
    }
    
    /**
     * Register a singleton service
     */
    template<typename Interface>
    void registerSingleton(const QString& name, std::shared_ptr<Interface> instance) {
        singletons_[name] = std::static_pointer_cast<void>(instance);
    }
    
    /**
     * Resolve a service by name
     */
    template<typename Interface>
    std::shared_ptr<Interface> resolve(const QString& name) {
        // Check singletons first
        auto singletonIt = singletons_.find(name);
        if (singletonIt != singletons_.end()) {
            return std::static_pointer_cast<Interface>(singletonIt->second);
        }
        
        // Check factories
        auto factoryIt = factories_.find(name);
        if (factoryIt != factories_.end()) {
            auto instance = factoryIt->second();
            return std::static_pointer_cast<Interface>(instance);
        }
        
        return nullptr;
    }
    
    /**
     * Check if service is registered
     */
    bool isRegistered(const QString& name) const {
        return singletons_.find(name) != singletons_.end() ||
               factories_.find(name) != factories_.end();
    }
    
    /**
     * Clear all services
     */
    void clear() {
        singletons_.clear();
        factories_.clear();
    }

private:
    ServiceContainer() = default;
    
    std::unordered_map<QString, std::shared_ptr<void>> singletons_;
    std::unordered_map<QString, std::function<std::shared_ptr<void>()>> factories_;
};

} // namespace BabyMonitor
