// Copyright 2016 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#pragma once
#include <memory>
#include <QKeyEvent>

#include "citra_qt/config.h"
#include "core/settings.h"
#include "ui_configure_input.h"

class QObject;
class QPushButton;
class QString;
class QWidget;

namespace Ui {
    class ConfigureInput;
}

class ConfigureInput : public QWidget
{
    Q_OBJECT

public:
    explicit ConfigureInput(QWidget* parent = nullptr);
    ~ConfigureInput();
    void applyConfiguration();

    public Q_SLOTS:
    void handleClick();
    void restoreDefaults();
private:
    std::unique_ptr<Ui::ConfigureInput> ui;
    std::map<Settings::NativeInput::Values, QPushButton*> input_mapping;
    std::vector<int> keys_pressed;
    QPushButton* changing_button = nullptr; /// button currently waiting for key press.

    void setConfiguration();
    void setKey();
    void removeDuplicates(const QString& newValue);
    void keyPressEvent(QKeyEvent* event) override;
    QString getKeyName(int key_code) const;
    Qt::Key ConfigureInput::getKeyValue(const QString& text) const;
};
