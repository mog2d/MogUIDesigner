#ifndef MOGASSETTREEWIDGET_H
#define MOGASSETTREEWIDGET_H

#include <QTreeWidget>

class MogAssetTreeWidget : public QTreeWidget
{
public:
    explicit MogAssetTreeWidget(QWidget *parent = Q_NULLPTR);

protected:
    virtual void mouseDoubleClickEvent(QMouseEvent * event) override;

private:

};

#endif // MOGASSETTREEWIDGET_H
