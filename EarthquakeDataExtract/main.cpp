#include "EarthquakeDataExtract.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    EarthquakeDataExtract w;
    w.show();
    return a.exec();
}
