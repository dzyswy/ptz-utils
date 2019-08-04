#ifndef _PTZ_CLIENT_H
#define _PTZ_CLIENT_H

#include <iostream>
#include <boost/thread.hpp>
#include <opencv2/opencv.hpp>
#include <QtWidgets/QWidget>
#include <QStyleFactory>
#include <QMessageBox>
#include <QDebug.h>
#include <QImage>
#include <QTimer>
#include <QFile>
#include <QPixmap>
#include <QFileDialog>
#include <QDir>
#include "ui_ptz_client.h"
#include "ptz_track.h"

using namespace std;
using namespace cv;
using namespace boost;




class ptz_client : public QWidget
{
	Q_OBJECT

public:
	ptz_client(QWidget *parent = 0);
	~ptz_client();
	


private:
	Ui::ptz_clientClass ui;
	ptz_track *mtrack;
	int open_state;
	int track_mode;
	
private:
	void init_ui();
	void connect_signals();	
	
	static void track_ptz_pos_fresh(void *arg, void *data);
	
signals:
	void fresh_ptz_pos(int pan, int tilt, int zoom);	
	
private slots:
	void do_fresh_ptz_pos(int pan, int tilt, int zoom);

	void on_pushButton_open_clicked();
	void on_checkBox_datascreen_stateChanged(int arg1);
	void on_toolButton_UL_pressed();
	void on_toolButton_U_pressed();
	void on_toolButton_UR_pressed();
	void on_toolButton_L_pressed();
	void on_toolButton_R_pressed();
	void on_toolButton_DL_pressed();
	void on_toolButton_D_pressed();
	void on_toolButton_DR_pressed();
	void toolButton_pantilt_stop();
	void on_toolButton_home_pressed();
	void on_horizontalSlider_pan_speed_valueChanged(int value);
	void on_horizontalSlider_tilt_speed_valueChanged(int value);
	void on_horizontalSlider_pan_abs_valueChanged(int value);
	void on_horizontalSlider_tilt_abs_valueChanged(int value);
	void on_toolButton_tele_pressed();
	void on_toolButton_wide_pressed();
	void toolButton_zoom_stop();
	void on_horizontalSlider_zoom_speed_valueChanged(int value);
	void on_horizontalSlider_zoom_abs_valueChanged(int value);
	void on_toolButton_near_pressed();
	void on_toolButton_far_pressed();
	void toolButton_focus_stop();
	void on_horizontalSlider_focus_speed_valueChanged(int value);
	void on_horizontalSlider_focus_abs_valueChanged(int value);
	void on_pushButton_all_pos_clicked();
	
	void on_comboBox_track_mode_currentIndexChanged(int index);
	void on_comboBox_pid_id_currentIndexChanged(int index);
	void on_pushButton_set_pid_para_clicked();
	void on_horizontalSlider_pan_dst_valueChanged(int value);
	void on_horizontalSlider_tilt_dst_valueChanged(int value);
	void on_horizontalSlider_zoom_dst_valueChanged(int value);
	
	void on_pushButton_get_ptz_offset_clicked();
	void on_pushButton_set_ptz_offset_clicked();
	void on_pushButton_do_ptz_offset_clicked();
	void on_pushButton_set_ptz_scale_clicked();
	void on_comboBox_zoom_map_num_currentIndexChanged(int index);
	void on_pushButton_set_zoom_map_clicked();
	void on_pushButton_calc_ball_cord_clicked();
	
};








#endif 
