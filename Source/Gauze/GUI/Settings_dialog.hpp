#pragma once

#include "Settings/Settings.hpp"

#include <QDialog>

namespace Ui
{

class Settings_dialog;

}

class Settings_dialog : public QDialog
{
	Q_OBJECT

public:

	explicit Settings_dialog(QWidget* parent, const Settings& settings);
	~Settings_dialog();

	const Settings& get_settings() const;

private slots:

	void on_accept_button__clicked();

	void on_cancel_button__clicked();

	void on_output_directory_button__clicked();

private:

	Ui::Settings_dialog* ui;

	Settings settings_;
};
