#ifndef OSCSHAPEUI_H
#define OSCSHAPEUI_H

#include <QMainWindow>

namespace Ui {
class OscShapeUI;
}

class OscShapeUI : public QMainWindow
{
    Q_OBJECT

public:
    explicit OscShapeUI(QWidget *parent = 0);
    ~OscShapeUI();

private slots:

    void on_PitchDial_A_sliderMoved(int position);

    void on_pushButton_clicked();

private:
    Ui::OscShapeUI *ui;
};

#endif // OSCSHAPEUI_H
