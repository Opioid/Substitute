#include "Settings_dialog.hpp"
#include "ui_Settings_dialog.h"
#include "Image_storage/Storage_options.hpp"

#include <QFileDialog>

Settings_dialog::Settings_dialog(QWidget* parent, const Settings& settings) :
	QDialog(parent),
	ui(new Ui::Settings_dialog),
	settings_(settings)
{
	ui->setupUi(this);

	ui->output_directory_line_edit_->setText(QString::fromStdString(settings.output_directory));

	ui->material_folder_line_edit_->setText(QString::fromStdString(settings.material_folder));

	ui->texture_folder_line_edit_->setText(QString::fromStdString(settings.texture_folder));

	QStringList containers;

	for (size_t i = 0, count = static_cast<size_t>(Storage_options::Container::Count); i < count; ++i)
	{
		std::string container_name = Storage_options::get_container_name(static_cast<Storage_options::Container>(i));

		containers.append(QString::fromStdString(container_name));
	}

	ui->containers_combo_box_->addItems(containers);

	ui->containers_combo_box_->setCurrentIndex(static_cast<int>(settings.output_container));
}

Settings_dialog::~Settings_dialog()
{
	delete ui;
}

const Settings& Settings_dialog::get_settings() const
{
	return settings_;
}

void Settings_dialog::on_accept_button__clicked()
{
	settings_.output_directory = ui->output_directory_line_edit_->text().toStdString();
	settings_.material_folder = ui->material_folder_line_edit_->text().toStdString();
	settings_.texture_folder = ui->texture_folder_line_edit_->text().toStdString();

	settings_.output_container = static_cast<Storage_options::Container>(ui->containers_combo_box_->currentIndex());

	accept();
}

void Settings_dialog::on_cancel_button__clicked()
{
	reject();
}

void Settings_dialog::on_output_directory_button__clicked()
{
	QString directory = QFileDialog::getExistingDirectory(this, "Select directory");

	if (directory.isEmpty())
	{
		return;
	}

	ui->output_directory_line_edit_->setText(directory);
}
