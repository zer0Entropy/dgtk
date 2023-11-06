//
// Created by zeroc00l on 10/31/23.
//

#ifndef DGTKPROJECT_SYSTEM_HPP
#define DGTKPROJECT_SYSTEM_HPP

enum class SystemID { Display = 0, Resource, Input, Log, TotalNumSystems };

class Game;

class System {
public:
    friend class Game;
    explicit System(SystemID assignID): id(assignID), interrupt(false) {}
    virtual ~System() {}
    SystemID        GetID() const { return id; }
    virtual void    Init() = 0;
    virtual void    Update() = 0;
    virtual void    Shutdown() = 0;
protected:
    void            SetInterrupt() { interrupt = true; }
    SystemID        id;
    bool            interrupt;
};

#endif //DGTKPROJECT_SYSTEM_HPP
