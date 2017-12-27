#include "mogentitytreewidget.h"
#include <QDropEvent>

MogEntityTreeWidget::MogEntityTreeWidget(QWidget *parent) : QTreeWidget(parent)
{
}

void MogEntityTreeWidget::dropEvent(QDropEvent *event)
{
    auto mime = event->mimeData();
    auto source = event->source();

    QModelIndex dragIndex = this->selectedIndexes().at(0);
    QModelIndex dropIndex = indexAt(event->pos());

    if (!dropIndex.isValid()) {
        return;
    }

    QTreeWidgetItem *dragItem = itemFromIndex(dragIndex);
    QTreeWidgetItem *dropItem = itemFromIndex(dropIndex);

    auto dragParent = dragItem->parent();

    dragParent->removeChild(dragItem);
    for (auto item : this->selectedItems()) {
        item->setSelected(false);
    }

    auto name = dragItem->text(0).toStdString();
    auto fromParentName = dragParent->text(0).toStdString();
    QTreeWidgetItem *toParent;
    std::string toParentName;

    if (dropItem->text(1) == "Group") {
        dropItem->addChild(dragItem);
        dropItem->setExpanded(true);
        dragItem->setSelected(true);
        toParent = dropItem;
        toParentName = dropItem->text(0).toStdString();

    } else {
        dropItem->parent()->insertChild(dropItem->parent()->indexOfChild(dropItem), dragItem);
        dragItem->setSelected(true);
        toParent = dropItem->parent();
        toParentName = dropItem->parent()->text(0).toStdString();
    }

    if (this->treeItemMoved) {
        this->treeItemMoved(name, fromParentName, toParentName);
    }

    std::vector<std::string> names;
    for (int i = 0; i < toParent->childCount(); i++) {
        auto item = toParent->child(i);
        names.emplace_back(item->text(0).toStdString());
    }
    if (this->treeItemSorted) {
        this->treeItemSorted(toParentName, names);
    }
}

void MogEntityTreeWidget::keyPressEvent(QKeyEvent *event)
{
    if (!event->type() == QKeyEvent::Type::KeyPress) return;
    if (event->key() != Qt::Key_Delete && event->key() != Qt::Key_Backspace) return;
    if (this->selectedItems().length() == 0) return;
    auto item = this->selectedItems().at(0);
    if (item->text(0) == "root") return;
    std::string name = item->text(0).toStdString();
    item->parent()->removeChild(item);
    if (this->treeItemRemoved) {
        this->treeItemRemoved(name);
    }
}

void MogEntityTreeWidget::setTreeItemMoved(std::function<void(std::string, std::string, std::string)> treeItemMoved)
{
    this->treeItemMoved = treeItemMoved;
}

void MogEntityTreeWidget::setTreeItemSorted(std::function<void(std::string, std::vector<std::string>)> treeItemSorted)
{
    this->treeItemSorted = treeItemSorted;
}

void MogEntityTreeWidget::setTreeItemRemoved(std::function<void(std::string)> treeItemRemoved)
{
    this->treeItemRemoved = treeItemRemoved;
}
