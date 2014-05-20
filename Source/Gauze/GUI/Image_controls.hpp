#pragma once

#include "Image/Image_buffer.hpp"
#include "Export/Export_item.hpp"

#include <QWidget>

class Image_provider;
class Thumbnail_provider;

namespace Ui
{
	class Image_controls;
}

class Image_controls : public QWidget
{
	Q_OBJECT

public:

	explicit Image_controls(QWidget* parent = nullptr);
	~Image_controls();

	void init(uint32_t num_channels, const rendering::Color3& constant, bool treat_missing_source_as_constant,
			  Thumbnail_provider* thumbnail_provider);

	void add_loaded_file_name(const QString& file_name);

	void configure_export_item(Export_item& item, Image_provider& image_provider);

signals:

	void new_file_loaded(const QString& file_name);

private slots:

	void on_open_file_button__clicked();

	void on_image_source_combo_box__currentIndexChanged(int index);

	void on_color_constant_button__clicked();

	void on_scalar_constant_spin_box__valueChanged(double arg1);

	void on_channels_combo_box__currentIndexChanged(int index);

	void on_inverted_check_box__clicked(bool checked);

	void on_modulate_spin_box__valueChanged(double arg1);

	void on_modulate_slider__valueChanged(int value);

private:

	bool is_single_channel() const;

	Export_item::Selection get_selection() const;

	bool is_inverted() const;

	float get_modulate_factor() const;

	void set_constant(float constant);
	void set_constant(const rendering::Color3& constant);

	void set_checkerboard();

	void update_constant();
	void update_image();

	void set_thumbnail(const Image_buffer<rendering::Color4c>& thumbnail);
	void set_thumbnail(const Image_buffer<rendering::Color4>& thumbnail);
	void set_thumbnail(const Image_buffer<rendering::Color4>& thumbnail, uint32_t channel, bool inverted, float modulate_factor);
	void set_thumbnail(const QColor& constant);

	void set_image_manipulator_enabled(bool enabled);

	Ui::Image_controls *ui;

	rendering::Color3 constant_;

	bool treat_missing_source_as_constant_;

	Image_buffer<rendering::Color4c> thumbnail_;

	Thumbnail_provider* thumbnail_provider_;
};
