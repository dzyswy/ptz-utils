#include "ptz_client.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	ptz_client w;
	w.show();
	return a.exec();
}
