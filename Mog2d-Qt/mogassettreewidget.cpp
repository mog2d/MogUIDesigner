#include "mogassettreewidget.h"

MogAssetTreeWidget::MogAssetTreeWidget(QWidget *parent) : QTreeWidget(parent)
{
}

void MogAssetTreeWidget::mouseDoubleClickEvent(QMouseEvent * event)
{
    QTreeWidget::mouseDoubleClickEvent(event);

    if (this->selectedItems().length() == 0) return;
    auto item = this->selectedItems().at(0);
}
