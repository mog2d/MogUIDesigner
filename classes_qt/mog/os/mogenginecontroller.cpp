#include "mogenginecontroller.h"
#include "mog/Constants.h"
#include "mog/core/Engine.h"
#include "app/App.h"

MogEngineController::MogEngineController() {
    this->app = make_shared<mog::App>();
    this->engine = mog::Engine::create(this->app);
}

void MogEngineController::startEngine() {
    this->engine->startEngine();
}

void MogEngineController::stopEngine() {
    this->engine->stopEngine();
}

void MogEngineController::drawFrame() {
    map<unsigned int, mog::TouchInput> touches;
    this->engine->onDrawFrame(touches);
}

void MogEngineController::resize(float width, float height) {
    this->engine->setDisplaySize(mog::Size(width, height));
    this->engine->setScreenSizeBasedOnHeight(BASE_SCREEN_HEIGHT);
}

std::shared_ptr<mog::AppBase> MogEngineController::getApp() {
    return this->app;
}
