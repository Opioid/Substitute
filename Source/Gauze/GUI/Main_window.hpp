#pragma once

#include "Image/Image_provider.hpp"
#include "Image/Thumbnail_provider.hpp"
#include "Export/Exporter.hpp"
#include "Export/Export_item.hpp"
#include "Settings/Settings.hpp"
#include <QMainWindow>
#include <cstdint>

class QComboBox;
class QCheckBox;
class QDoubleSpinBox;
class QSlider;
class QLabel;
class QProgressBar;

namespace Ui
{

class Main_window;

}

class Main_window : public QMainWindow
{
    Q_OBJECT

public:

    explicit Main_window(QWidget* parent = nullptr);

    ~Main_window();

protected:

	void closeEvent(QCloseEvent* event);

public slots:

	void slot_color_tab_new_file_loaded(const QString& file_name);
	void slot_normals_tab_new_file_loaded(const QString& file_name);
	void slot_surface_tab_new_file_loaded(const QString& file_name);

private slots:

	void on_export_button__clicked();

	void on_open_settings_action__triggered();

private:

	Ui::Main_window* ui;

	QProgressBar* progress_bar_;

	Image_provider     image_provider_;
	Thumbnail_provider thumbnail_provider_;

	Exporter exporter_;
	Export_set export_set_;

	Settings settings_;

	static const std::string s_settings_file_name;
	static const uint2 s_thumbnail_dimensions;
};
