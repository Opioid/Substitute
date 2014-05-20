#include "GUI/Main_window.hpp"
#include <QApplication>
#include <QFile>
#include <QStyle>

int main(int argc, char *argv[])
{
	Q_INIT_RESOURCE(Resources);

	QApplication application(argc, argv);

	// Load an application style
	QFile style_file(":/GUI/GUI/Style.qss");
	style_file.open(QFile::ReadOnly);

	// Apply the loaded stylesheet
	QString style(style_file.readAll());
	application.setStyleSheet(style);

	Main_window window;
	window.show();

	return application.exec();
}
