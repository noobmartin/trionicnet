#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  Q_INIT_RESOURCE(icons);
  MainWindow w;
  w.show();

  return a.exec();
}
