#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QImage>
#include <QString>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QPixmap>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <QListWidgetItem>
#include <QElapsedTimer>
#include <QDrag>
#include <QDropEvent>
#include <QPainter>
#include <QAbstractItemView>
#include "ImageProcessing.h"
#include "SkyMapMatching.h"
#include "exif.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_openButton_clicked();

    void on_focalLengthInput_editingFinished();

    void on_evalButton_clicked();

    void on_tabWidget_currentChanged(int index);
    void showAboutDialog();
    void on_starPointTable_cellDoubleClicked(int row, int column);

    void on_simButton_clicked();

    void on_simStarPointTable_cellDoubleClicked(int row, int column);

protected:
    void dragEnterEvent(QDragEnterEvent*event);//拖动进入事件
    void dropEvent(QDropEvent*event);

private:
    Ui::MainWindow *ui;
    void loadPicture(QString);
    int findMatchingStar(int);
    void getAlgorithm();
    double evalStarMapMatching(EvalArgs);
    void simStarMapMatching(GeneratedImage);
    vector<StarPoint> starRecs,starMap;
    vector<QString> starNames;
    SkyMapMatching* pSMM;
    QImage skyImg;
    double posX,posY,focus;
    bool algorithm[4]={false};
    QStringList algorithmNames;
    simResult sr;
};
const QString name_path= ":/Data/Data/sky_table_name_SAO.csv";
const QString data_path= ":/Data/Data/sky_table_loc_SAO.csv";
vector<StarPoint> loadStarPoint(QString);
vector<StarPoint> initStarMapMatching(SkyMapMatching*,ImageProperties);

vector<QString> loadStarNames(QString);
#endif // MAINWINDOW_H
