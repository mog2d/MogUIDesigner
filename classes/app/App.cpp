#include "app/App.h"
#include "app/MainScene.h"
#include "mog/core/MogUILoader.h"
#include "mog/core/DataStore.h"

using namespace mog;

void App::onLoad() {
    this->mainScene = make_shared<MainScene>();
    this->loadScene(this->mainScene);
}

void App::saveUI(std::string filepath) {
    auto root = this->mainScene->getRootGroup()->findChildByName("root");
    auto uiDict = MogUILoader::serialize(root);
    DataStore::serialize(filepath, uiDict);
}

void App::createEntity(EntityType entityType, std::string name, std::string parentName) {
    shared_ptr<Group> group = nullptr;
    if (parentName.length() == 0) {
        group = this->mainScene->getRootGroup();
    } else {
        group = static_pointer_cast<mog::Group>(this->mainScene->getRootGroup()->findChildByName(parentName));
    }
    shared_ptr<Entity> entity = nullptr;
    switch (entityType) {
    case EntityType::Rectangle:
        entity = Rectangle::create(Size(100, 100));
        break;

    case EntityType::RoundedRectangle:
        entity = RoundedRectangle::create(Size(100, 100), 10);
        break;

    case EntityType::Circle:
        entity = Circle::create(50);
        break;

    case EntityType::Label:
        entity = Label::create("Label", 30);
        break;

    case EntityType::Sprite:
        entity = Sprite::create("");
        entity->setSize(100, 100);
        break;

    case EntityType::Slice9Sprite: {
        auto sprite = Sprite::create("");
        sprite->setSize(100, 100);
        entity = Slice9Sprite::create(sprite, Rect(0, 0, 0, 0));
        break;
    }

    case EntityType::SpriteSheet: {
        auto sprite = Sprite::create("");
        sprite->setSize(100, 100);
        entity = SpriteSheet::create(sprite, Size(100, 100));
        break;
    }

    case EntityType::Group:
        entity = Group::create();
        break;
    }
    if (entity) {
        entity->setName(name);
        group->add(entity);
    }
}

void App::removeEntity(std::string name)
{
    if (auto entity = this->mainScene->getRootGroup()->findChildByName(name)) {
        entity->removeFromParent();
    }
}

void App::setName(std::string name, std::string newName) {
    auto entity = this->mainScene->getRootGroup()->findChildByName(name);
    if (entity) {
        entity->setName(newName);
    }
}

void App::setTag(std::string name, std::string tag) {
    auto entity = this->mainScene->getRootGroup()->findChildByName(name);
    if (entity) {
        entity->setTag(tag);
    }
}

void App::setPositionX(std::string name, float x) {
    auto entity = this->mainScene->getRootGroup()->findChildByName(name);
    if (entity) {
        entity->setPositionX(x);
    }
}

void App::setPositionY(std::string name, float y) {
    auto entity = this->mainScene->getRootGroup()->findChildByName(name);
    if (entity) {
        entity->setPositionY(y);
    }
}

void App::setWidth(std::string name, float width, bool isRatio) {
    auto entity = this->mainScene->getRootGroup()->findChildByName(name);
    if (entity) {
        entity->setWidth(width, isRatio);
    }
}

void App::setHeight(std::string name, float height, bool isRatio) {
    auto entity = this->mainScene->getRootGroup()->findChildByName(name);
    if (entity) {
        entity->setHeight(height, isRatio);
    }
}

void App::setScaleX(std::string name, float x) {
    auto entity = this->mainScene->getRootGroup()->findChildByName(name);
    if (entity) {
        entity->setScaleX(x);
    }
}

void App::setScaleY(std::string name, float y) {
    auto entity = this->mainScene->getRootGroup()->findChildByName(name);
    if (entity) {
        entity->setScaleY(y);
    }
}

void App::setAnchorX(std::string name, float x) {
    auto entity = this->mainScene->getRootGroup()->findChildByName(name);
    if (entity) {
        entity->setAnchorX(x);
    }
}

void App::setAnchorY(std::string name, float y) {
    auto entity = this->mainScene->getRootGroup()->findChildByName(name);
    if (entity) {
        entity->setAnchorY(y);
    }
}

void App::setOriginX(std::string name, float x) {
    auto entity = this->mainScene->getRootGroup()->findChildByName(name);
    if (entity) {
        entity->setOriginX(x);
    }
}

void App::setOriginY(std::string name, float y) {
    auto entity = this->mainScene->getRootGroup()->findChildByName(name);
    if (entity) {
        entity->setOriginY(y);
    }
}

void App::setColor(std::string name, const Color &color) {
    auto entity = this->mainScene->getRootGroup()->findChildByName(name);
    if (entity) {
        entity->setColor(color);
    }
}

mog::Point App::getPosition(std::string name) {
    auto entity = this->mainScene->getRootGroup()->findChildByName(name);
    if (entity) {
        return entity->getPosition();
    }
    return Point::zero;
}

mog::Size App::getSize(std::string name) {
    auto entity = this->mainScene->getRootGroup()->findChildByName(name);
    if (entity) {
        return entity->getSizeValue();
    }
    return Size::zero;
}

bool App::isRatioWidth(std::string name) {
    auto entity = this->mainScene->getRootGroup()->findChildByName(name);
    if (entity) {
        return entity->isRatioWidth();
    }
    return false;
}

bool App::isRatioHeight(std::string name) {
    auto entity = this->mainScene->getRootGroup()->findChildByName(name);
    if (entity) {
        return entity->isRatioHeight();
    }
    return false;
}

mog::Point App::getScale(std::string name) {
    auto entity = this->mainScene->getRootGroup()->findChildByName(name);
    if (entity) {
        return entity->getScale();
    }
    return Point::zero;
}

mog::Point App::getAnchor(std::string name) {
    auto entity = this->mainScene->getRootGroup()->findChildByName(name);
    if (entity) {
        return entity->getAnchor();
    }
    return Point::zero;
}

mog::Point App::getOrigin(std::string name) {
    auto entity = this->mainScene->getRootGroup()->findChildByName(name);
    if (entity) {
        return entity->getOrigin();
    }
    return Point::zero;
}

mog::Color App::getColor(std::string name) {
    auto entity = this->mainScene->getRootGroup()->findChildByName(name);
    if (entity) {
        return entity->getColor();
    }
    return Color::white;
}

std::string App::getSpriteFilename(std::string name) {
    auto entity = this->mainScene->getRootGroup()->findChildByName(name);
    auto sprite = static_pointer_cast<Sprite>(entity);
    return sprite->getFilename();
}

mog::Rect App::getSpriteRect(std::string name) {
    auto entity = this->mainScene->getRootGroup()->findChildByName(name);
    auto sprite = static_pointer_cast<Sprite>(entity);
    if (sprite) {
        return sprite->getRect();
    }
    return Rect::zero;
}

std::string App::getLabelText(std::string name) {
    auto entity = this->mainScene->getRootGroup()->findChildByName(name);
    auto label = static_pointer_cast<Label>(entity);
    if (label) {
        return label->getText();
    }
    return "";
}

float App::getLabelFontSize(std::string name) {
    auto entity = this->mainScene->getRootGroup()->findChildByName(name);
    auto label = static_pointer_cast<Label>(entity);
    if (label) {
        return label->getFontSize();
    }
    return 0;
}

std::string App::getLabelFontFilename(std::string name) {
    auto entity = this->mainScene->getRootGroup()->findChildByName(name);
    auto label = static_pointer_cast<Label>(entity);
    if (label) {
        return label->getFontFilename();
    }
    return "";
}

float App::getLabelHeight(std::string name) {
    auto entity = this->mainScene->getRootGroup()->findChildByName(name);
    auto label = static_pointer_cast<Label>(entity);
    if (label) {
        return label->getFontHeight();
    }
    return 0;
}

mog::Rect App::getSlice9SpriteCenterRect(std::string name) {
    auto entity = this->mainScene->getRootGroup()->findChildByName(name);
    auto slice9Sprite = static_pointer_cast<Slice9Sprite>(entity);
    if (slice9Sprite) {
        return slice9Sprite->getCenterRect();
    }
    return mog::Rect::zero;
}

mog::Size App::getSpriteSheetFrameSize(std::string name) {
    auto entity = this->mainScene->getRootGroup()->findChildByName(name);
    auto spriteSheet = static_pointer_cast<SpriteSheet>(entity);
    if (spriteSheet) {
        return spriteSheet->getFrameSize();
    }
    return mog::Size::zero;
}

unsigned int App::getSpriteSheetFrameCount(std::string name) {
    auto entity = this->mainScene->getRootGroup()->findChildByName(name);
    auto spriteSheet = static_pointer_cast<SpriteSheet>(entity);
    if (spriteSheet) {
        return spriteSheet->getFrameCount();
    }
    return 0;
}

unsigned int App::getSpriteSheetMargin(std::string name) {
    auto entity = this->mainScene->getRootGroup()->findChildByName(name);
    auto spriteSheet = static_pointer_cast<SpriteSheet>(entity);
    if (spriteSheet) {
        return spriteSheet->getMargin();
    }
    return 0;
}

void App::replaceRoundedRectangle(std::string name, float cornerRadius) {
    auto entity = this->mainScene->getRootGroup()->findChildByName(name);
    auto newRRectangle = RoundedRectangle::create(entity->getSize(), cornerRadius);
    this->replaceEntity(entity, newRRectangle);
}

void App::replaceCircle(std::string name, float radius) {
    auto entity = this->mainScene->getRootGroup()->findChildByName(name);
    auto newCircle = Circle::create(radius);
    this->replaceEntity(entity, newCircle);
}

void App::replaceLabel(std::string name, std::string text, float fontSize, std::string fontFilename, float height) {
    auto entity = this->mainScene->getRootGroup()->findChildByName(name);
    auto newLabel = Label::create(text, fontSize, fontFilename, height);
    this->replaceEntity(entity, newLabel);
}

void App::replaceSprite(std::string name, std::string filepath, const Rect &rect) {
    bool resetSize = false;
    Rect _rect = rect;
    auto entity = this->mainScene->getRootGroup()->findChildByName(name);
    if (filepath != (static_pointer_cast<Sprite>(entity))->getFilename()) {
        resetSize = true;
        _rect = Rect::zero;
    }
    auto newSprite = Sprite::createWithFilePath(filepath, _rect, this->getDensity(filepath));
    this->replaceEntity(entity, newSprite, resetSize);
}

void App::replaceSlice9Sprite(std::string name, std::string filepath, const Rect &rect, Rect &centerRect) {
    bool resetSize = false;
    Rect _rect = rect;
    auto entity = this->mainScene->getRootGroup()->findChildByName(name);
    if (filepath != (static_pointer_cast<Sprite>(entity))->getFilename()) {
        resetSize = true;
        _rect = Rect::zero;
    }
    auto newSprite = Sprite::createWithFilePath(filepath, _rect, this->getDensity(filepath));
    auto newSlice9Sprite = Slice9Sprite::create(newSprite, centerRect);
    this->replaceEntity(entity, newSlice9Sprite, resetSize);
}

void App::replaceSpriteSheet(std::string name, std::string filepath, const Rect &rect, Size &frameSize, int frameCount, int margin) {
    auto entity = this->mainScene->getRootGroup()->findChildByName(name);
    bool resetSize = false;
    Rect _rect = rect;
    Size _frameSize = frameSize;
    if (filepath != (static_pointer_cast<Sprite>(entity))->getFilename()) {
        resetSize = true;
        _rect = Rect::zero;
        _frameSize = Size::zero;
    }
    auto newSprite = Sprite::createWithFilePath(filepath, _rect, this->getDensity(filepath));
    if (resetSize) {
        _frameSize = newSprite->getSizeValue();
    }
    auto newSpriteSheet = SpriteSheet::create(newSprite, _frameSize, frameCount, margin);
    this->replaceEntity(entity, newSpriteSheet, resetSize);
}

void App::replaceEntity(const std::shared_ptr<mog::Entity> &oldEntity, const std::shared_ptr<mog::Entity> &newEntity, bool resetSize) {
    if (!oldEntity) return;

    newEntity->setName(oldEntity->getName());
    newEntity->setPosition(oldEntity->getPosition());
    newEntity->setAnchor(oldEntity->getAnchor());
    newEntity->setOrigin(oldEntity->getOrigin());
    newEntity->setColor(oldEntity->getColor());
    if (!resetSize) {
        newEntity->setWidth(oldEntity->getWidth(), oldEntity->isRatioWidth());
        newEntity->setHeight(oldEntity->getHeight(), oldEntity->isRatioHeight());
    }

    auto group = oldEntity->getGroup();
    auto childEntities = group->getChildEntities();
    bool finded = false;
    std::vector<shared_ptr<Entity>> backup;
    for (auto &childEntity : childEntities) {
        if (finded) {
            backup.emplace_back(childEntity);
            childEntity->removeFromParent();
        } else if (childEntity == oldEntity) {
            finded = true;
            oldEntity->removeFromParent();
        }
    }
    group->add(newEntity);
    for (auto &entity : backup) {
        group->add(entity);
    }
}

mog::Density App::getDensity(std::string filepath) {
    if (filepath.find("@3x") != std::string::npos) {
        return Density::x3_0;
    } else if (filepath.find("@2x") != std::string::npos) {
        return Density::x2_0;
    } else if (filepath.find("@1_5x") != std::string::npos) {
        return Density::x1_5;
    }
    return Density::x1_0;
}

void App::startAnimationSpriteSheet(std::string name) {
    auto entity = this->mainScene->getRootGroup()->findChildByName(name);
    auto spriteSheet = static_pointer_cast<SpriteSheet>(entity);
    if (spriteSheet) {
        spriteSheet->startAnimation(1.0f);
    }
}

void App::stopAnimationSpriteSheet(std::string name) {
    auto entity = this->mainScene->getRootGroup()->findChildByName(name);
    auto spriteSheet = static_pointer_cast<SpriteSheet>(entity);
    if (spriteSheet) {
        spriteSheet->stopAnimation();
    }
}

void App::setSpriteSheetAnimationFinish(std::string name, function<void()> onFinishAnimation) {
    auto entity = this->mainScene->getRootGroup()->findChildByName(name);
    auto spriteSheet = static_pointer_cast<SpriteSheet>(entity);
    if (spriteSheet) {
        spriteSheet->setOnFinishEvent([onFinishAnimation](const shared_ptr<SpriteSheet> &spriteSheet) {
            onFinishAnimation();
        });
    }
}

void App::resetAnimationSpriteSheet(std::string name) {
    auto entity = this->mainScene->getRootGroup()->findChildByName(name);
    auto spriteSheet = static_pointer_cast<SpriteSheet>(entity);
    if (spriteSheet) {
        spriteSheet->stopAnimation();
        spriteSheet->selectFrame(0);
    }
}
