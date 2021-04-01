#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_UnfoldingShapes.h"

class UnfoldingShapes : public QMainWindow
{
    Q_OBJECT

public:
    UnfoldingShapes(QWidget *parent = Q_NULLPTR);

private:
    Ui::UnfoldingShapesClass ui;
};
