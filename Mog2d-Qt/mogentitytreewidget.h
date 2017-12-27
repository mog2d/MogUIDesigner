#ifndef MOGTREEWIDGET_H
#define MOGTREEWIDGET_H

#include <QTreeWidget>

class MogEntityTreeWidget : public QTreeWidget
{
public:
    explicit MogEntityTreeWidget(QWidget *parent = Q_NULLPTR);
    void setTreeItemMoved(std::function<void(std::string, std::string, std::string)> treeItemMoved);
    void setTreeItemSorted(std::function<void(std::string, std::vector<std::string>)> treeItemSorted);
    void setTreeItemRemoved(std::function<void(std::string)> treeItemRemoved);

protected:
    virtual void dropEvent(QDropEvent *event) override;
    virtual void keyPressEvent(QKeyEvent *event) override;

private:
    std::function<void(std::string, std::string, std::string)> treeItemMoved;
    std::function<void(std::string, std::vector<std::string>)> treeItemSorted;
    std::function<void(std::string)> treeItemRemoved;
};

#endif // MOGTREEWIDGET_H
