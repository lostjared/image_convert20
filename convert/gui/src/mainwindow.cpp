#include "mainwindow.hpp"
#include "converter.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("Image Convert Qt");
    resize(760, 520);

    auto *central = new QWidget(this);
    auto *mainLayout = new QVBoxLayout(central);

    auto *filesGroup = new QGroupBox("Input", central);
    auto *filesLayout = new QVBoxLayout(filesGroup);

    auto *listRow = new QHBoxLayout;
    listFileEdit = new QLineEdit(filesGroup);
    auto *listBrowse = new QPushButton("Browse List", filesGroup);
    listRow->addWidget(new QLabel("Input list file", filesGroup));
    listRow->addWidget(listFileEdit);
    listRow->addWidget(listBrowse);
    filesLayout->addLayout(listRow);

    auto *imageRow = new QHBoxLayout;
    imageFileEdit = new QLineEdit(filesGroup);
    auto *imageBrowse = new QPushButton("Browse Image", filesGroup);
    imageRow->addWidget(new QLabel("Single image file", filesGroup));
    imageRow->addWidget(imageFileEdit);
    imageRow->addWidget(imageBrowse);
    filesLayout->addLayout(imageRow);

    auto *optionsGroup = new QGroupBox("Options", central);
    auto *form = new QFormLayout(optionsGroup);

    outputFormatBox = new QComboBox(optionsGroup);
    outputFormatBox->addItems({"png", "jpg", "jpeg", "bmp", "webp", "tiff"});

    resizeCheck = new QCheckBox("Resize output", optionsGroup);
    widthSpin = new QSpinBox(optionsGroup);
    heightSpin = new QSpinBox(optionsGroup);
    qualitySpin = new QSpinBox(optionsGroup);

    widthSpin->setRange(1, 32768);
    heightSpin->setRange(1, 32768);
    widthSpin->setValue(640);
    heightSpin->setValue(360);

    qualitySpin->setRange(0, 100);
    qualitySpin->setValue(100);

    form->addRow("Output format", outputFormatBox);
    form->addRow("JPEG quality", qualitySpin);
    form->addRow("", resizeCheck);
    form->addRow("Width", widthSpin);
    form->addRow("Height", heightSpin);

    convertButton = new QPushButton("Convert", central);

    logEdit = new QTextEdit(central);
    logEdit->setReadOnly(true);

    mainLayout->addWidget(filesGroup);
    mainLayout->addWidget(optionsGroup);
    mainLayout->addWidget(convertButton);
    mainLayout->addWidget(new QLabel("Log", central));
    mainLayout->addWidget(logEdit, 1);

    setCentralWidget(central);

    connect(listBrowse, &QPushButton::clicked, this, &MainWindow::browseListFile);
    connect(imageBrowse, &QPushButton::clicked, this, &MainWindow::browseImageFile);
    connect(convertButton, &QPushButton::clicked, this, &MainWindow::runConversion);
    connect(resizeCheck, &QCheckBox::toggled, this, &MainWindow::updateResizeState);

    updateResizeState();
}

void MainWindow::browseListFile() {
    const QString file = QFileDialog::getOpenFileName(this, "Select input list file", QString(), "Text Files (*.txt);;All Files (*)");
    if (!file.isEmpty()) {
        listFileEdit->setText(file);
    }
}

void MainWindow::browseImageFile() {
    const QString file = QFileDialog::getOpenFileName(this, "Select image file", QString(), "Images (*.png *.jpg *.jpeg *.bmp *.tif *.tiff *.webp);;All Files (*)");
    if (!file.isEmpty()) {
        imageFileEdit->setText(file);
    }
}

void MainWindow::updateResizeState() {
    const bool enabled = resizeCheck->isChecked();
    widthSpin->setEnabled(enabled);
    heightSpin->setEnabled(enabled);
}

void MainWindow::runConversion() {
    logEdit->clear();

    ConvertOptions options;
    options.outputFormat = outputFormatBox->currentText().toStdString();
    options.quality = qualitySpin->value();

    if (resizeCheck->isChecked()) {
        options.width = widthSpin->value();
        options.height = heightSpin->value();
    }

    if (!listFileEdit->text().trimmed().isEmpty()) {
        std::string error;
        auto loaded = loadInputList(listFileEdit->text().trimmed().toStdString(), error);
        if (!error.empty()) {
            QMessageBox::warning(this, "Error", QString::fromStdString(error));
            return;
        }
        options.files.insert(options.files.end(), loaded.begin(), loaded.end());
    }

    if (!imageFileEdit->text().trimmed().isEmpty()) {
        options.files.emplace_back(imageFileEdit->text().trimmed().toStdString());
    }

    auto result = convertFiles(options);
    for (const auto &line : result.log) {
        logEdit->append(QString::fromStdString(line));
    }

    if (result.success) {
        QMessageBox::information(this, "Done", QString("Converted %1 image(s).").arg(result.converted));
    } else {
        QMessageBox::warning(this, "No files converted", "No images were converted.");
    }
}
