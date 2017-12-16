#include "MainScene.h"

using namespace mog;

void MainScene::onLoad() {
    auto root = Group::create();
    root->setName("root");
    root->setSize(this->getRootGroup()->getSize());
    this->add(root);
}
