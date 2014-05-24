#include "Image_controls.hpp"
#include "ui_Image_controls.h"
#include "Qt_helper/Signal_blocker.hpp"

#include "Image/Image_provider.hpp"
#include "Image/Thumbnail_provider.hpp"
#include "Image/Filter/To_BGRA.hpp"
#include "Image/Filter/Channel_to_grayscale.hpp"
#include "Image/Filter/Invert.hpp"
#include "Image/Filter/Modulate.hpp"
#include "Image/Filter/Copy.hpp"

#include <QFileDialog>
#include <QColorDialog>

Image_controls::Image_controls(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::Image_controls),
	thumbnail_(rendering::Data_format::R8G8B8A8_UNorm_sRGB),
	thumbnail_provider_(nullptr)
{
	ui->setupUi(this);
}

Image_controls::~Image_controls()
{
	delete ui;
}

void Image_controls::init(uint32_t num_channels, const rendering::Color3& constant, bool treat_missing_source_as_constant,
						  Thumbnail_provider* thumbnail_provider)
{
	treat_missing_source_as_constant_ = treat_missing_source_as_constant;

	thumbnail_provider_ = thumbnail_provider;

	bool single_channel = 1 == num_channels;

	if (treat_missing_source_as_constant)
	{
		ui->constant_stacked_widget_->setCurrentIndex(single_channel ? 1 : 0);
	}
	else
	{
		ui->constant_stacked_widget_->setCurrentIndex(2);
	}


	ui->manipulator_stacked_widget_->setCurrentIndex(single_channel ? 1 : 0);

	constant_ = constant;
	ui->scalar_constant_spin_box_->setValue(double(constant.x));

	if (single_channel)
	{
		QStringList channels;
		channels.append("Red");
		channels.append("Green");
		channels.append("Blue");
		channels.append("Alpha");

		ui->channels_combo_box_->addItems(channels);
	}

	treat_missing_source_as_constant_ = treat_missing_source_as_constant;

	ui->image_source_combo_box_->addItem(treat_missing_source_as_constant ? "Use constant" : "");

	thumbnail_.resize(thumbnail_provider->thumbnail_dimensions());

	update_constant();
}

void Image_controls::add_loaded_file_name(const QString& file_name)
{
	int index = ui->image_source_combo_box_->findText(file_name);

	if (-1 == index)
	{
		ui->image_source_combo_box_->addItem(file_name);
	}
}

void Image_controls::configure_export_item(Export_item& item, Image_provider& image_provider)
{
	if (0 != ui->image_source_combo_box_->currentIndex())
	{
		item.cached_image = image_provider.get_image(ui->image_source_combo_box_->currentText().toStdString());

		item.inverted = is_inverted();

		item.modulate_factor = get_modulate_factor();
	}
	else
	{
		item.cached_image = nullptr;
	}

	item.selection = get_selection();
	item.constant = constant_;
}

void Image_controls::on_open_file_button__clicked()
{
	QString file_name = QFileDialog::getOpenFileName(this, "Open image file");

	if (!file_name.isNull())
	{
		int index = ui->image_source_combo_box_->findText(file_name);

		if (-1 == index)
		{
			// we don't have the file name in our combo box yet

			if (thumbnail_provider_->get_thumbnail(file_name.toStdString()))
			{
				add_loaded_file_name(file_name);

				int num_items = ui->image_source_combo_box_->count();
				ui->image_source_combo_box_->setCurrentIndex(num_items - 1);

				emit new_file_loaded(file_name);
			}
		}
		else
		{
			ui->image_source_combo_box_->setCurrentIndex(index);
		}
	}
}

void Image_controls::on_image_source_combo_box__currentIndexChanged(int index)
{
	if (0 == index)
	{
		update_constant();
	}
	else
	{
		update_image();
	}
}

void Image_controls::on_color_constant_button__clicked()
{
	rendering::Color3 rgb = rendering::linear_to_sRGB(constant_);

	QColor qcolor = QColorDialog::getColor(QColor::fromRgbF(rgb.r, rgb.g, rgb.b), this, "Color constant");

	if (qcolor.isValid())
	{
		rgb = rendering::Color3(qcolor.redF(), qcolor.greenF(), qcolor.blueF());

		set_constant(rendering::sRGB_to_linear(rgb));

		if (0 == ui->image_source_combo_box_->currentIndex())
		{
			update_constant();
		}
		else
		{
			ui->image_source_combo_box_->setCurrentIndex(0);
		}
	}
}

void Image_controls::on_scalar_constant_spin_box__valueChanged(double arg1)
{
	set_constant(static_cast<float>(arg1));

	if (0 == ui->image_source_combo_box_->currentIndex())
	{
		update_constant();
	}
	else
	{
		ui->image_source_combo_box_->setCurrentIndex(0);
	}
}

void Image_controls::on_channels_combo_box__currentIndexChanged(int /*index*/)
{
	update_image();
}

void Image_controls::on_inverted_check_box__clicked(bool /*checked*/)
{
	update_image();
}

void Image_controls::on_modulate_spin_box__valueChanged(double value)
{
	int slider_value = static_cast<int>(value * 100.0);

	Signal_blocker blocker(ui->modulate_spin_box_);
	ui->modulate_slider_->setValue(slider_value);

	update_image();
}

void Image_controls::on_modulate_slider__valueChanged(int value)
{
	double spin_box_value = static_cast<double>(value) / 100.0;

	Signal_blocker blocker(ui->modulate_slider_);
	ui->modulate_spin_box_->setValue(spin_box_value);

	update_image();
}

bool Image_controls::is_single_channel() const
{
	return ui->manipulator_stacked_widget_->currentIndex() == 1;
}

Export_item::Selection Image_controls::get_selection() const
{
	if (0 != ui->image_source_combo_box_->currentIndex())
	{
		if (ui->channels_combo_box_)
		{
			return static_cast<Export_item::Selection>(ui->channels_combo_box_->currentIndex());
		}
		else
		{
			return Export_item::Selection::RGB;
		}
	}
	else
	{
		if (treat_missing_source_as_constant_)
		{
			return Export_item::Selection::Constant;
		}
		else
		{
			return Export_item::Selection::Ignore;
		}
	}
}

bool Image_controls::is_inverted() const
{
	if (ui->inverted_check_box_)
	{
		return Qt::Checked == ui->inverted_check_box_->checkState();
	}
	else
	{
		return false;
	}
}

float Image_controls::get_modulate_factor() const
{
	if (ui->modulate_slider_)
	{
		return float(ui->modulate_spin_box_->value());
	}
	else
	{
		return 1.f;
	}
}

void Image_controls::set_constant(float constant)
{
	constant_ = rendering::Color3(constant, constant, constant);
}

void Image_controls::set_constant(const rendering::Color3& constant)
{
	constant_ = constant;
}

void Image_controls::set_checkerboard()
{
	if (!thumbnail_provider_)
	{
		return;
	}

	const Image_buffer<rendering::Color4c>& checkerboard = thumbnail_provider_->checkerboard();

	set_thumbnail(checkerboard);
}

void Image_controls::update_constant()
{
	if (!treat_missing_source_as_constant_)
	{
		set_checkerboard();
	}
	else
	{
		rendering::Color3 rgb = rendering::linear_to_sRGB(constant_);

		set_thumbnail(QColor::fromRgbF(rgb.r, rgb.g, rgb.b));
	}

	set_image_manipulator_enabled(false);
}

void Image_controls::update_image()
{
	if (!thumbnail_provider_)
	{
		return;
	}

	QString file_name = ui->image_source_combo_box_->currentText();

	Image_buffer<rendering::Color4>* thumbnail = thumbnail_provider_->get_thumbnail(file_name.toStdString());

	if (!thumbnail)
	{
		return;
	}

	if (is_single_channel())
	{
		set_thumbnail(*thumbnail, static_cast<uint32_t>(ui->channels_combo_box_->currentIndex()), is_inverted(), get_modulate_factor());
	}
	else
	{
		set_thumbnail(*thumbnail);
	}

	set_image_manipulator_enabled(true);
}

void Image_controls::set_thumbnail(const Image_buffer<rendering::Color4c>& source)
{
	To_BGRA filter(true);
	filter.filter(thumbnail_, source);

	QImage qimage(reinterpret_cast<const uchar*>(thumbnail_.data()),
				  thumbnail_.dimensions().x, thumbnail_.dimensions().y, QImage::Format_RGB32);

	QPixmap pixmap = QPixmap::fromImage(qimage);

	ui->image_label_->setPixmap(pixmap);
}

void Image_controls::set_thumbnail(const Image_buffer<rendering::Color4>& source)
{
	const uint2& source_dimensions = source.dimensions();
	const uint2& thumbnail_dimensions = thumbnail_.dimensions();

	if (source_dimensions.x < thumbnail_dimensions.x || source_dimensions.y < thumbnail_dimensions.y)
	{
		Copy<rendering::Color4c> copy;
		copy.filter(thumbnail_, thumbnail_provider_->checkerboard());
	}

	To_BGRA filter(true);
	filter.filter(thumbnail_, source);

	QImage qimage(reinterpret_cast<const uchar*>(thumbnail_.data()),
				  thumbnail_dimensions.x, thumbnail_dimensions.y, QImage::Format_RGB32);

	QPixmap pixmap = QPixmap::fromImage(qimage);

	ui->image_label_->setPixmap(pixmap);
}

void Image_controls::set_thumbnail(const Image_buffer<rendering::Color4>& source, uint32_t channel, bool inverted, float modulate_factor)
{
	Image_buffer<rendering::Color4>& scratch_buffer = thumbnail_provider_->scratch_buffer();

	Channel_to_grayscale<rendering::Color4> to_grayscale(channel);
	to_grayscale.filter(scratch_buffer, source);

	if (inverted)
	{
		Invert<rendering::Color4> invert;
		invert.filter(scratch_buffer, scratch_buffer);
	}

	if (1.f != modulate_factor)
	{
		Modulate modulate(modulate_factor);
		modulate.filter(scratch_buffer, scratch_buffer);
	}

	const uint2& source_dimensions = source.dimensions();
	const uint2& thumbnail_dimensions = thumbnail_.dimensions();

	if (source_dimensions.x < thumbnail_dimensions.x || source_dimensions.y < thumbnail_dimensions.y)
	{
		Copy<rendering::Color4c> copy;
		copy.filter(thumbnail_, thumbnail_provider_->checkerboard());
	}

	// No need to swap channels for Qt here, as it is grayscale
	To_BGRA filter(false);
	filter.filter(thumbnail_, scratch_buffer, source.dimensions());

	QImage qimage(reinterpret_cast<const uchar*>(thumbnail_.data()),
				  thumbnail_dimensions.x, thumbnail_dimensions.y, QImage::Format_RGB32);

	QPixmap pixmap = QPixmap::fromImage(qimage);

	ui->image_label_->setPixmap(pixmap);
}

void Image_controls::set_thumbnail(const QColor& color)
{
	QPixmap pixmap = QPixmap(ui->image_label_->width(), ui->image_label_->height());
	pixmap.fill(color);
	ui->image_label_->setPixmap(pixmap);
}

void Image_controls::set_image_manipulator_enabled(bool enabled)
{
	if (1 == ui->manipulator_stacked_widget_->currentIndex())
	{
		ui->manipulator_stacked_widget_->currentWidget()->setEnabled(enabled);
	}
}
