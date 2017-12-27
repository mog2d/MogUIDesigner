#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidget>
#include <QCheckBox>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QDir>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <functional>
#include "mog/core/Data.h"
#include "mog/base/Entity.h"
#include "app/App.h"

namespace Ui {
    class MainWindow;
    }

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QApplication &a, QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_comboBox_CreateEntity_currentIndexChanged(int index);
    void saveFile();
    void entitiesItemSelectionChanged();
    void applicationStateChanged(Qt::ApplicationState state);
    void propertiesCellClicked(int row, int column);
    void platformChanged(int index);

private:
    enum class Property {
        Name,
        Tag,
        PositionX,
        PositionY,
        Width,
        IsRatioWidth,
        Height,
        IsRatioHeight,
        ScaleX,
        ScaleY,
        AnchorX,
        AnchorY,
        OriginX,
        OriginY,
        Color,
        // RoundedRectangle
        CornerRadius,
        // Circle
        Radius,
        // Label
        Text,
        FontSize,
        FontFilename,
        FontHeight,
        // Sprite
        Filename,
        RectX,
        RectY,
        RectWidth,
        RectHeight,
        // Slice9Sprite
        CenterRectX,
        CenterRectY,
        CenterRectWidth,
        CenterRectHeight,
        // SpriteSheet
        FrameSizeWidth,
        FrameSizeHeight,
        FrameCount,
        Margin,
        Play,
        Reset,
        // Group
        Batching,
    };

    static const std::unordered_map<std::string, mog::EntityType> entityTypeMap;

    Ui::MainWindow *ui;
    std::set<std::string> nameSet;
    QString projectPath;
    std::map<QString, std::pair<int, int>> propertiesGroups;
    int propertiesGroupStack = 0;
    std::map<Property, int> propertyRows;
    std::set<std::string> assetsSet;
    std::map<std::string, std::string> assetsPathMap;
    bool isOpenProjectDialog = false;
    QIntValidator *intValidator;
    QDoubleValidator *doubleValidator;

    void initProperties();
    void initPropertiesCommon();
    void initPropertiesRoundedRectangle();
    void initPropertiesCircle();
    void initPropertiesLabel();
    void initPropertiesSprite();
    void initPropertiesSlice9Sprite();
    void initPropertiesSpriteSheet();
    void initPropertiesGroup();
    void setPropertyValuesFromEntity();

    void addTreeItemEntity(QTreeWidgetItem *parentItem, std::string parentName);

    QLineEdit *createLineEdit(QString name, Property prop, QValidator *validator, std::function<void()> editingFinished);
    QCheckBox *createCheckBox(QString name, Property prop, std::function<void(bool checked)> clicked);
    QComboBox *createComboBox(QString name, Property prop, QStringList &items, std::function<void(const QString &text)> currentTextChanged);
    QString toRatioValue(QString text);

    QWidget *getWidget(Property prop);
    mog::Dictionary createDictionary(mog::EntityType entityType);
    void setPropertiesCommon(mog::Dictionary &dict);
    void setPropertiesSprite(mog::Dictionary &dict);

    void startPropertiesGroup(QString name);
    int addPropertyItem(QString name, QWidget *widget);
    void endPropertiesGroup(QString name);

    shared_ptr<mog::App> getApp();
    std::string getSelectedName();
    std::string getSelectedEntityType();
    mog::EntityType toEntityType(std::string entityTypeName);
    std::string toString(mog::EntityType entityType);
    QString formatFloat(float f);
    bool openMogProject();
    void initAssets();
    QTreeWidgetItem *findAssetItems(QDir assetsDir);
    bool isImageFile(QFileInfo fileInfo);

    std::string getAssetFilePath(Platform platform, std::string filename);
    std::vector<std::string> getAssetsPlatformDir(Platform platform);
};

#endif // MAINWINDOW_H
