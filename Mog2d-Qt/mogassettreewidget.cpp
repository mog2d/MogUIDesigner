#include "mogassettreewidget.h"

MogAssetTreeWidget::MogAssetTreeWidget(QWidget *parent) : QTreeWidget(parent)
{
}

void MogAssetTreeWidget::setTreeItemDoubleClicked(std::function<void(std::string path)> treeItemDoubleClicked)
{
    this->treeItemDoubleClicked = treeItemDoubleClicked;
}

void MogAssetTreeWidget::mouseDoubleClickEvent(QMouseEvent * event)
{
    QTreeWidget::mouseDoubleClickEvent(event);

    if (!this->treeItemDoubleClicked) return;

    if (this->selectedItems().length() == 0) return;
    QTreeWidgetItem *item = this->selectedItems().at(0);

    std::string path = item->text(0).toStdString();
    while (item = item->parent()) {
        path = item->text(0).toStdString() + "/" + path;
    }

    this->treeItemDoubleClicked(path);
}
