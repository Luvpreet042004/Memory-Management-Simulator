#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "../include/allocator/physical_memory.h"
#include "../include/cache/cache.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_initButton_clicked();
    void on_mallocButton_clicked();
    void on_freeButton_clicked();
    void on_setStrategyButton_clicked();
    void on_setL1Button_clicked();
    void on_setL2Button_clicked();
    void on_accessButton_clicked();
    void on_resetButton_clicked();

private:
    Ui::MainWindow *ui;

    PhysicalMemory *pm = nullptr;
    Cache l1;
    Cache l2;

    size_t total_cycles = 0;

    void log(const QString &text);
    void updateStats();
    void updateMemoryView();
};

#endif
