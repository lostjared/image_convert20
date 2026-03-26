#pragma once

#include <QMainWindow>

class QCheckBox;
class QComboBox;
class QLineEdit;
class QPushButton;
class QSpinBox;
class QTextEdit;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void browseListFile();
    void browseImageFile();
    void runConversion();
    void updateResizeState();

private:
    QLineEdit *listFileEdit;
    QLineEdit *imageFileEdit;
    QComboBox *outputFormatBox;
    QCheckBox *resizeCheck;
    QSpinBox *widthSpin;
    QSpinBox *heightSpin;
    QSpinBox *qualitySpin;
    QTextEdit *logEdit;
    QPushButton *convertButton;
};
