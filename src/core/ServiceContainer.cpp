// ServiceContainer.cpp - Simple dependency injection container implementation
#include "ServiceContainer.h"

namespace BabyMonitor {

ServiceContainer& ServiceContainer::getInstance() {
    static ServiceContainer instance;
    return instance;
}

} // namespace BabyMonitor
