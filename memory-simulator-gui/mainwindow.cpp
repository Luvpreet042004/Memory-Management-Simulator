#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QGraphicsTextItem>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
}

MainWindow::~MainWindow() {
    delete pm;
    delete ui;
}

void MainWindow::log(const QString &text) {
    ui->consoleLog->append(text);
}

void MainWindow::on_initButton_clicked()
{
    bool ok;
    size_t size = ui->memSizeEdit->text().toULongLong(&ok);

    if (!ok || size == 0) {
        QMessageBox::warning(this, "Error", "Invalid memory size");
        return;
    }

    if (pm) {
        delete pm;
        pm = nullptr;
    }

    try {
        pm = new PhysicalMemory(size);
    } catch (...) {
        QMessageBox::critical(this, "Error", "Failed to allocate physical memory");
        return;
    }

    total_cycles = 0;

    if (!ui->memoryView->scene()) {
        ui->memoryView->setScene(new QGraphicsScene(this));
    }

    log("Initialized memory: " + QString::number(size) + " bytes");

    updateStats();
    updateMemoryView();
}


void MainWindow::on_mallocButton_clicked() {
    if (!pm) { log("Initialize memory first"); return; }

    bool ok;
    size_t size = ui->allocSizeEdit->text().toULongLong(&ok);
    if (!ok || size == 0) return;

    size_t id = pm->allocate(size);
    if (id == 0) log("Allocation failed");
    else log("Allocated block id=" + QString::number(id));

    updateStats();
    updateMemoryView();
}

void MainWindow::on_freeButton_clicked() {
    if (!pm) return;

    bool ok;
    size_t id = ui->freeIdEdit->text().toULongLong(&ok);
    if (!ok) return;

    if (pm->deallocate(id)) log("Block " + QString::number(id) + " freed");
    else log("Invalid block id");

    updateStats();
    updateMemoryView();
}

void MainWindow::on_setStrategyButton_clicked() {
    if (!pm) return;

    QString strat = ui->strategyCombo->currentText();
    if (strat == "First Fit") pm->set_strategy(FitStrategy::FIRST_FIT);
    else if (strat == "Best Fit") pm->set_strategy(FitStrategy::BEST_FIT);
    else if (strat == "Worst Fit") pm->set_strategy(FitStrategy::WORST_FIT);

    log("Allocator set to " + strat);
}

void MainWindow::on_accessButton_clicked() {
    if (!pm) return;

    bool ok1, ok2;
    size_t id = ui->accessIdEdit->text().toULongLong(&ok1);
    size_t offset = ui->accessOffsetEdit->text().toULongLong(&ok2);
    if (!ok1 || !ok2) return;

    auto block = pm->get_block(id);
    if (offset >= block.size) { log("Offset out of bounds"); return; }

    size_t addr = block.start + offset;

    if (l1.access(addr)) {
        log("L1 hit (1 cycle)");
        total_cycles += 1;
    }
    else if (l2.access(addr)) {
        log("L2 hit (10 cycles)");
        total_cycles += 10;
    }
    else {
        log("Memory access (100 cycles)");
        total_cycles += 100;
    }

    updateStats();
}

void MainWindow::updateStats() {
    if (!pm) return;

    ui->memStatsLabel->setText(QString("Memory: Used %1 / %2")
                                   .arg(pm->get_used_memory())
                                   .arg(pm->get_total_memory()));

    ui->cacheStatsLabel->setText(QString("L1: %1 hits / %2 misses, L2: %3 hits / %4 misses")
                                     .arg(l1.get_hits()).arg(l1.get_misses())
                                     .arg(l2.get_hits()).arg(l2.get_misses()));

    ui->cycleLabel->setText("Total cycles: " + QString::number(total_cycles));
}

void MainWindow::on_setL1Button_clicked()
{
    bool ok1, ok2, ok3;
    size_t size  = ui->l1SizeEdit->text().toULongLong(&ok1);
    size_t block = ui->l1BlockEdit->text().toULongLong(&ok2);
    size_t assoc = ui->l1AssocEdit->text().toULongLong(&ok3);

    if (!ok1 || !ok2 || !ok3) {
        log("Invalid L1 cache parameters");
        return;
    }

    l1.configure(size, block, assoc);
    log("L1 cache configured");
    updateStats();
}

void MainWindow::on_setL2Button_clicked()
{
    bool ok1, ok2, ok3;
    size_t size  = ui->l2SizeEdit->text().toULongLong(&ok1);
    size_t block = ui->l2BlockEdit->text().toULongLong(&ok2);
    size_t assoc = ui->l2AssocEdit->text().toULongLong(&ok3);

    if (!ok1 || !ok2 || !ok3) {
        log("Invalid L2 cache parameters");
        return;
    }

    l2.configure(size, block, assoc);
    log("L2 cache configured");
    updateStats();
}



void MainWindow::updateMemoryView()
{
    if (!pm) return;

    auto scene = ui->memoryView->scene();
    scene->clear();

    const int viewWidth = 200;
    const int spacing = 2;                  // small gap between blocks
    double scale = 400.0 / pm->get_total_memory();

    // Draw free blocks
    for (const auto& b : pm->get_free_list()) {
        scene->addRect(
            0,
            b.start * scale,
            viewWidth,
            b.size * scale - spacing,
            QPen(QColor("#888888"), 1),          // gray border
            QBrush(QColor("#555555"))            // dark gray fill
            );
    }

    // Draw allocated blocks
    for (const auto& [id, b] : pm->get_allocated()) {
        auto rect = scene->addRect(
            0,
            b.start * scale,
            viewWidth,
            b.size * scale - spacing,
            QPen(QColor("#00ff99"), 2),          // bright border
            QBrush(QColor("#009966"))            // green fill
            );

        auto label = scene->addText(QString("ID %1").arg(id));
        label->setDefaultTextColor(Qt::white);
        label->setPos(5, b.start * scale);
    }

    scene->setSceneRect(scene->itemsBoundingRect());
}

void MainWindow::on_resetButton_clicked()
{
    delete pm;
    pm = nullptr;

    l1.reset();
    l2.reset();

    total_cycles = 0;

    ui->memSizeEdit->clear();
    ui->allocSizeEdit->clear();
    ui->freeIdEdit->clear();
    ui->accessIdEdit->clear();
    ui->accessOffsetEdit->clear();

    ui->consoleLog->clear();
    log("Simulation reset. Ready for new run.");

    updateStats();
    updateMemoryView();
}


