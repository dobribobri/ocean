#ifndef CORE_HPP
#define CORE_HPP

#include <iostream>
#include <stdio.h>
#include "ocean.h"
#include "gnuplot_i.hpp"
#include <QtCharts/QtCharts>
#include "dcolor.h"
#include <QWidget>
#include <fstream>

namespace models {
    model CURRENT_MODEL;
    model MODEL1 = "1";
    model MODEL2 = "2";
}

std::string dtos(double i)
{
    std::stringstream s;
    s << i;
    return s.str();
}

void saveCSV(std::string filename) {
    std::ifstream fin;
    fin.open(QDir::currentPath().toStdString() + "tmp.csv");
    std::ofstream fout;
    fout.open(filename);

    std::string buff;
    while (getline(fin, buff)) {
        fout << buff + "\n";
    }
    fin.close();
    fout.close();
}

void dndloss(model _model, std::vector<double> _temperature, double _saltiness) {
    Ocean* ocean = new Ocean();
    ocean->initialize(_model, NULL, _saltiness, NULL, NULL, NULL);

    QChart* chart = new QChart();
    chart->setTitle("Dielectric 'n' dielectric loss");

    QValueAxis *axisX = new QValueAxis;
    axisX->setTitleText("Lamda");
    axisX->setTickCount(10);
    chart->addAxis(axisX, Qt::AlignBottom);
    QValueAxis *axisY = new QValueAxis;
    axisY->setTitleText("Eps1 / Eps2");
    axisY->setTickCount(10);
    chart->addAxis(axisY, Qt::AlignLeft);

    double min, max; int f = 1;
    for (unsigned int t = 0; t < _temperature.size(); t++) {
        double temperature = _temperature[t];
        ocean->setTemperature(temperature);

        QLineSeries* s_e1 = new QLineSeries();
        QLineSeries* s_e2 = new QLineSeries();

        DColor* col = new DColor(t);
        QColor* color = new QColor(col->getR(), col->getG(), col->getB());
        QPen* pen = new QPen();
        pen->setWidthF(0.7);
        pen->setColor(*color);
        s_e1->setPen(*pen);
        s_e2->setPen(*pen);

        for (double i = 0.01; i < 100; i++) {
            ocean->setLamda(i);
            double eps1 = ocean->epsilon1();
            double eps2 = ocean->epsilon2();
            s_e1->append(i, eps1);
            s_e2->append(i, eps2);
            if (f) {
                min = max = (eps1 + eps2)/2;
                f = 0;
            } else {
                if (eps1 < min) min = eps1;
                if (eps2 < min) min = eps2;
                if (eps1 > max) max = eps1;
                if (eps2 > max) max = eps2;
            }
        }

        s_e2->setName(QString::fromStdString(dtos(_temperature[t])));

        chart->addSeries(s_e1);
        chart->addSeries(s_e2);

        s_e1->attachAxis(axisX);
        s_e1->attachAxis(axisY);
        s_e2->attachAxis(axisX);
        s_e2->attachAxis(axisY);

    }

    axisY->setRange(min - 0.5, max + 0.5);
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    QWidget* chartWindow = new QWidget(0);
    QVBoxLayout* layout = new QVBoxLayout(chartWindow);
    layout->addWidget(chartView);
    chartWindow->setLayout(layout);
    layout->activate();
    chartWindow->resize(1200,500);
    chartWindow->show();
    delete ocean;
    return;
}

void kappaFromTheta(model _model, double _temperature, double _saltiness, double _alpha, std::vector<double> _lamda) {
    Ocean* ocean = new Ocean();
    ocean->initialize(_model, _temperature, _saltiness, NULL, NULL, _alpha);

    QChart* chart = new QChart();
    chart->setTitle("Kappa(Theta)");

    QValueAxis *axisX = new QValueAxis;
    axisX->setTitleText("Theta");
    axisX->setTickCount(10);
    chart->addAxis(axisX, Qt::AlignBottom);
    QValueAxis *axisY = new QValueAxis;
    axisY->setTitleText("Kappa");
    axisY->setTickCount(10);
    chart->addAxis(axisY, Qt::AlignLeft);

    std::ofstream fout;
    fout.open(QDir::currentPath().toStdString() + "tmp.csv");

    double max = 0, min = 1;
    for (unsigned int i = 0; i < _lamda.size(); i++) {
        ocean->setLamda(_lamda[i]);
        ocean->recalculateEps();

        QLineSeries* s_l = new QLineSeries();
        QPen* pen = new QPen();
        pen->setWidthF(0.7);
        DColor* col = new DColor(i+1);
        QColor* color = new QColor(col->getR(), col->getG(), col->getB());
        pen->setColor(*color);
        s_l->setPen(*pen);

        for (double theta = 0.001; theta < 90; theta += 0.001) {
            ocean->setTheta(theta);

            double kappa = ocean->kappa();
            s_l->append(theta, kappa);

            if (kappa > max) max = kappa;
            if (kappa < min) min = kappa;

            std::string write = dtos(_lamda[i]) + ", " + dtos(theta) + ", " + dtos(kappa) + "\n";
            fout << write;

        }

        s_l->setName(QString::fromStdString(dtos(_lamda[i])));
        chart->addSeries(s_l);

        s_l->attachAxis(axisX);
        s_l->attachAxis(axisY);
    }

    fout.close();

    axisY->setRange(min - 0.15, max + 0.15);
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    QWidget* chartWindow = new QWidget(0);
    QVBoxLayout* layout = new QVBoxLayout(chartWindow);
    layout->addWidget(chartView);
    chartWindow->setLayout(layout);
    layout->activate();
    chartWindow->resize(800,500);
    chartWindow->show();
    delete ocean;
    return;
}

void kappaFromTemperature(model _model, double _saltiness, double _theta, double _alpha, std::vector<double> _lamda) {
    Ocean* ocean = new Ocean();
    ocean->initialize(_model, NULL, _saltiness, NULL, _theta, _alpha);

    QChart* chart = new QChart();
    chart->setTitle("Kappa(Temperature)");

    QValueAxis *axisX = new QValueAxis;
    axisX->setTitleText("Temperature");
    axisX->setTickCount(10);
    chart->addAxis(axisX, Qt::AlignBottom);
    QValueAxis *axisY = new QValueAxis;
    axisY->setTitleText("Kappa");
    axisY->setTickCount(10);
    chart->addAxis(axisY, Qt::AlignLeft);

    std::ofstream fout;
    fout.open(QDir::currentPath().toStdString() + "tmp.csv");

    double max = 0, min = 1;
    for (unsigned int i = 0; i < _lamda.size(); i++) {
        ocean->setLamda(_lamda[i]);

        QLineSeries* s_l = new QLineSeries();
        QPen* pen = new QPen();
        pen->setWidthF(0.7);
        DColor* col = new DColor(i+1);
        QColor* color = new QColor(col->getR(), col->getG(), col->getB());
        pen->setColor(*color);
        s_l->setPen(*pen);

        for (double t = 0.; t < 30.; t += 0.1) {
            ocean->setTemperature(t);

            ocean->recalculateEps();

            double kappa = ocean->_kappa_double();
            s_l->append(t, kappa);

            if (kappa > max) max = kappa;
            if (kappa < min) min = kappa;

            std::string write = dtos(_lamda[i]) + ", " + dtos(t) + ", " + dtos(kappa) + "\n";
            fout << write;

        }

        s_l->setName(QString::fromStdString(dtos(_lamda[i])));
        chart->addSeries(s_l);

        s_l->attachAxis(axisX);
        s_l->attachAxis(axisY);
    }

    fout.close();

    axisY->setRange(min - 0.15, max + 0.15);
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    QWidget* chartWindow = new QWidget(0);
    QVBoxLayout* layout = new QVBoxLayout(chartWindow);
    layout->addWidget(chartView);
    chartWindow->setLayout(layout);
    layout->activate();
    chartWindow->resize(800,500);
    chartWindow->show();
    delete ocean;
    return;
}

#endif // CORE_HPP

