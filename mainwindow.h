#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QString>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

    void on_plotButton_clicked();

    void on_tabWidget_tabBarClicked(int index);

    void on_saltiness_slider_epsC_sliderMoved(int position);

    void on_saltiness_slider_kappatheta_sliderMoved(int position);

    void on_temperature_slider_kappatheta_sliderMoved(int position);

    void on_alpha_slider_kappatheta_sliderMoved(int position);

    void on_saltiness_slider_kappaT_sliderMoved(int position);

    void on_theta_slider_kappaT_sliderMoved(int position);

    void on_alpha_slider_kappaT_sliderMoved(int position);

private:
    Ui::MainWindow *ui;

    QString whereToSaveFile();

};

#endif // MAINWINDOW_H
