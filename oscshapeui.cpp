#include "oscshapeui.h"
#include "ui_oscshapeui.h"

OscShapeUI::OscShapeUI(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::OscShapeUI)
{
    ui->setupUi(this);
}

OscShapeUI::~OscShapeUI()
{
    delete ui;
}

void OscShapeUI::on_PitchDial_A_sliderMoved(int position)
{

}

void OscShapeUI::on_pushButton_clicked()
{
        close();
}
