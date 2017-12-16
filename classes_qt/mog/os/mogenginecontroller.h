#ifndef MOGENGINECONTROLLER_H
#define MOGENGINECONTROLLER_H

#include <memory>

namespace mog {
    class Engine;
    class AppBase;
}

class MogEngineController {
public:
    std::shared_ptr<mog::AppBase> getApp();
    MogEngineController();

    void startEngine();
    void resize(float width, float height);
    void stopEngine();
    void drawFrame();

private:
    std::shared_ptr<mog::Engine> engine;
    std::shared_ptr<mog::AppBase> app;
};

#endif // MOGENGINECONTROLLER_H
