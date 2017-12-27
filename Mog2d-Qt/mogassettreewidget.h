#ifndef MOGASSETTREEWIDGET_H
#define MOGASSETTREEWIDGET_H

#include <QTreeWidget>

class MogAssetTreeWidget : public QTreeWidget
{
public:
    explicit MogAssetTreeWidget(QWidget *parent = Q_NULLPTR);
    void setTreeItemDoubleClicked(std::function<void(std::string path)> treeItemDoubleClicked);

protected:
    virtual void mouseDoubleClickEvent(QMouseEvent * event) override;

private:
    std::function<void(std::string path)> treeItemDoubleClicked;
};

#endif // MOGASSETTREEWIDGET_H
