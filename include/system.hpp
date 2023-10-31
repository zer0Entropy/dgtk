//
// Created by zeroc00l on 10/31/23.
//

#ifndef DGTKPROJECT_SYSTEM_HPP
#define DGTKPROJECT_SYSTEM_HPP

enum class SystemID { Display, Resource, Input };

class System {
public:
    explicit System(SystemID assignID): id(assignID) {}
    virtual ~System() {}
    SystemID        GetID() const { return id; }
    virtual void    Init() = 0;
    virtual void    Update() = 0;
    virtual void    Shutdown() = 0;
protected:
    SystemID        id;
};

#endif //DGTKPROJECT_SYSTEM_HPP
