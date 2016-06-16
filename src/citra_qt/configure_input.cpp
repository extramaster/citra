// Copyright 2016 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#include <utility>

#include "citra_qt/configure_input.h"

ConfigureInput::ConfigureInput(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::ConfigureInput)
{
    ui->setupUi(this);

    // Initialize mapping of input enum to UI button.
    input_mapping = {
        { std::make_pair(Settings::NativeInput::Values::A, ui->btnFaceA) },
        { std::make_pair(Settings::NativeInput::Values::B, ui->btnFaceB) },
        { std::make_pair(Settings::NativeInput::Values::X, ui->btnFaceX) },
        { std::make_pair(Settings::NativeInput::Values::Y, ui->btnFaceY) },
        { std::make_pair(Settings::NativeInput::Values::L, ui->btnShdrL) },
        { std::make_pair(Settings::NativeInput::Values::R, ui->btnShdrR) },
        { std::make_pair(Settings::NativeInput::Values::ZL, ui->btnShdrZL) },
        { std::make_pair(Settings::NativeInput::Values::ZR, ui->btnShdrZR) },
        { std::make_pair(Settings::NativeInput::Values::START, ui->btnStart) },
        { std::make_pair(Settings::NativeInput::Values::SELECT, ui->btnSelect) },
        { std::make_pair(Settings::NativeInput::Values::HOME, ui->btnHome) },
        { std::make_pair(Settings::NativeInput::Values::DUP, ui->btnDirUp) },
        { std::make_pair(Settings::NativeInput::Values::DDOWN, ui->btnDirDown) },
        { std::make_pair(Settings::NativeInput::Values::DLEFT, ui->btnDirLeft) },
        { std::make_pair(Settings::NativeInput::Values::DRIGHT, ui->btnDirRight) },
        { std::make_pair(Settings::NativeInput::Values::CUP, ui->btnStickUp) },
        { std::make_pair(Settings::NativeInput::Values::CDOWN, ui->btnStickDown) },
        { std::make_pair(Settings::NativeInput::Values::CLEFT, ui->btnStickLeft) },
        { std::make_pair(Settings::NativeInput::Values::CRIGHT, ui->btnStickRight) },
        { std::make_pair(Settings::NativeInput::Values::CIRCLE_UP, ui->btnCircleUp) },
        { std::make_pair(Settings::NativeInput::Values::CIRCLE_DOWN, ui->btnCircleDown) },
        { std::make_pair(Settings::NativeInput::Values::CIRCLE_LEFT, ui->btnCircleLeft) },
        { std::make_pair(Settings::NativeInput::Values::CIRCLE_RIGHT, ui->btnCircleRight) },
    };

    // Attach handle click method to each button click.
    for (const auto& entry : input_mapping) {
        connect(entry.second, SIGNAL(released()), this, SLOT(handleClick()));
    }
    connect(ui->btnRestoreDefaults, SIGNAL(released()), this, SLOT(restoreDefaults()));
    setFocusPolicy(Qt::ClickFocus);
    this->setConfiguration();
}

ConfigureInput::~ConfigureInput()
{
}

/// Event handler for all button released() event.
void ConfigureInput::handleClick()
{
    QPushButton* sender = qobject_cast<QPushButton*>(QObject::sender());
    previous_mapping = sender->text();
    sender->setText(tr("[waiting]"));
    sender->setFocus();
    grabKeyboard();
    grabMouse();
    changing_button = sender;
}

/// Save all button configurations to settings file
void ConfigureInput::applyConfiguration()
{
    for (int i = 0; i < Settings::NativeInput::NUM_INPUTS - 1; ++i) {
        int value = getKeyValue(input_mapping[Settings::NativeInput::Values(i)]->text());
        Settings::values.input_mappings[Settings::NativeInput::All[i]] = value;
    }
    Settings::Apply();
}

/// Load configuration settings into button text
void ConfigureInput::setConfiguration()
{
    for (int i = 0; i < Settings::NativeInput::NUM_INPUTS - 1; ++i) {
        QString keyValue = getKeyName(Settings::values.input_mappings[i]);
        input_mapping[Settings::NativeInput::Values(i)]->setText(keyValue);
    }
}

/// Handle key press event for input tab when a button is 'waiting'.
void ConfigureInput::keyPressEvent(QKeyEvent* event)
{
    if (changing_button != nullptr && event->key() != Qt::Key_unknown)
    {
        key_pressed = event->key();
        setKey();
    }
}

/// Set button text to name of key pressed.
void ConfigureInput::setKey()
{
    QString key_value = getKeyName(key_pressed);
    if (key_pressed == Qt::Key_Escape)
        changing_button->setText(previous_mapping);
    else
        changing_button->setText(key_value);
    removeDuplicates(key_value);
    key_pressed = Qt::Key_unknown;
    releaseKeyboard();
    releaseMouse();
    changing_button = nullptr;
    previous_mapping = nullptr;
}

/// Convert key ASCII value to its' letter/name
QString ConfigureInput::getKeyName(int key_code) const
{
    if (key_code == Qt::Key_Shift)
        return tr("Shift");

    if (key_code == Qt::Key_Control)
        return tr("Ctrl");

    if (key_code == Qt::Key_Alt)
        return tr("Alt");

    if (key_code == Qt::Key_Meta)
        return "";

    if (key_code == -1)
        return "";

    return QKeySequence(key_code).toString();
}

/// Convert letter/name of key to its ASCII value.
Qt::Key ConfigureInput::getKeyValue(const QString& text) const
{
    if (text == "Shift")
        return Qt::Key_Shift;
    if (text == "Ctrl")
        return Qt::Key_Control;
    if (text == "Alt")
        return Qt::Key_Alt;
    if (text == "Meta")
        return Qt::Key_unknown;
    if (text == "")
        return Qt::Key_unknown;
    return Qt::Key(QKeySequence(text)[0]);
}

/// Check all inputs for duplicate keys. Clears out any other button with same key as new button.
void ConfigureInput::removeDuplicates(const QString& newValue)
{
    for (int i = 0; i < Settings::NativeInput::NUM_INPUTS - 1; ++i) {
        if (changing_button != input_mapping[Settings::NativeInput::Values(i)]) {
            QString oldValue = input_mapping[Settings::NativeInput::Values(i)]->text();
            if (newValue == oldValue)
                input_mapping[Settings::NativeInput::Values(i)]->setText("");
        }
    }
}

/// Restore all buttons to their default values.
void ConfigureInput::restoreDefaults() {
    for (int i = 0; i < Settings::NativeInput::NUM_INPUTS - 1; ++i) {
        QString keyValue = getKeyName(Config::GetDefaultInput()[i].toInt());
        input_mapping[Settings::NativeInput::Values(i)]->setText(keyValue);
    }
}
