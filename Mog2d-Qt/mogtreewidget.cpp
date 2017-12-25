#include "mogtreewidget.h"
#include <QDropEvent>

MogTreeWidget::MogTreeWidget(QWidget *parent) : QTreeWidget(parent)
{
}

void MogTreeWidget::dropEvent(QDropEvent *event)
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
    auto fromParent = dragParent->text(0).toStdString();
    std::string toParent;

    if (dropItem->text(1) == "Group") {
        dropItem->addChild(dragItem);
        dropItem->setExpanded(true);
        dragItem->setSelected(true);
        toParent = dropItem->text(0).toStdString();

    } else {
        dropItem->parent()->insertChild(dropItem->parent()->indexOfChild(dropItem), dragItem);
        dragItem->setSelected(true);
        toParent = dropItem->parent()->text(0).toStdString();
    }

    if (this->treeItemMoved) {
        this->treeItemMoved(name, fromParent, toParent);
    }

    std::vector<std::string> names;
    for (int i = 0; i < dragParent->childCount(); i++) {
        auto item = dragParent->child(i);
        names.emplace_back(item->text(0).toStdString());
    }
    this->treeItemSorted(toParent, names);
}

void MogTreeWidget::setTreeItemMoved(std::function<void(std::string, std::string, std::string)> treeItemMoved)
{
    this->treeItemMoved = treeItemMoved;
}

void MogTreeWidget::setTreeItemSorted(std::function<void(std::string, std::vector<std::string>)> treeItemSorted)
{
    this->treeItemSorted = treeItemSorted;
}
