#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "origin.h"
#include "mog/base/Entity.h"
#include "mog/core/MogUILoader.h"
#include <QFileDialog>
#include <QColorDialog>

const std::unordered_map<std::string, mog::EntityType> MainWindow::entityTypeMap = {
    {"Rectangle",           mog::EntityType::Rectangle},
    {"RoundedRectangle",    mog::EntityType::RoundedRectangle},
    {"Circle",              mog::EntityType::Circle},
    {"Label",               mog::EntityType::Label},
    {"Sprite",              mog::EntityType::Sprite},
    {"Slice9Sprite",        mog::EntityType::Slice9Sprite},
    {"SpriteSheet",         mog::EntityType::SpriteSheet},
    {"Group",               mog::EntityType::Group},
    {"BatchingGroup",       mog::EntityType::BatchingGroup},
};


MainWindow::MainWindow(QApplication &a, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->doubleValidator = new QDoubleValidator(this);
    this->intValidator = new QIntValidator(this);
    this->initProperties();

    this->connect(&a, SIGNAL(applicationStateChanged(Qt::ApplicationState)),
                  this, SLOT(applicationStateChanged(Qt::ApplicationState)));

    this->connect(this->ui->treeWidget_Entities, SIGNAL(itemSelectionChanged()),
                  this, SLOT(entitiesItemSelectionChanged()));

    this->connect(this->ui->comboBox_Platform, SIGNAL(currentIndexChanged(int)),
                   this, SLOT(platformChanged(int)));

    this->ui->actionSave->setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton));
    this->connect(this->ui->actionSave, SIGNAL(triggered()), this, SLOT(saveFile()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::saveFile() {
    QString dirPath = QDir(this->projectPath).filePath("assets");
    QString filepath = QFileDialog::getSaveFileName(this, "Save file", dirPath, "*.mogui");
    this->getApp()->saveUI(filepath.toStdString());
    this->initAssets();
}

void MainWindow::propertiesCellClicked(int row, int column)
{
}

void MainWindow::initProperties()
{
    this->initPropertiesCommon();

    this->connect(this->ui->tableWidget_Properties, SIGNAL(cellClicked(int, int)),
                  this, SLOT(propertiesCellClicked(int, int)));
}

QLineEdit *MainWindow::createLineEdit(QString name, Property prop, QValidator *validator, std::function<void()> editingFinished)
{
    auto lineEdit = new QLineEdit(this);
    if (validator) {
        lineEdit->setValidator(validator);
    }
    int row = this->addPropertyItem(name, lineEdit);
    this->propertyRows[prop] = row;

    if (editingFinished) {
        this->connect(lineEdit, &QLineEdit::editingFinished, editingFinished);
    }
}

QCheckBox *MainWindow::createCheckBox(QString name, Property prop, std::function<void(bool checked)> clicked)
{
    auto checkBox = new QCheckBox(this);
    int row = this->addPropertyItem(name, checkBox);
    this->propertyRows[prop] = row;
    this->connect(checkBox, &QCheckBox::clicked, clicked);
    return checkBox;
}

QComboBox *MainWindow::createComboBox(QString name, Property prop, QStringList &items, std::function<void(const QString &text)> currentTextChanged)
{
    QComboBox *comboBox = new QComboBox(this);
    comboBox->addItems(items);
    this->connect(comboBox, &QComboBox::currentTextChanged, currentTextChanged);
    int row = this->addPropertyItem(name, comboBox);
    this->propertyRows[prop] = row;

    return comboBox;
}

QString MainWindow::toRatioValue(QString text)
{
    float f = text.toFloat();
    if (f < 0) f = 0;
    if (f > 1.0f) f = 1.0f;
    return this->formatFloat(f);
}

QWidget *MainWindow::getWidget(Property prop)
{
    int row = this->propertyRows[prop];
    return this->ui->tableWidget_Properties->cellWidget(row, 1);
}

void MainWindow::initPropertiesCommon()
{
    auto doubleValidator = new QDoubleValidator(this);

    this->startPropertiesGroup("Common");

    // Name
    this->createLineEdit("name", Property::Name, nullptr, [this]() {
        auto lineEdit = (QLineEdit *)this->getWidget(Property::Name);
        auto text = lineEdit->text();
        auto name = this->getSelectedName();

        auto items = this->ui->treeWidget_Entities->selectedItems();
        if (items.count() != 1) {
            return;
        }
        auto selectItem = items.at(0);
        if (text.length() == 0 || this->nameSet.count(text.toStdString()) > 0) {
            lineEdit->setText(selectItem->text(0));
            return;
        }
        this->nameSet.erase(selectItem->text(0).toStdString());
        this->nameSet.insert(text.toStdString());
        selectItem->setText(0, text);

        this->getApp()->setName(name, text.toStdString());
    });

    this->createLineEdit("tag", Property::Tag, nullptr, [this]() {
        auto text = ((QLineEdit *)this->getWidget(Property::Tag))->text();
        this->getApp()->setTag(this->getSelectedName(), text.toStdString());
    });

    // Position
    this->startPropertiesGroup("Position");

    this->createLineEdit("x", Property::PositionX, this->doubleValidator, [this]() {
        auto text = ((QLineEdit *)this->getWidget(Property::PositionX))->text();
        this->getApp()->setPositionX(this->getSelectedName(), text.toFloat());
    });

    this->createLineEdit("y", Property::PositionY, this->doubleValidator, [this]() {
        auto text = ((QLineEdit *)this->getWidget(Property::PositionY))->text();
        this->getApp()->setPositionY(this->getSelectedName(), text.toFloat());
    });

    this->endPropertiesGroup("Position");

    // Size
    this->startPropertiesGroup("Size");

    this->createCheckBox("isRatioWidth", Property::IsRatioWidth, [this](bool checked) {
        auto text = ((QLineEdit *)this->getWidget(Property::Width))->text();
        this->getApp()->setWidth(this->getSelectedName(), text.toFloat(), checked);
    });

    this->createLineEdit("width", Property::Width, this->doubleValidator, [this]() {
        auto text = ((QLineEdit *)this->getWidget(Property::Width))->text();
        bool isRatio = ((QCheckBox *)this->getWidget(Property::IsRatioWidth))->isChecked();
        this->getApp()->setWidth(this->getSelectedName(), text.toFloat(), isRatio);
    });

    this->createCheckBox("isRatioHeight", Property::IsRatioHeight, [this](bool checked) {
        auto text = ((QLineEdit *)this->getWidget(Property::Height))->text();
        this->getApp()->setHeight(this->getSelectedName(), text.toFloat(), checked);
    });

    this->createLineEdit("height", Property::Height, this->doubleValidator, [this]() {
        auto text = ((QLineEdit *)this->getWidget(Property::Height))->text();
        bool isRatio = ((QCheckBox *)this->getWidget(Property::IsRatioHeight))->isChecked();
        this->getApp()->setHeight(this->getSelectedName(), text.toFloat(), isRatio);
    });

    this->endPropertiesGroup("Size");

    // Scale
    this->startPropertiesGroup("Scale");

    this->createLineEdit("x", Property::ScaleX, this->doubleValidator, [this]() {
        auto text = ((QLineEdit *)this->getWidget(Property::ScaleX))->text();
        this->getApp()->setScaleX(this->getSelectedName(), text.toFloat());
    });

    this->createLineEdit("y", Property::ScaleY, this->doubleValidator, [this]() {
        auto text = ((QLineEdit *)this->getWidget(Property::ScaleY))->text();
        this->getApp()->setScaleY(this->getSelectedName(), text.toFloat());
    });

    this->endPropertiesGroup("Scale");

    // Anchor
    this->startPropertiesGroup("Anchor");

    this->createLineEdit("x", Property::AnchorX, this->doubleValidator, [this]() {
        auto lineEdit = (QLineEdit *)this->getWidget(Property::AnchorX);
        auto text = lineEdit->text();
        text = this->toRatioValue(text);
        this->getApp()->setAnchorX(this->getSelectedName(), text.toFloat());
        lineEdit->setText(text);
    });

    this->createLineEdit("y", Property::AnchorY, this->doubleValidator, [this]() {
        auto lineEdit = (QLineEdit *)this->getWidget(Property::AnchorY);
        auto text = lineEdit->text();
        text = this->toRatioValue(text);
        this->getApp()->setAnchorY(this->getSelectedName(), text.toFloat());
        lineEdit->setText(text);
    });

    this->endPropertiesGroup("Anchor");

    // Origin
    this->startPropertiesGroup("Origin");

    this->createLineEdit("x", Property::OriginX, this->doubleValidator, [this]() {
        auto lineEdit = (QLineEdit *)this->getWidget(Property::OriginX);
        auto text = lineEdit->text();
        text = this->toRatioValue(text);
        this->getApp()->setOriginX(this->getSelectedName(), text.toFloat());
        lineEdit->setText(text);
    });

    this->createLineEdit("y", Property::OriginY, this->doubleValidator, [this]() {
        auto lineEdit = (QLineEdit *)this->getWidget(Property::OriginY);
        auto text = lineEdit->text();
        text = this->toRatioValue(text);
        this->getApp()->setOriginY(this->getSelectedName(), text.toFloat());
        lineEdit->setText(text);
    });

    this->endPropertiesGroup("Origin");


    QPushButton *colorButton = new QPushButton(this);
    colorButton->setAutoFillBackground(true);
    colorButton->setFlat(true);
    auto palette = colorButton->palette();
    QColor color = QColor(Qt::white);
    palette.setColor(QPalette::Button, color);
    colorButton->setPalette(palette);
    QString text;
    text.sprintf("(%2.2f, %2.2f, %2.2f, %2.2f)", color.redF(), color.greenF(), color.blueF(), color.alphaF());
    colorButton->setText(text);
    this->connect(colorButton, &QPushButton::clicked, [this, colorButton](bool checked) {
        auto palette = colorButton->palette();
        QColor color = QColorDialog::getColor(palette.color(QPalette::Button), this);
        if (!color.isValid()) return;

        palette.setColor(QPalette::Button, color);
        QString text;
        text.sprintf("(%2.2f, %2.2f, %2.2f, %2.2f)", color.redF(), color.greenF(), color.blueF(), color.alphaF());
        colorButton->setText(text);
        if ((color.redF() + color.greenF() + color.blueF()) / 3.0f >= 0.5f) {
            palette.setColor(QPalette::ButtonText, QColor(Qt::black));
        } else {
            palette.setColor(QPalette::ButtonText, QColor(Qt::white));
        }
        colorButton->setPalette(palette);

        mog::Color mogColor = mog::Color(color.redF(), color.greenF(), color.blueF(), color.alphaF());
        this->getApp()->setColor(this->getSelectedName(), mogColor);
    });
    int colorRow = this->addPropertyItem("color", colorButton);
    this->propertyRows[Property::Color] = colorRow;

    this->endPropertiesGroup("Common");
}

void MainWindow::initPropertiesRoundedRectangle()
{
    auto doubleValidator = new QDoubleValidator(this);

    this->startPropertiesGroup("RoundedRectangle");

    this->createLineEdit("cornerRadius", Property::CornerRadius, this->doubleValidator, [this]() {
        float cornerRadius = ((QLineEdit *)this->getWidget(Property::CornerRadius))->text().toFloat();
        this->getApp()->replaceRoundedRectangle(this->getSelectedName(), cornerRadius);
    });

    this->endPropertiesGroup("RoundedRectangle");
}

void MainWindow::initPropertiesCircle()
{
    auto doubleValidator = new QDoubleValidator(this);

    this->startPropertiesGroup("Circle");

    this->createLineEdit("radius", Property::Radius, this->doubleValidator, [this]() {
        float radius = ((QLineEdit *)this->getWidget(Property::Radius))->text().toFloat();
        this->getApp()->replaceCircle(this->getSelectedName(), radius);
    });

    this->endPropertiesGroup("Circle");
}

void MainWindow::initPropertiesLabel()
{
    auto doubleValidator = new QDoubleValidator(this);

    this->startPropertiesGroup("Label");

    auto setLabelProperty = [this]() {
        auto name = this->getSelectedName();
        auto textW = ((QLineEdit *)this->getWidget(Property::Text))->text();
        auto fontSizeW = ((QLineEdit *)this->getWidget(Property::FontSize))->text();
        auto fontFilenameW = ((QLineEdit *)this->getWidget(Property::FontFilename))->text();
        auto fontHeightW = ((QLineEdit *)this->getWidget(Property::FontHeight))->text();
        this->getApp()->replaceLabel(name, textW.toStdString(), fontSizeW.toFloat(),fontFilenameW.toStdString(), fontHeightW.toFloat());
    };

    this->createLineEdit("text", Property::Text, nullptr, setLabelProperty);
    this->createLineEdit("fontSize", Property::FontSize, this->doubleValidator, setLabelProperty);
    this->createLineEdit("fontFilename", Property::FontFilename, nullptr, setLabelProperty);
    this->createLineEdit("fontHeight", Property::FontHeight, this->doubleValidator, setLabelProperty);

    this->endPropertiesGroup("Label");
}

void MainWindow::initPropertiesSprite()
{
    auto doubleValidator = new QDoubleValidator(this);

    this->startPropertiesGroup("Sprite");

    auto setSpriteProperty = [this]() {
        auto name = this->getSelectedName();
        Platform platform = (Platform)(this->ui->comboBox_Platform->currentIndex() + 1);

        auto filename = ((QComboBox *)this->getWidget(Property::Filename))->currentText();
        auto rectX = ((QLineEdit *)this->getWidget(Property::RectX))->text();
        auto rectY = ((QLineEdit *)this->getWidget(Property::RectY))->text();
        auto rectW = ((QLineEdit *)this->getWidget(Property::RectWidth))->text();
        auto rectH = ((QLineEdit *)this->getWidget(Property::RectHeight))->text();

        std::string filepath = this->getAssetFilePath(platform, filename.toStdString());
        mog::Rect rect = mog::Rect(rectX.toFloat(), rectY.toFloat(), rectW.toFloat(), rectH.toFloat());

        this->getApp()->replaceSprite(name, filepath, rect);
        this->setPropertyValuesFromEntity();
    };

    QStringList items;
    items.append("");
    for (auto asset : this->assetsSet) {
        items.append(asset.c_str());
    }
    this->createComboBox("filename", Property::Filename, items, [this, setSpriteProperty](const QString &text) {
        auto name = this->getSelectedName();
        this->getApp()->setWidth(name, 0);
        this->getApp()->setHeight(name, 0);
        setSpriteProperty();
    });

    this->startPropertiesGroup("Rect");

    this->createLineEdit("x", Property::RectX, this->doubleValidator, setSpriteProperty);
    this->createLineEdit("y", Property::RectY, this->doubleValidator, setSpriteProperty);
    this->createLineEdit("width", Property::RectWidth, this->doubleValidator, setSpriteProperty);
    this->createLineEdit("height", Property::RectHeight, this->doubleValidator, setSpriteProperty);

    this->endPropertiesGroup("Rect");

    this->endPropertiesGroup("Sprite");
}

void MainWindow::initPropertiesSlice9Sprite()
{
    auto setSlice9SpriteProperty = [this]() {
        auto name = this->getSelectedName();
        Platform platform = (Platform)(this->ui->comboBox_Platform->currentIndex() + 1);

        auto filename = ((QComboBox *)this->getWidget(Property::Filename))->currentText();
        auto rectX = ((QLineEdit *)this->getWidget(Property::RectX))->text();
        auto rectY = ((QLineEdit *)this->getWidget(Property::RectY))->text();
        auto rectW = ((QLineEdit *)this->getWidget(Property::RectWidth))->text();
        auto rectH = ((QLineEdit *)this->getWidget(Property::RectHeight))->text();
        auto centerRectX = ((QLineEdit *)this->getWidget(Property::CenterRectX))->text();
        auto centerRectY = ((QLineEdit *)this->getWidget(Property::CenterRectY))->text();
        auto centerRectW = ((QLineEdit *)this->getWidget(Property::CenterRectWidth))->text();
        auto centerRectH = ((QLineEdit *)this->getWidget(Property::CenterRectHeight))->text();

        std::string filepath = this->getAssetFilePath(platform, filename.toStdString());
        mog::Rect rect = mog::Rect(rectX.toFloat(), rectY.toFloat(), rectW.toFloat(), rectH.toFloat());
        mog::Rect centerRect = mog::Rect(centerRectX.toFloat(), centerRectY.toFloat(), centerRectW.toFloat(), centerRectH.toFloat());

        this->getApp()->replaceSlice9Sprite(name, filepath, rect, centerRect);
        this->setPropertyValuesFromEntity();
    };

    this->startPropertiesGroup("Slice9Sprite");

    this->startPropertiesGroup("Sprite");

    QStringList items;
    items.append("");
    for (auto asset : this->assetsSet) {
        items.append(asset.c_str());
    }
    this->createComboBox("filename", Property::Filename, items, [setSlice9SpriteProperty](const QString &text) {
        setSlice9SpriteProperty();
    });

    this->startPropertiesGroup("Rect");

    this->createLineEdit("x", Property::RectX, this->doubleValidator, setSlice9SpriteProperty);
    this->createLineEdit("y", Property::RectY, this->doubleValidator, setSlice9SpriteProperty);
    this->createLineEdit("width", Property::RectWidth, this->doubleValidator, setSlice9SpriteProperty);
    this->createLineEdit("height", Property::RectHeight, this->doubleValidator, setSlice9SpriteProperty);

    this->endPropertiesGroup("Rect");

    this->endPropertiesGroup("Sprite");

    this->startPropertiesGroup("CenterRect");

    this->createLineEdit("x", Property::CenterRectX, this->doubleValidator, setSlice9SpriteProperty);
    this->createLineEdit("y", Property::CenterRectY, this->doubleValidator, setSlice9SpriteProperty);
    this->createLineEdit("width", Property::CenterRectWidth, this->doubleValidator, setSlice9SpriteProperty);
    this->createLineEdit("height", Property::CenterRectHeight, this->doubleValidator, setSlice9SpriteProperty);

    this->endPropertiesGroup("CenterRect");

    this->endPropertiesGroup("Slice9Sprite");
}

void MainWindow::initPropertiesSpriteSheet()
{
    auto setSpriteSheetProperty = [this]() {
        auto name = this->getSelectedName();
        Platform platform = (Platform)(this->ui->comboBox_Platform->currentIndex() + 1);

        auto filename = ((QComboBox *)this->getWidget(Property::Filename))->currentText();
        auto rectX = ((QLineEdit *)this->getWidget(Property::RectX))->text();
        auto rectY = ((QLineEdit *)this->getWidget(Property::RectY))->text();
        auto rectW = ((QLineEdit *)this->getWidget(Property::RectWidth))->text();
        auto rectH = ((QLineEdit *)this->getWidget(Property::RectHeight))->text();
        auto frameSizeWidth = ((QLineEdit *)this->getWidget(Property::FrameSizeWidth))->text();
        auto frameSizeHeight = ((QLineEdit *)this->getWidget(Property::FrameSizeHeight))->text();
        auto frameCount = ((QLineEdit *)this->getWidget(Property::FrameCount))->text();
        auto margin = ((QLineEdit *)this->getWidget(Property::Margin))->text();

        std::string filepath = this->getAssetFilePath(platform, filename.toStdString());
        mog::Rect rect = mog::Rect(rectX.toFloat(), rectY.toFloat(), rectW.toFloat(), rectH.toFloat());
        mog::Size frameSize = mog::Size(frameSizeWidth.toFloat(), frameSizeHeight.toFloat());

        this->getApp()->replaceSpriteSheet(name, filepath, rect, frameSize, frameCount.toInt(), margin.toInt());
        this->setPropertyValuesFromEntity();
    };

    this->startPropertiesGroup("SpriteSheet");

    this->startPropertiesGroup("Sprite");

    QStringList items;
    items.append("");
    for (auto asset : this->assetsSet) {
        items.append(asset.c_str());
    }
    this->createComboBox("filename", Property::Filename, items, [setSpriteSheetProperty](const QString &text) {
        setSpriteSheetProperty();
    });

    this->startPropertiesGroup("Rect");

    this->createLineEdit("x", Property::RectX, this->doubleValidator, setSpriteSheetProperty);
    this->createLineEdit("y", Property::RectY, this->doubleValidator, setSpriteSheetProperty);
    this->createLineEdit("width", Property::RectWidth, this->doubleValidator, setSpriteSheetProperty);
    this->createLineEdit("height", Property::RectHeight, this->doubleValidator, setSpriteSheetProperty);

    this->endPropertiesGroup("Rect");

    this->endPropertiesGroup("Sprite");

    this->startPropertiesGroup("frameSize");

    this->createLineEdit("width", Property::FrameSizeWidth, this->doubleValidator, setSpriteSheetProperty);
    this->createLineEdit("height", Property::FrameSizeHeight, this->doubleValidator, setSpriteSheetProperty);

    this->endPropertiesGroup("frameSize");

    this->createLineEdit("frameCount", Property::FrameCount, this->intValidator, setSpriteSheetProperty);
    this->createLineEdit("margin", Property::Margin, this->intValidator, setSpriteSheetProperty);

    this->startPropertiesGroup("test");

    QPushButton *playButton = new QPushButton(this);
    playButton->setText("Start Animation");
    this->connect(playButton, &QPushButton::clicked, [this]() {
        auto button = (QPushButton *)this->getWidget(Property::Play);

        if (button->text() == "Stop Animation") {
            this->getApp()->stopAnimationSpriteSheet(this->getSelectedName());

        } else {
            button->setText("Stop Animation");

            this->getApp()->setSpriteSheetAnimationFinish(this->getSelectedName(), [this]() {
                auto button = (QPushButton *)this->getWidget(Property::Play);
                button->setText("Start Animation");
            });
            this->getApp()->startAnimationSpriteSheet(this->getSelectedName());
        }
    });
    int playRow = this->addPropertyItem("", playButton);
    this->propertyRows[Property::Play] = playRow;

    QPushButton *resetButton = new QPushButton(this);
    resetButton->setText("Reset");
    this->connect(resetButton, &QPushButton::clicked, [this]() {
        this->getApp()->resetAnimationSpriteSheet(this->getSelectedName());
    });
    int resetRow = this->addPropertyItem("", resetButton);
    this->propertyRows[Property::Reset] = resetRow;


    this->endPropertiesGroup("test");

    this->endPropertiesGroup("SpriteSheet");
}

void MainWindow::initPropertiesGroup()
{
    this->startPropertiesGroup("Group");

    this->endPropertiesGroup("Group");
}

void MainWindow::startPropertiesGroup(QString name)
{
    int row = this->ui->tableWidget_Properties->rowCount();
    this->ui->tableWidget_Properties->insertRow(row);
    QTableWidgetItem *groupHeaderItem = new QTableWidgetItem();
    groupHeaderItem->setFlags(groupHeaderItem->flags() ^ Qt::ItemIsEditable);
    groupHeaderItem->setIcon(QApplication::style()->standardIcon(QStyle::SP_TitleBarUnshadeButton));
    groupHeaderItem->setText(name);
    groupHeaderItem->setBackgroundColor(QColor::fromRgbF(0.8, 0.8, 0.8));
    this->ui->tableWidget_Properties->setSpan(row, 0, 1, 2);
    this->ui->tableWidget_Properties->setItem(row, 0, groupHeaderItem);

    this->propertiesGroupStack++;
    this->propertiesGroups[name] = std::pair<int, int>(row, row);
}

int MainWindow::addPropertyItem(QString name, QWidget *widget)
{
    int row = this->ui->tableWidget_Properties->rowCount();
    this->ui->tableWidget_Properties->insertRow(row);
    QTableWidgetItem *labelItem = new QTableWidgetItem();
    labelItem->setText("      " + name);
    labelItem->setFlags(labelItem->flags() ^ Qt::ItemIsEditable);
    this->ui->tableWidget_Properties->setItem(row, 0, labelItem);
    this->ui->tableWidget_Properties->setCellWidget(row, 1, widget);
    return row;
}

void MainWindow::endPropertiesGroup(QString name)
{
    this->propertiesGroupStack--;
    int row = this->ui->tableWidget_Properties->rowCount();
    auto pair = this->propertiesGroups[name];
    this->propertiesGroups[name] = std::pair<int, int>(pair.first, row - 1);
}

void MainWindow::applicationStateChanged(Qt::ApplicationState state)
{
    if (this->projectPath.count() == 0 && state == Qt::ApplicationState::ApplicationActive) {
        if (this->isOpenProjectDialog) {
            return;
        }
        if (!this->openMogProject()) {
            QApplication::quit();
        }
        this->entitiesItemSelectionChanged();
    }
}

bool MainWindow::openMogProject()
{
    this->isOpenProjectDialog = true;
    QFileDialog fileDialog(this);
    fileDialog.setFileMode(QFileDialog::Directory);
    fileDialog.setOption(QFileDialog::ShowDirsOnly, true);
    if (fileDialog.exec()){
        QStringList filePaths = fileDialog.selectedFiles();
        this->projectPath = filePaths.at(0);
        this->initAssets();
        this->isOpenProjectDialog = false;
        return true;
    }
    this->isOpenProjectDialog = false;
    return false;
}

void MainWindow::initAssets()
{
    this->ui->treeWidget_Assets->clear();

    const map<QString, QString> platformToAssetsDir {
        {"iOS", "assets_ios"},
        {"Android", "assets_android"},
        {"Mac", "assets_mac"},
        {"Qt", "assets_qt"},
    };
    auto platform = this->ui->comboBox_Platform->currentText();
    auto platformDir = platformToAssetsDir.at(platform);

    QDir dir(this->projectPath);
    if (!dir.exists()) return;

    QStringList assetsList;
    for (auto info : dir.entryInfoList()) {
        if (info.isDir()) {
            if (info.fileName() == "assets" || info.fileName() == platformDir) {
                assetsList.append(info.absoluteFilePath());
            }
        }
    }

    this->assetsSet.clear();
    this->assetsPathMap.clear();
    for (auto assetsPath : assetsList) {
        auto assetsDir = QDir(assetsPath);
        auto item = this->findAssetItems(assetsDir);
        this->ui->treeWidget_Assets->addTopLevelItem(item);
    }

    if (this->propertyRows.count(Property::Filename) > 0 && this->propertyRows[Property::Filename] >= 0) {
        auto comboBox = (QComboBox *)this->ui->tableWidget_Properties->cellWidget(this->propertyRows[Property::Filename], 1);
        auto selectedText = comboBox->currentText();
        comboBox->clear();
        comboBox->addItem("");
        for (auto asset : this->assetsSet) {
            comboBox->addItem(QString(asset.c_str()));
        }
        comboBox->setCurrentText(selectedText);
    }
}

QTreeWidgetItem *MainWindow::findAssetItems(QDir assetsDir)
{
    auto item = new QTreeWidgetItem();
    item->setIcon(0, QApplication::style()->standardIcon(QStyle::SP_DirIcon));
    item->setText(0, assetsDir.dirName());

    for (auto info : assetsDir.entryInfoList()) {
        if (info.fileName().startsWith(".")) continue;

        if (info.isDir()) {
            auto childItem = this->findAssetItems(QDir(info.absoluteFilePath()));
            childItem->setIcon(0, QApplication::style()->standardIcon(QStyle::SP_DirIcon));
            item->addChild(childItem);

        } else {
            auto childItem = new QTreeWidgetItem();
            childItem->setText(0, info.fileName());
            childItem->setIcon(0, QApplication::style()->standardIcon(QStyle::SP_FileIcon));
            item->addChild(childItem);

            if (this->isImageFile(info)) {
                auto i = childItem;
                QStringList pathList;
                pathList.append(i->text(0));
                QRegExp exp("@[0-9_]+x");
                while (auto p = i->parent()) {
                    if (!exp.exactMatch(p->text(0))) {
                        pathList.append(p->text(0));
                    }
                    i = p;
                }
                std::reverse(pathList.begin(), pathList.end());
                std::string asset = pathList.join("/").toStdString();
                this->assetsSet.insert(asset);
                this->assetsPathMap[info.absoluteFilePath().toStdString()] = asset;
            }
        }
    }

    return item;
}

bool MainWindow::isImageFile(QFileInfo fileInfo)
{
    auto extension = fileInfo.suffix();
    QStringList imageExtensions;
    imageExtensions.append("png");
    imageExtensions.append("jpg");
    imageExtensions.append("jpeg");
    imageExtensions.append("tga");
    imageExtensions.append("bmp");
    imageExtensions.append("psd");
    imageExtensions.append("gif");
    imageExtensions.append("hdr");
    imageExtensions.append("pic");

    if (imageExtensions.indexOf(extension.toLower()) > -1) {
        return true;
    }
    return false;
}

void MainWindow::on_comboBox_CreateEntity_currentIndexChanged(int index)
{
    if (index == 0) return;

    auto items = this->ui->treeWidget_Entities->selectedItems();
    if (items.count() != 1) {
        this->ui->comboBox_CreateEntity->setCurrentIndex(0);
        return;
    }
    auto selectItem = items.at(0);
    if (selectItem->text(1) != QString("Group")) {
        this->ui->comboBox_CreateEntity->setCurrentIndex(0);
        return;
    }

    MogGLWidget *mogGlWidget = (MogGLWidget *)this->ui->openGLWidget;
    auto app = mogGlWidget->engineController->getApp();
    QString name = this->ui->comboBox_CreateEntity->itemText(index);
    string nameStr = name.toStdString();

    int i = 0;
    while (this->nameSet.count(nameStr) > 0) {
        i++;
        nameStr = nameStr + to_string(i);
    }
    std::string parentNameStr = selectItem->text(0).toStdString();
    mog::EntityType entityType = this->toEntityType(name.toStdString());

    this->getApp()->createEntity(entityType, nameStr, parentNameStr);
    this->nameSet.insert(nameStr);

    auto child = new QTreeWidgetItem(selectItem);
    child->setText(0, QString(nameStr.c_str()));
    child->setText(1, name);
    child->setSelected(true);
    selectItem->setSelected(false);
    selectItem->setExpanded(true);

    this->ui->comboBox_CreateEntity->setCurrentIndex(0);

    this->entitiesItemSelectionChanged();
}

void MainWindow::entitiesItemSelectionChanged()
{
    auto common = this->propertiesGroups["Common"];
    int rowCount = this->ui->tableWidget_Properties->rowCount();
    set<int> rowSet;
    for (int i = common.second + 1; i < rowCount; i++) {
        this->ui->tableWidget_Properties->removeRow(common.second + 1);
        rowSet.insert(i);
    }
    for (auto pair : this->propertyRows) {
        if (rowSet.count(pair.second) > 0) {
            this->propertyRows[pair.first] = -1;
        }
    }

    std::string entityTypeStr = this->getSelectedEntityType();
    if (entityTypeStr.length() > 0) {
        auto entityType = this->toEntityType(entityTypeStr);

        this->ui->comboBox_CreateEntity->setEnabled(false);
        switch (entityType) {
        case mog::EntityType::RoundedRectangle:
            this->initPropertiesRoundedRectangle();
            break;

        case mog::EntityType::Circle:
            this->initPropertiesCircle();
            break;

        case mog::EntityType::Label:
            this->initPropertiesLabel();
            break;

        case mog::EntityType::Sprite:
            this->initPropertiesSprite();
            break;

        case mog::EntityType::Slice9Sprite:
            this->initPropertiesSlice9Sprite();
            break;

        case mog::EntityType::SpriteSheet:
            this->initPropertiesSpriteSheet();
            break;

        case mog::EntityType::Group:
            this->ui->comboBox_CreateEntity->setEnabled(true);
            break;
        }
    }

    this->setPropertyValuesFromEntity();
}

void MainWindow::setPropertyValuesFromEntity() {
    std::string name = this->getSelectedName();
    std::string entityTypeName = this->getSelectedEntityType();

    auto position = this->getApp()->getPosition(name);
    auto size = this->getApp()->getSize(name);
    auto scale = this->getApp()->getScale(name);
    auto anchor = this->getApp()->getAnchor(name);
    auto origin = this->getApp()->getOrigin(name);
    auto color = this->getApp()->getColor(name);
    bool isRatioWidth = this->getApp()->isRatioWidth(name);
    bool isRatioHeight = this->getApp()->isRatioHeight(name);

    auto nameW = (QLineEdit *)this->getWidget(Property::Name);
    nameW->setText(QString::fromStdString(name));

    auto positionXW = (QLineEdit *)this->getWidget(Property::PositionX);
    positionXW->setText(this->formatFloat(position.x));

    auto positionYW = (QLineEdit *)this->getWidget(Property::PositionY);
    positionYW->setText(this->formatFloat(position.y));

    auto sizeWW = (QLineEdit *)this->getWidget(Property::Width);
    sizeWW->setText(this->formatFloat(size.width));

    auto sizeWRW = (QCheckBox *)this->getWidget(Property::IsRatioWidth);
    sizeWRW->setChecked(isRatioWidth);

    auto sizeHRW = (QCheckBox *)this->getWidget(Property::IsRatioHeight);
    sizeHRW->setChecked(isRatioHeight);

    auto sizeHW = (QLineEdit *)this->getWidget(Property::Height);
    sizeHW->setText(this->formatFloat(size.height));

    auto scaleXW = (QLineEdit *)this->getWidget(Property::ScaleX);
    scaleXW->setText(this->formatFloat(scale.x));

    auto scaleYW = (QLineEdit *)this->getWidget(Property::ScaleY);;
    scaleYW->setText(this->formatFloat(scale.y));

    auto anchorXW = (QLineEdit *)this->getWidget(Property::AnchorX);
    anchorXW->setText(this->formatFloat(anchor.x));

    auto anchorYW = (QLineEdit *)this->getWidget(Property::AnchorY);
    anchorYW->setText(this->formatFloat(anchor.y));

    auto originXW = (QLineEdit *)this->getWidget(Property::OriginX);
    originXW->setText(this->formatFloat(origin.x));

    auto originYW = (QLineEdit *)this->getWidget(Property::OriginY);
    originYW->setText(this->formatFloat(origin.y));

    auto colorButton = (QPushButton *)this->getWidget(Property::Color);
    auto palette = colorButton->palette();
    palette.setColor(QPalette::Button, QColor((int)(color.r * 255.0f), (int)(color.g * 255.0f), (int)(color.b * 255.0f), (int)(color.a * 255.0f)));
    if ((color.r + color.g + color.b) / 3.0f >= 0.5f) {
        palette.setColor(QPalette::ButtonText, QColor(Qt::black));
    } else {
        palette.setColor(QPalette::ButtonText, QColor(Qt::white));
    }
    colorButton->setPalette(palette);

    bool enabled = (name.length() > 0);
    nameW->setEnabled(enabled);
    positionXW->setEnabled(enabled);
    positionYW->setEnabled(enabled);
    sizeWW->setEnabled(enabled);
    sizeHW->setEnabled(enabled);
    scaleXW->setEnabled(enabled);
    scaleYW->setEnabled(enabled);
    anchorXW->setEnabled(enabled);
    anchorYW->setEnabled(enabled);
    originXW->setEnabled(enabled);
    originYW->setEnabled(enabled);
    colorButton->setEnabled(enabled);

    if (name == "root") {
        nameW->setEnabled(false);
    }

    if (entityTypeName.length() == 0) return;

    auto entityType = this->toEntityType(entityTypeName);
    switch (entityType) {
        case mog::EntityType::Label: {
            auto text = this->getApp()->getLabelText(name);
            float fontSize = this->getApp()->getLabelFontSize(name);
            auto fontFilename = this->getApp()->getLabelFontFilename(name);
            float fontHeight = this->getApp()->getLabelHeight(name);

            auto textW = (QLineEdit *)this->getWidget(Property::Text);
            textW->setText(text.c_str());

            auto fontSizeW = (QLineEdit *)this->getWidget(Property::FontSize);
            fontSizeW->setText(this->formatFloat(this->getApp()->getLabelFontSize(name)));

            auto fontFilenameW = (QLineEdit *)this->getWidget(Property::FontFilename);
            fontFilenameW->setText(fontFilename.c_str());

            auto fontHeightW = (QLineEdit *)this->getWidget(Property::FontHeight);
            fontHeightW->setText(this->formatFloat(fontHeight));

            break;
        }
        case mog::EntityType::Sprite:
        case mog::EntityType::Slice9Sprite:
        case mog::EntityType::SpriteSheet: {
            auto filepath = this->getApp()->getSpriteFilename(name);
            auto rect = this->getApp()->getSpriteRect(name);

            auto filenameW = (QComboBox *)this->getWidget(Property::Filename);
            if (filepath.length() > 0 && filenameW->currentText().length() == 0) {
                std::string assetName = this->assetsPathMap[filepath];
                filenameW->setCurrentText(assetName.c_str());
            }

            auto rectXW = (QLineEdit *)this->getWidget(Property::RectX);
            rectXW->setText(this->formatFloat(rect.position.x));

            auto rectYW = (QLineEdit *)this->getWidget(Property::RectY);
            rectYW->setText(this->formatFloat(rect.position.y));

            auto rectWW = (QLineEdit *)this->getWidget(Property::RectWidth);
            rectWW->setText(this->formatFloat(rect.size.width));

            auto rectHW = (QLineEdit *)this->getWidget(Property::RectHeight);
            rectHW->setText(this->formatFloat(rect.size.height));

            if (entityType == mog::EntityType::Slice9Sprite) {
                auto centerRect = this->getApp()->getSlice9SpriteCenterRect(name);

                auto centerRectXW = (QLineEdit *)this->getWidget(Property::CenterRectX);
                centerRectXW->setText(this->formatFloat(centerRect.position.x));

                auto centerRectYW = (QLineEdit *)this->getWidget(Property::CenterRectY);
                centerRectYW->setText(this->formatFloat(centerRect.position.y));

                auto centerRectWW = (QLineEdit *)this->getWidget(Property::CenterRectWidth);
                centerRectWW->setText(this->formatFloat(centerRect.size.width));

                auto centerRectHW = (QLineEdit *)this->getWidget(Property::CenterRectHeight);
                centerRectHW->setText(this->formatFloat(centerRect.size.height));
            }
            if (entityType == mog::EntityType::SpriteSheet) {
                auto frameSize = this->getApp()->getSpriteSheetFrameSize(name);
                auto frameCount = this->getApp()->getSpriteSheetFrameCount(name);
                auto margin = this->getApp()->getSpriteSheetMargin(name);

                auto frameSizeWidthW = (QLineEdit *)this->getWidget(Property::FrameSizeWidth);
                frameSizeWidthW->setText(this->formatFloat(frameSize.width));

                auto frameSizeHeightW = (QLineEdit *)this->getWidget(Property::FrameSizeHeight);
                frameSizeHeightW->setText(this->formatFloat(frameSize.height));

                auto frameCountW = (QLineEdit *)this->getWidget(Property::FrameCount);
                frameCountW->setText(QString::number(frameCount));

                auto marginW = (QLineEdit *)this->getWidget(Property::Margin);
                marginW->setText(QString::number(margin));
            }

            break;
        }
    }
}

shared_ptr<mog::App> MainWindow::getApp() {
    MogGLWidget *mogGlWidget = (MogGLWidget *)this->ui->openGLWidget;
    auto app = mogGlWidget->engineController->getApp();
    return static_pointer_cast<mog::App>(app);
}

std::string MainWindow::getSelectedName() {
    auto items = this->ui->treeWidget_Entities->selectedItems();
    if (items.count() == 0) {
        this->ui->comboBox_CreateEntity->setCurrentIndex(0);
        return "";
    }
    auto selectItem = items.at(0);
    QString name = selectItem->text(0);
    return name.toStdString();
}

std::string MainWindow::getSelectedEntityType() {
    auto items = this->ui->treeWidget_Entities->selectedItems();
    if (items.count() == 0) {
        this->ui->comboBox_CreateEntity->setCurrentIndex(0);
        return "";
    }
    auto selectItem = items.at(0);
    return selectItem->text(1).toStdString();
}

mog::EntityType MainWindow::toEntityType(std::string entityTypeName) {
    return MainWindow::entityTypeMap.at(entityTypeName);
}

mog::Dictionary MainWindow::createDictionary(mog::EntityType entityType) {
    mog::Dictionary dict;

    dict.put(mog::MogUILoader::PropertyNames::EntityType, mog::Int((int)entityType));
    this->setPropertiesCommon(dict);

    return dict;
}

void MainWindow::setPropertiesCommon(mog::Dictionary &dict) {
    auto nameW = (QLineEdit *)this->getWidget(Property::Name);
    string name = nameW->text().toStdString();
    dict.put(mog::MogUILoader::PropertyNames::Name, mog::String(name));

    auto tagW = (QLineEdit *)this->getWidget(Property::Tag);
    string tag = tagW->text().toStdString();
    dict.put(mog::MogUILoader::PropertyNames::Tag, mog::String(tag));

    auto positionXW = (QLineEdit *)this->getWidget(Property::PositionX);
    float positionX = positionXW->text().toFloat();
    dict.put(mog::MogUILoader::PropertyNames::PositionX, mog::Float(positionX));

    auto positionYW = (QLineEdit *)this->getWidget(Property::PositionY);
    float positionY = positionYW->text().toFloat();
    dict.put(mog::MogUILoader::PropertyNames::PositionY, mog::Float(positionY));

    auto sizeWW = (QLineEdit *)this->getWidget(Property::Width);
    float width = sizeWW->text().toFloat();
    dict.put(mog::MogUILoader::PropertyNames::Width, mog::Float(width));

    auto sizeHW = (QLineEdit *)this->getWidget(Property::Height);
    float height = sizeHW->text().toFloat();
    dict.put(mog::MogUILoader::PropertyNames::Height, mog::Float(height));

    auto scaleXW = (QLineEdit *)this->getWidget(Property::ScaleX);
    float scaleX = scaleXW->text().toFloat();
    dict.put(mog::MogUILoader::PropertyNames::ScaleX, mog::Float(scaleX));

    auto scaleYW = (QLineEdit *)this->getWidget(Property::ScaleY);
    float scaleY = scaleYW->text().toFloat();
    dict.put(mog::MogUILoader::PropertyNames::ScaleY, mog::Float(scaleY));

    auto anchorXW = (QLineEdit *)this->getWidget(Property::AnchorX);
    float anchorX = anchorXW->text().toFloat();
    dict.put(mog::MogUILoader::PropertyNames::AnchorX, mog::Float(anchorX));

    auto anchorYW = (QLineEdit *)this->getWidget(Property::AnchorY);
    float anchorY = anchorYW->text().toFloat();
    dict.put(mog::MogUILoader::PropertyNames::AnchorY, mog::Float(anchorY));

    auto originXW = (QLineEdit *)this->getWidget(Property::OriginX);
    float originX = anchorYW->text().toFloat();
    dict.put(mog::MogUILoader::PropertyNames::OriginX, mog::Float(originX));

    auto originYW = (QLineEdit *)this->getWidget(Property::OriginY);
    float originY = originYW->text().toFloat();
    dict.put(mog::MogUILoader::PropertyNames::OriginY, mog::Float(originY));
}

QString MainWindow::formatFloat(float f) {
    auto str = QString::number(f, 'f', 6);
    QStringList list = str.split('.');
    QString s1 = list.at(0);
    QString s2 = list.at(1);

    char c[64];
    sprintf(c, "%d.%d", s1.toInt(), s2.toInt());
    return QString(c);
}

std::vector<std::string> MainWindow::getAssetsPlatformDir(Platform platform) {
    std::vector<std::string> vec;
    switch (platform) {
    case Platform::IOS:
        vec.emplace_back("assets_ios");
        break;
    case Platform::Android:
        vec.emplace_back("assets_android");
        break;
    case Platform::Mac:
        vec.emplace_back("assets_mac");
        break;
    case Platform::Qt:
        vec.emplace_back("assets_qt");
        break;
    }
    vec.emplace_back("assets");
    return vec;
}

std::string MainWindow::getAssetFilePath(Platform platform, std::string filename) {
    std::vector<std::string> platformDirs = this->getAssetsPlatformDir(platform);
    std::vector<std::string> densityDirs {"@2x", "@3x", "@1_5x"};
    for (std::string platformDir : platformDirs) {
        QString platformPath = QDir(this->projectPath).filePath(QString(platformDir.c_str()));
        for (std::string densityDir : densityDirs) {
            QString densityPath = QDir(platformPath).filePath(densityDir.c_str());
            QString filePath = QDir(densityPath).filePath(QString(filename.c_str()));
            if (QFile(filePath).exists()) {
                return filePath.toStdString();
            }
        }
        QString filePath = QDir(platformPath).filePath(QString(filename.c_str()));
        if (QFile(filePath).exists()) {
            return filePath.toStdString();
        }
    }
    return "";
}

void MainWindow::platformChanged(int index) {
    this->initAssets();
}

