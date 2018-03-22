#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "core.hpp"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->on_tabWidget_tabBarClicked(1);

    ui->first_model_epsC->setChecked(true);
    ui->first_model_kappatheta->setChecked(true);
    ui->first_model_kappaT->setChecked(true);
    ui->default_radiobutton_epsC->setChecked(true);
    ui->default_radiobutton_kappatheta->setChecked(true);
    ui->default_radiobutton_kappaT->setChecked(true);

    ui->saltiness_slider_epsC->setSliderPosition(35);
    ui->saltiness_slider_kappatheta->setSliderPosition(35);
    ui->temperature_slider_kappatheta->setSliderPosition(20);
    ui->alpha_slider_kappatheta->setSliderPosition(0);
    ui->saltiness_slider_kappaT->setSliderPosition(35);
    ui->theta_slider_kappaT->setSliderPosition(0);
    ui->alpha_slider_kappaT->setSliderPosition(90);

    this->on_saltiness_slider_epsC_sliderMoved(35);
    this->on_saltiness_slider_kappatheta_sliderMoved(35);
    this->on_temperature_slider_kappatheta_sliderMoved(20);
    this->on_alpha_slider_kappatheta_sliderMoved(0);
    this->on_saltiness_slider_kappaT_sliderMoved(35);
    this->on_theta_slider_kappaT_sliderMoved(0);
    this->on_alpha_slider_kappaT_sliderMoved(90);

    ui->saving_checkBox->setChecked(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::whereToSaveFile()
{
    QString filename = QFileDialog::getSaveFileName(
        this,
        "Save CSV",
        QDir::currentPath(),
        "CSV (*.csv)" );
    if (!filename.isNull())
    {
        qDebug( filename.toStdString().c_str() );
    }
    return filename;
}

void MainWindow::on_plotButton_clicked()
{
    if (ui->tabWidget->currentIndex() == 0) {

        //Epss

        if (ui->first_model_epsC->isChecked()) {
            models::CURRENT_MODEL = models::MODEL1;
        } else if (ui->second_model_epsC->isChecked()) {
            models::CURRENT_MODEL = models::MODEL2;
        }

        double saltiness = ui->saltiness_textEdit_epsC->toPlainText().toDouble();

        std::vector<double> temperature;
        if (ui->default_radiobutton_epsC->isChecked()) {
            temperature.push_back(0);
            temperature.push_back(20);
            temperature.push_back(40);
            temperature.push_back(60);
            temperature.push_back(80);
            temperature.push_back(100);
        } else if (ui->choose_radiobutton_epsC->isChecked()) {
            QString temp = ui->temperatures_textEdit_epsC->toPlainText();
            QRegExp rx("[, ]");
            QStringList list = temp.split(rx, QString::SkipEmptyParts);
            foreach (QString str, list) temperature.push_back(str.toDouble());
        }

        dndloss(models::CURRENT_MODEL, temperature, saltiness);

    }

    if (ui->tabWidget->currentIndex() == 1) {

        //Kappa from Theta

        if (ui->first_model_kappatheta->isChecked()) {
            models::CURRENT_MODEL = models::MODEL1;
        } else if (ui->second_model_kappatheta->isChecked()) {
            models::CURRENT_MODEL = models::MODEL2;
        }

        double temperature = ui->temperature_textEdit_kappatheta->toPlainText().toDouble();

        double saltiness = ui->saltiness_textEdit_kappatheta->toPlainText().toDouble();

        double alpha = ui->alpha_textEdit_kappatheta->toPlainText().toDouble();

        std::vector<double> lamda;
        if (ui->default_radiobutton_kappatheta->isChecked()) {
            lamda.push_back(0.8);
            lamda.push_back(1.6);
            lamda.push_back(2.4);
            lamda.push_back(3.2);
            lamda.push_back(4.);
        } else if (ui->choose_radiobutton_kappatheta->isChecked()) {
            QString temp = ui->lamda_textEdit_kappatheta->toPlainText();
            QRegExp rx("[, ]");
            QStringList list = temp.split(rx, QString::SkipEmptyParts);
            foreach (QString str, list) lamda.push_back(str.toDouble());
        }

        kappaFromTheta(models::CURRENT_MODEL, temperature, saltiness, alpha, lamda);

        if (ui->saving_checkBox->isChecked()) {
            std::string where = this->whereToSaveFile().toStdString();
            if (where != "") saveCSV(where);
        }

    }

    if (ui->tabWidget->currentIndex() == 2) {

        //Kappa from Temp

        if (ui->first_model_kappaT->isChecked()) {
            models::CURRENT_MODEL = models::MODEL1;
        } else if (ui->second_model_kappaT->isChecked()) {
            models::CURRENT_MODEL = models::MODEL2;
        }

        double saltiness = ui->saltiness_textEdit_kappaT->toPlainText().toDouble();

        double theta = ui->theta_textEdit_kappaT->toPlainText().toDouble();

        double alpha = ui->alpha_textEdit_kappaT->toPlainText().toDouble();

        std::vector<double> lamda;
        if (ui->default_radiobutton_kappaT->isChecked()) {
            lamda.push_back(0.8);
            lamda.push_back(1.35);
            lamda.push_back(3.2);
            lamda.push_back(6.0);
            lamda.push_back(21);
        } else if (ui->choose_radiobutton_kappaT->isChecked()) {
            QString temp = ui->lamda_textEdit_kappaT->toPlainText();
            QRegExp rx("[, ]");
            QStringList list = temp.split(rx, QString::SkipEmptyParts);
            foreach (QString str, list) lamda.push_back(str.toDouble());
        }

        kappaFromTemperature(models::CURRENT_MODEL, saltiness, theta, alpha, lamda);

        if (ui->saving_checkBox->isChecked()) {
            std::string where = this->whereToSaveFile().toStdString();
            if (where != "") saveCSV(where);
        }

    }

}

void MainWindow::on_tabWidget_tabBarClicked(int index)
{
    if (index == 0) {
        ui->saving_checkBox->setEnabled(false);
    }
    if (index == 1) {
        ui->saving_checkBox->setEnabled(true);
    }
    if (index == 3) {
        ui->saving_checkBox->setEnabled(true);
    }
}

void MainWindow::on_saltiness_slider_epsC_sliderMoved(int position)
{
    ui->saltiness_textEdit_epsC->setText(QString::number(position));
}

void MainWindow::on_saltiness_slider_kappatheta_sliderMoved(int position)
{
    ui->saltiness_textEdit_kappatheta->setText(QString::number(position));
}

void MainWindow::on_temperature_slider_kappatheta_sliderMoved(int position)
{
    ui->temperature_textEdit_kappatheta->setText(QString::number(position));
}

void MainWindow::on_alpha_slider_kappatheta_sliderMoved(int position)
{
    ui->alpha_textEdit_kappatheta->setText(QString::number(position));
}

void MainWindow::on_saltiness_slider_kappaT_sliderMoved(int position)
{
    ui->saltiness_textEdit_kappaT->setText(QString::number(position));
}

void MainWindow::on_theta_slider_kappaT_sliderMoved(int position)
{
    ui->theta_textEdit_kappaT->setText(QString::number(position));
}

void MainWindow::on_alpha_slider_kappaT_sliderMoved(int position)
{
    ui->alpha_textEdit_kappaT->setText(QString::number(position));
}
