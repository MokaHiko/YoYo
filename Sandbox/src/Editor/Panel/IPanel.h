#ifndef IPANEL_H
#define IPANEL_H

#pragma once

class Scene;
class IPanel
{
public:
    IPanel();
    virtual ~IPanel();
    
    virtual void Draw(Scene* scene) = 0;
private:
};

#endif