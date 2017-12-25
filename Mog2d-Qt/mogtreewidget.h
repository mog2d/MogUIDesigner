#ifndef MOGTREEWIDGET_H
#define MOGTREEWIDGET_H

#include <QTreeWidget>

class MogTreeWidget : public QTreeWidget
{
public:
    explicit MogTreeWidget(QWidget *parent = Q_NULLPTR);
    void setTreeItemMoved(std::function<void(std::string, std::string, std::string)> treeItemMoved);
    void setTreeItemSorted(std::function<void(std::string, std::vector<std::string>)> treeItemSorted);

protected:
    virtual void dropEvent(QDropEvent *event) override;

private:
    std::function<void(std::string, std::string, std::string)> treeItemMoved;
    std::function<void(std::string, std::vector<std::string>)> treeItemSorted;
};

#endif // MOGTREEWIDGET_H
