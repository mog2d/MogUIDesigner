#include "mog/Constants.h"
#include "mog/base/Group.h"
#include "mog/core/Engine.h"
#include <algorithm>

using namespace mog;

shared_ptr<Group> Group::create() {
    return shared_ptr<Group>(new Group());
}

Group::Group() {
    this->sortOrderDirty = true;
}

void Group::updateFrame(const shared_ptr<Engine> &engine, float delta) {
    this->screenScale = engine->getScreenScale();
    this->updatePositionAndSize();
    this->extractEvent(engine, delta);

    auto childEntitiesToDraw = this->getSortedChildEntitiesToDraw();
    for (const auto &entity : childEntitiesToDraw) {
        if ((this->dirtyFlag & DIRTY_SIZE) == DIRTY_SIZE) {
            entity->dirtyFlag |= (DIRTY_POSITION | DIRTY_SIZE);
        }
        entity->updateFrame(engine, delta);
    }
    
    this->onUpdate(delta);
    this->dirtyFlag = 0;
}

void Group::drawFrame(float delta) {
    if (!this->visible) return;
    
    auto childEntitiesToDraw = this->getSortedChildEntitiesToDraw();
    if (this->reRenderFlag > 0 && (this->reRenderFlag & RERENDER_COLOR) == RERENDER_COLOR) {
        this->setReRenderFlagToChild(RERENDER_COLOR);
    }
    
    this->renderer->pushMatrix();
    this->renderer->pushColor();
    this->renderer->applyTransform(this->transform, this->screenScale);
    
    for (const auto &entity : childEntitiesToDraw) {
        entity->drawFrame(delta);
    }
    
    this->renderer->popColor();
    this->renderer->popMatrix();
    
    this->reRenderFlag = 0;
}

void Group::getVerticesNum(int *num) {
    if (!this->visible) return;
    auto childEntitiesToDraw = this->getSortedChildEntitiesToDraw();
    for (auto &entity : childEntitiesToDraw) {
        entity->getVerticesNum(num);
    }
}

void Group::getIndiciesNum(int *num) {
    if (!this->visible) return;
    auto childEntitiesToDraw = this->getSortedChildEntitiesToDraw();
    for (auto &entity : childEntitiesToDraw) {
        entity->getIndiciesNum(num);
    }
}

void Group::bindVertices(float *vertices, int *idx, bool bakeTransform) {
    if (!this->visible) return;
    if (bakeTransform) {
        this->renderer->pushMatrix();
        this->renderer->applyTransform(this->transform, this->screenScale, false);
    }
    
    auto childEntitiesToDraw = this->getSortedChildEntitiesToDraw();
    for (auto &entity : childEntitiesToDraw) {
        entity->bindVertices(vertices, idx, true);
    }
    
    if (bakeTransform) {
        this->renderer->popMatrix();
    }
}

void Group::bindIndices(short *indices, int *idx, int start) {
    if (!this->visible) return;
    auto childEntitiesToDraw = this->getSortedChildEntitiesToDraw();
    for (auto &entity : childEntitiesToDraw) {
        entity->bindIndices(indices, idx, start);
        int verticesNum = 0;
        entity->getVerticesNum(&verticesNum);
        start += verticesNum;
    }
}

void Group::bindVertexTexCoords(const shared_ptr<TextureAtlas> &textureAtlas,
                                float *vertexTexCoords, int *idx, float x, float y, float w, float h) {
    if (!this->visible) return;
    auto childEntitiesToDraw = this->getSortedChildEntitiesToDraw();
    for (auto &entity : childEntitiesToDraw) {
        auto g = dynamic_cast<Group *>(entity.get());
        if (g) {
            g->bindVertexTexCoords(textureAtlas, vertexTexCoords, idx, x, y, w, h);
            
        } else {
            auto cell = textureAtlas->getCell(entity->getTexture());
            entity->bindVertexTexCoords(vertexTexCoords, idx,
                                        (float)cell->x / textureAtlas->width,
                                        (float)cell->y / textureAtlas->height,
                                        (float)cell->width / textureAtlas->width,
                                        (float)cell->height / textureAtlas->height);
        }
    }
}

void Group::bindVertexColors(float *vertexColors, int *idx, const Color &parentColor) {
    if (!this->visible) return;
    Color color = Color(parentColor.r * this->transform->color.r,
                        parentColor.g * this->transform->color.g,
                        parentColor.b * this->transform->color.b,
                        parentColor.a * this->transform->color.a);
    auto childEntitiesToDraw = this->getSortedChildEntitiesToDraw();
    for (auto &entity : childEntitiesToDraw) {
        entity->bindVertexColors(vertexColors, idx, color);
    }
}

void Group::bindVertexSub() {
    if (!this->visible) return;
    auto childEntitiesToDraw = this->getSortedChildEntitiesToDraw();
    for (auto &entity : childEntitiesToDraw) {
        entity->bindVertexSub();
    }
}

void Group::updateMatrix() {
    this->renderer->pushMatrix();
    this->renderer->applyTransform(this->transform, this->screenScale, false);
    for (auto &entity : childEntitiesToDraw) {
        entity->updateMatrix();
    }
    this->renderer->popMatrix();
}

void Group::sortChildEntitiesToDraw() {
    this->childEntitiesToDraw.clear();
    this->childEntitiesToDraw.reserve(this->childEntities.size());
    unordered_map<uintptr_t, unsigned int> entitiyIndexes;
    entitiyIndexes.reserve(this->childEntities.size());
    for (int i = 0; i < this->childEntities.size(); i++) {
        this->childEntitiesToDraw.emplace_back(this->childEntities[i]);
        entitiyIndexes[(uintptr_t)this->childEntities[i].get()] = i;
    }
    sort(this->childEntitiesToDraw.begin(), this->childEntitiesToDraw.end(),
         [&entitiyIndexes](const shared_ptr<Entity> &e1, const shared_ptr<Entity> &e2) {
             if (e1->getZIndex() == e2->getZIndex()) {
                 return entitiyIndexes[(uintptr_t)e1.get()] < entitiyIndexes[(uintptr_t)e2.get()];
             }
             return e1->getZIndex() < e2->getZIndex();
         });
    
    this->sortOrderDirty = false;
}

void Group::add(const shared_ptr<Entity> &entity) {
    this->childEntities.emplace_back(entity);
    if (this->entityIdSet.count((uintptr_t)entity.get()) > 0) return;
    this->entityIdSet.insert((uintptr_t)entity.get());
    
    entity->setGroup(static_pointer_cast<Group>(shared_from_this()));
    this->sortOrderDirty = true;
    this->setReRenderFlag(RERENDER_ALL);
}

void Group::remove(const shared_ptr<Entity> &entity) {
    this->childEntities.erase(std::remove(this->childEntities.begin(), this->childEntities.end(), entity), this->childEntities.end());
    this->sortOrderDirty = true;
    this->setReRenderFlag(RERENDER_ALL);
    this->entityIdSet.erase((uintptr_t)entity.get());
}

void Group::removeAll() {
    this->childEntities.clear();
    this->childEntitiesToDraw.clear();
    this->entityIdSet.clear();
    this->sortOrderDirty = true;
    this->setReRenderFlag(RERENDER_ALL);
}

vector<shared_ptr<Entity>> Group::getChildEntities() {
    return this->childEntities;
}

vector<shared_ptr<Entity>> Group::getSortedChildEntitiesToDraw() {
    if (this->sortOrderDirty) {
        this->sortChildEntitiesToDraw();
    }
    return this->childEntitiesToDraw;
}

shared_ptr<Entity> Group::findChildByName(string name, bool recursive) {
    for (const auto &entity : this->childEntities) {
        if (entity->getName() == name) return entity;
        if (recursive) {
            if (entity->getEntityType() == EntityType::Group || entity->getEntityType() == EntityType::BatchingGroup) {
                if (auto e = static_pointer_cast<Group>(entity)->findChildByName(name)) {
                    return e;
                }
            }
        }
    }
    return nullptr;
}

vector<shared_ptr<Entity>> Group::findChildrenByTag(string tag, bool recursive) {
    vector<shared_ptr<Entity>> vec;
    for (const auto &entity : this->childEntities) {
        if (entity->getName() == name) {
            vec.emplace_back(entity);
        };
        if (recursive) {
            if (entity->getEntityType() == EntityType::Group || entity->getEntityType() == EntityType::BatchingGroup) {
                if (auto e = static_pointer_cast<Group>(entity)->findChildByName(name)) {
                    vec.emplace_back(e);
                }
            }
        }
    }
    return vec;
}

void Group::addTextureTo(const shared_ptr<TextureAtlas> &textureAtlas) {
    auto childEntitiesToDraw = this->getSortedChildEntitiesToDraw();
    for (const auto &entity : childEntitiesToDraw) {
        entity->addTextureTo(textureAtlas);
    }
}

void Group::enableSortOrderDirty() {
    this->sortOrderDirty = true;
    if (auto group = this->group.lock()) {
        group->enableSortOrderDirty();
    }
    this->setReRenderFlag(RERENDER_ALL);
}

void Group::setReRenderFlagToChild(unsigned char flag) {
    auto childEntitiesToDraw = this->getSortedChildEntitiesToDraw();
    for (const auto &entity : childEntitiesToDraw) {
        entity->setReRenderFlagToChild(flag);
    }
    this->reRenderFlag |= flag;
}

shared_ptr<Group> Group::clone() {
    auto entity = this->cloneEntity();
    return static_pointer_cast<Group>(entity);
}

shared_ptr<Entity> Group::cloneEntity() {
    auto group = shared_ptr<Group>(new Group());
    group->copyFrom(shared_from_this());
    return group;
}

void Group::copyFrom(const shared_ptr<Entity> &src) {
    Entity::copyFrom(src);
    auto srcGroup = static_pointer_cast<Group>(src);
    auto srcChildentities = srcGroup->getChildEntities();
    for (auto &entity : srcChildentities) {
        auto cloneEntity = entity->cloneEntity();
        this->add(cloneEntity);
    }
}

EntityType Group::getEntityType() {
    return EntityType::Group;
}

