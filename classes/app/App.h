#ifndef App_h
#define App_h

#include "mog/mog.h"
#include "platform.h"
#include "origin.h"

namespace mog {
    class Scene;
    class App : public AppBase {
    public:
        void onLoad() override;

        void saveUI(std::string filepath);
        void createEntity(EntityType entityType, std::string name, std::string parentName);
        void removeEntity(std::string name);
        void setName(std::string name, std::string newName);
        void setTag(std::string name, std::string tag);
        void setPositionX(std::string name, float x);
        void setPositionY(std::string name, float y);
        void setWidth(std::string name, float width, bool isRatio = false);
        void setHeight(std::string name, float height, bool isRatio = false);
        void setScaleX(std::string name, float x);
        void setScaleY(std::string name, float y);
        void setAnchorX(std::string name, float x);
        void setAnchorY(std::string name, float y);
        void setOriginX(std::string name, float x);
        void setOriginY(std::string name, float y);
        void setColor(std::string name, const Color &color);

        void replaceRoundedRectangle(std::string name, float cornerRadius);
        void replaceCircle(std::string name, float radius);
        void replaceLabel(std::string name, std::string text, float fontSize, std::string fontFilename, float height);
        void replaceSprite(std::string name, std::string filepath, const Rect &rect);
        void replaceSlice9Sprite(std::string name, std::string filepath, const Rect &rect, Rect &centerRect);
        void replaceSpriteSheet(std::string name, std::string filepath, const Rect &rect, Size &frameSize, int frameCount, int margin);
        void replaceEntity(const std::shared_ptr<mog::Entity> &oldEntity, const std::shared_ptr<mog::Entity> &newEntity, bool resetSize = false);

        mog::Density getDensity(std::string filepath);

        mog::Point getPosition(std::string name);
        mog::Size getSize(std::string name);
        mog::Point getScale(std::string name);
        mog::Point getAnchor(std::string name);
        mog::Point getOrigin(std::string name);
        mog::Color getColor(std::string name);
        std::string getSpriteFilename(std::string name);
        mog::Rect getSpriteRect(std::string name);
        std::string getLabelText(std::string name);
        float getLabelFontSize(std::string name);
        std::string getLabelFontFilename(std::string name);
        float getLabelHeight(std::string name);
        mog::Rect getSlice9SpriteCenterRect(std::string name);

    private:
        shared_ptr<mog::Scene> mainScene;
    };
}

#endif /* App_h */
