#include "Main_window.hpp"
#include "ui_Main_window.h"

#include "Settings_dialog.hpp"

#include "Image/Filter/To_BGRA.hpp"
#include "Image_storage/Image_writer.hpp"

#include <QFileDialog>
#include <QColorDialog>
#include <QPixmap>
#include <QProgressBar>
#include <QMessageBox>

QImage to_QImage(const Image_buffer<rendering::Color4c>& source);

const std::string Main_window::s_settings_file_name = "settings.txt";

const uint2 Main_window::s_thumbnail_dimensions = uint2(192, 192);

Main_window::Main_window(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::Main_window),
	thumbnail_provider_(image_provider_, s_thumbnail_dimensions)
{
    ui->setupUi(this);

	settings_.load(s_settings_file_name);

	ui->material_name_line_edit_->setText("Material_name");

	ui->color_image_controls_->init(3, rendering::Color3(0.75f, 0.75f, 0.75f), true, &thumbnail_provider_);
	ui->opacity_image_controls_->init(1, rendering::Color3(1.f, 1.f, 1.f), true, &thumbnail_provider_);
	ui->emissive_image_controls_->init(1, rendering::Color3(0.f, 0.f, 0.f), true, &thumbnail_provider_);

	ui->normals_image_controls_->init(2, rendering::Color3(0.f, 0.f, 0.f), false, &thumbnail_provider_);
	ui->height_image_controls_->init(1, rendering::Color3(0.f, 0.f, 0.f), false, &thumbnail_provider_);

	ui->metallic_image_controls_->init(1, rendering::Color3(0.f, 0.f, 0.f), true, &thumbnail_provider_);
	ui->roughness_image_controls_->init(1, rendering::Color3(0.75f, 0.75f, 0.75f), true, &thumbnail_provider_);
	ui->cavity_image_controls_->init(1, rendering::Color3(1.f, 1.f, 1.f), false, &thumbnail_provider_);

	connect(ui->color_image_controls_, SIGNAL(new_file_loaded(const QString&)),
			this,					   SLOT(slot_color_tab_new_file_loaded(const QString&)));

	connect(ui->opacity_image_controls_, SIGNAL(new_file_loaded(const QString&)),
			this,						 SLOT(slot_color_tab_new_file_loaded(const QString&)));

	connect(ui->emissive_image_controls_, SIGNAL(new_file_loaded(const QString&)),
			this,						  SLOT(slot_color_tab_new_file_loaded(const QString&)));

	connect(ui->normals_image_controls_, SIGNAL(new_file_loaded(const QString&)),
			this,						 SLOT(slot_normals_tab_new_file_loaded(const QString&)));

	connect(ui->height_image_controls_, SIGNAL(new_file_loaded(const QString&)),
			this,						SLOT(slot_normals_tab_new_file_loaded(const QString&)));

	connect(ui->metallic_image_controls_, SIGNAL(new_file_loaded(const QString&)),
			this,						  SLOT(slot_surface_tab_new_file_loaded(const QString&)));

	connect(ui->roughness_image_controls_, SIGNAL(new_file_loaded(const QString&)),
			this,						   SLOT(slot_surface_tab_new_file_loaded(const QString&)));

	connect(ui->cavity_image_controls_, SIGNAL(new_file_loaded(const QString&)),
			this,						SLOT(slot_surface_tab_new_file_loaded(const QString&)));
}

Main_window::~Main_window()
{
    delete ui;
}

void Main_window::closeEvent(QCloseEvent* /*event*/)
{}

void Main_window::slot_color_tab_new_file_loaded(const QString& file_name)
{
	ui->color_image_controls_->add_loaded_file_name(file_name);
	ui->opacity_image_controls_->add_loaded_file_name(file_name);
	ui->emissive_image_controls_->add_loaded_file_name(file_name);
}

void Main_window::slot_normals_tab_new_file_loaded(const QString& file_name)
{
	ui->normals_image_controls_->add_loaded_file_name(file_name);
	ui->height_image_controls_->add_loaded_file_name(file_name);
}

void Main_window::slot_surface_tab_new_file_loaded(const QString& file_name)
{
	ui->metallic_image_controls_->add_loaded_file_name(file_name);
	ui->roughness_image_controls_->add_loaded_file_name(file_name);
	ui->cavity_image_controls_->add_loaded_file_name(file_name);
}

void Main_window::on_export_button__clicked()
{
	QApplication::setOverrideCursor(Qt::WaitCursor);

	ui->color_image_controls_->configure_export_item(export_set_.color, image_provider_);
	ui->opacity_image_controls_->configure_export_item(export_set_.opacity, image_provider_);
	ui->emissive_image_controls_->configure_export_item(export_set_.emissive, image_provider_);
	ui->normals_image_controls_->configure_export_item(export_set_.normals, image_provider_);
	ui->height_image_controls_->configure_export_item(export_set_.height, image_provider_);
	ui->metallic_image_controls_->configure_export_item(export_set_.metallic, image_provider_);
	ui->roughness_image_controls_->configure_export_item(export_set_.roughness, image_provider_);
	ui->cavity_image_controls_->configure_export_item(export_set_.cavity, image_provider_);

	exporter_.set_target_directory(settings_.output_directory, settings_.material_folder, settings_.texture_folder);
	exporter_.set_container(settings_.output_container);
	exporter_.write(ui->material_name_line_edit_->text().toStdString(), export_set_);

	QApplication::restoreOverrideCursor();
}

void Main_window::on_open_settings_action__triggered()
{
	Settings_dialog settings_dialog(this, settings_);

	if (QDialog::Accepted == settings_dialog.exec())
	{
		settings_ = settings_dialog.get_settings();

		settings_.save(s_settings_file_name);
	}
}
