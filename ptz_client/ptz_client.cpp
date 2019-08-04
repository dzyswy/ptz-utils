#include "ptz_client.h"

ptz_client::ptz_client(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	open_state = 0;
	track_mode = 0;
	mtrack = new ptz_track(track_ptz_pos_fresh, this);
	
	mtrack->load_config("config.ini");
	init_ui();
	connect_signals();
}

ptz_client::~ptz_client()
{
	mtrack->save_config("config.ini");
	delete mtrack;
}

void ptz_client::init_ui()
{
	on_comboBox_pid_id_currentIndexChanged(0);
	
	ui.spinBox_pan_offset->setValue(mtrack->get_pos_offset(PTZ_PID_PAN));
	ui.spinBox_tilt_offset->setValue(mtrack->get_pos_offset(PTZ_PID_TILT));
	ui.spinBox_zoom_offset->setValue(mtrack->get_pos_offset(PTZ_PID_ZOOM));
	
	ui.doubleSpinBox_pan_scale->setValue(mtrack->get_pos_scale(PTZ_PID_PAN));
	ui.doubleSpinBox_tilt_scale->setValue(mtrack->get_pos_scale(PTZ_PID_TILT));
	
	vector<pair<int, float> > zoom_map;
	mtrack->get_zoom_map(zoom_map);
	int zoom_map_num = zoom_map.size();
	
	if (zoom_map_num > 1)
	{
		on_comboBox_zoom_map_num_currentIndexChanged(zoom_map_num - 2);
		ui.spinBox_item0_zoom_pos->setValue(zoom_map[0].first);
		ui.doubleSpinBox_item0_zoom_dist->setValue(zoom_map[0].second);
		ui.spinBox_item1_zoom_pos->setValue(zoom_map[1].first);
		ui.doubleSpinBox_item1_zoom_dist->setValue(zoom_map[1].second);
	}
	
	if (zoom_map_num > 2) {
		ui.spinBox_item2_zoom_pos->setValue(zoom_map[2].first);
		ui.doubleSpinBox_item2_zoom_dist->setValue(zoom_map[2].second);
	}
	
	
	if (zoom_map_num > 3)
	{
		ui.spinBox_item3_zoom_pos->setValue(zoom_map[3].first);
		ui.doubleSpinBox_item3_zoom_dist->setValue(zoom_map[3].second);
	}	
	if (zoom_map_num > 4)
	{
		ui.spinBox_item4_zoom_pos->setValue(zoom_map[4].first);
		ui.doubleSpinBox_item4_zoom_dist->setValue(zoom_map[4].second);
	}	
	if (zoom_map_num > 5)
	{
		ui.spinBox_item5_zoom_pos->setValue(zoom_map[5].first);
		ui.doubleSpinBox_item5_zoom_dist->setValue(zoom_map[5].second);
	}	
	if (zoom_map_num > 6)
	{
		ui.spinBox_item6_zoom_pos->setValue(zoom_map[6].first);
		ui.doubleSpinBox_item6_zoom_dist->setValue(zoom_map[6].second);
	}	
	if (zoom_map_num > 7)
	{
		ui.spinBox_item7_zoom_pos->setValue(zoom_map[7].first);
		ui.doubleSpinBox_item7_zoom_dist->setValue(zoom_map[7].second);
	}	
	
	
}

void ptz_client::track_ptz_pos_fresh(void *arg, void *data)
{
	ptz_client *client = (ptz_client *)arg;
	struct ptz_positon *pos = (struct ptz_positon *)data;
	emit client->fresh_ptz_pos(pos->pan, pos->tilt, pos->zoom);
}

void ptz_client::connect_signals()
{
	connect(ui.toolButton_UL, SIGNAL(released()), this, SLOT(toolButton_pantilt_stop()));
	connect(ui.toolButton_U, SIGNAL(released()), this, SLOT(toolButton_pantilt_stop()));
	connect(ui.toolButton_UR, SIGNAL(released()), this, SLOT(toolButton_pantilt_stop()));
	connect(ui.toolButton_L, SIGNAL(released()), this, SLOT(toolButton_pantilt_stop()));
	connect(ui.toolButton_R, SIGNAL(released()), this, SLOT(toolButton_pantilt_stop()));
	connect(ui.toolButton_DL, SIGNAL(released()), this, SLOT(toolButton_pantilt_stop()));
	connect(ui.toolButton_D, SIGNAL(released()), this, SLOT(toolButton_pantilt_stop()));
	connect(ui.toolButton_DR, SIGNAL(released()), this, SLOT(toolButton_pantilt_stop()));
	
	connect(ui.toolButton_tele, SIGNAL(released()), this, SLOT(toolButton_zoom_stop()));
	connect(ui.toolButton_wide, SIGNAL(released()), this, SLOT(toolButton_zoom_stop()));
	
	connect(ui.toolButton_near, SIGNAL(released()), this, SLOT(toolButton_focus_stop()));
	connect(ui.toolButton_far, SIGNAL(released()), this, SLOT(toolButton_focus_stop()));
	
	connect(this, SIGNAL(fresh_ptz_pos(int, int, int)), this, SLOT(do_fresh_ptz_pos(int, int, int)));
}

void ptz_client::do_fresh_ptz_pos(int pan, int tilt, int zoom)
{
	ui.horizontalSlider_pan_now_pos->setValue(pan);
	ui.spinBox_pan_now_pos->setValue(pan);
	
	ui.horizontalSlider_tilt_now_pos->setValue(tilt);
	ui.spinBox_tilt_now_pos->setValue(tilt);
	
	ui.horizontalSlider_zoom_now_pos->setValue(zoom);
	ui.spinBox_zoom_now_pos->setValue(zoom);
}

void ptz_client::on_pushButton_open_clicked()
{
	string dev_name = ui.comboBox_dev->currentText().toStdString();

	if (!open_state)
	{
		int ret = mtrack->open_ptz(dev_name.c_str());
		if (ret < 0)
			return;
		
		ui.pushButton_open->setText(QString::fromLocal8Bit("关闭"));
		open_state = 1;
	}	
	else
	{
		mtrack->close_ptz();
		ui.pushButton_open->setText(QString::fromLocal8Bit("连接"));
		open_state = 0;
	}
}

void ptz_client::on_checkBox_datascreen_stateChanged(int arg1)
{
    switch (arg1)
    {
    case Qt::Unchecked:
        mtrack->set_datascreen_off();
        break;
    case Qt::Checked:
        mtrack->set_datascreen_on();
        break;
    }
}

void ptz_client::on_toolButton_UL_pressed()
{
	int pan_speed = ui.horizontalSlider_pan_speed->value();
	int tilt_speed = ui.horizontalSlider_tilt_speed->value();
	mtrack->set_pantilt_upleft(pan_speed, tilt_speed);
}


void ptz_client::on_toolButton_U_pressed()
{
	int pan_speed = ui.horizontalSlider_pan_speed->value();
	int tilt_speed = ui.horizontalSlider_tilt_speed->value();
	mtrack->set_pantilt_up(pan_speed, tilt_speed);
}


void ptz_client::on_toolButton_UR_pressed()
{
	int pan_speed = ui.horizontalSlider_pan_speed->value();
	int tilt_speed = ui.horizontalSlider_tilt_speed->value();
	mtrack->set_pantilt_upright(pan_speed, tilt_speed);
}

void ptz_client::on_toolButton_L_pressed()
{
	int pan_speed = ui.horizontalSlider_pan_speed->value();
	int tilt_speed = ui.horizontalSlider_tilt_speed->value();
	mtrack->set_pantilt_left(pan_speed, tilt_speed);
}

void ptz_client::on_toolButton_R_pressed()
{
	int pan_speed = ui.horizontalSlider_pan_speed->value();
	int tilt_speed = ui.horizontalSlider_tilt_speed->value();
	mtrack->set_pantilt_right(pan_speed, tilt_speed);
}

void ptz_client::on_toolButton_DL_pressed()
{
	int pan_speed = ui.horizontalSlider_pan_speed->value();
	int tilt_speed = ui.horizontalSlider_tilt_speed->value();
	mtrack->set_pantilt_downleft(pan_speed, tilt_speed);
}


void ptz_client::on_toolButton_D_pressed()
{
	int pan_speed = ui.horizontalSlider_pan_speed->value();
	int tilt_speed = ui.horizontalSlider_tilt_speed->value();
	mtrack->set_pantilt_down(pan_speed, tilt_speed);
}

void ptz_client::on_toolButton_DR_pressed()
{
	int pan_speed = ui.horizontalSlider_pan_speed->value();
	int tilt_speed = ui.horizontalSlider_tilt_speed->value();
	mtrack->set_pantilt_downright(pan_speed, tilt_speed);
}

void ptz_client::toolButton_pantilt_stop()
{
	int pan_speed = ui.horizontalSlider_pan_speed->value();
	int tilt_speed = ui.horizontalSlider_tilt_speed->value();
	mtrack->set_pantilt_stop(pan_speed, tilt_speed);
}

void ptz_client::on_toolButton_home_pressed()
{
	mtrack->set_pantilt_home();
}

void ptz_client::on_horizontalSlider_pan_speed_valueChanged(int value)
{
	ui.spinBox_pan_speed->setValue(value);
}

void ptz_client::on_horizontalSlider_tilt_speed_valueChanged(int value)
{
	ui.spinBox_tilt_speed->setValue(value);
}

void ptz_client::on_horizontalSlider_pan_abs_valueChanged(int value)
{
	int pan_speed = ui.horizontalSlider_pan_speed->value();
	int tilt_speed = ui.horizontalSlider_tilt_speed->value();
	int pan_abs = ui.horizontalSlider_pan_abs->value();
	int tilt_abs = ui.horizontalSlider_tilt_abs->value();
	mtrack->set_pantilt_absolute_position(pan_speed, tilt_speed, pan_abs, tilt_abs);
	ui.spinBox_pan_abs->setValue(pan_abs);
}

void ptz_client::on_horizontalSlider_tilt_abs_valueChanged(int value)
{
	int pan_speed = ui.horizontalSlider_pan_speed->value();
	int tilt_speed = ui.horizontalSlider_tilt_speed->value();
	int pan_abs = ui.horizontalSlider_pan_abs->value();
	int tilt_abs = ui.horizontalSlider_tilt_abs->value();
	mtrack->set_pantilt_absolute_position(pan_speed, tilt_speed, pan_abs, tilt_abs);
	ui.spinBox_tilt_abs->setValue(tilt_abs);
}

void ptz_client::on_toolButton_tele_pressed()
{
	int speed = ui.horizontalSlider_zoom_speed->value();
	mtrack->set_zoom_tele_speed(speed);
}

void ptz_client::on_toolButton_wide_pressed()
{
	int speed = ui.horizontalSlider_zoom_speed->value();
	mtrack->set_zoom_wide_speed(speed);
}

void ptz_client::toolButton_zoom_stop()
{
	mtrack->set_zoom_stop();
}

void ptz_client::on_horizontalSlider_zoom_speed_valueChanged(int value)
{
	ui.spinBox_zoom_speed->setValue(value);
}

void ptz_client::on_horizontalSlider_zoom_abs_valueChanged(int value)
{
	mtrack->set_zoom_value(value);
	ui.spinBox_zoom_abs->setValue(value);
}

void ptz_client::on_toolButton_near_pressed()
{
	int speed = ui.horizontalSlider_focus_speed->value();
	mtrack->set_focus_near_speed(speed);
}

void ptz_client::on_toolButton_far_pressed()
{
	int speed = ui.horizontalSlider_focus_speed->value();
	mtrack->set_focus_far_speed(speed);
}

void ptz_client::toolButton_focus_stop()
{
	mtrack->set_focus_stop();
}

void ptz_client::on_horizontalSlider_focus_speed_valueChanged(int value)
{
	ui.spinBox_focus_speed->setValue(value);
}

void ptz_client::on_horizontalSlider_focus_abs_valueChanged(int value)
{
	mtrack->set_focus_value(value);
	ui.spinBox_focus_abs->setValue(value);
}

void ptz_client::on_pushButton_all_pos_clicked()
{
	int ret = 0;
	int pan_pos = 0, tilt_pos = 0, zoom_pos = 0, focus_pos = 0;
	ret = mtrack->get_pantilt_position(&pan_pos, &tilt_pos);
	if (ret < 0)
		return;
	ui.spinBox_pan_pos->setValue(pan_pos);
	ui.spinBox_tilt_pos->setValue(tilt_pos);
	ret = mtrack->get_zoom_value(&zoom_pos);
	if (ret < 0)
		return;
	ui.spinBox_zoom_pos->setValue(zoom_pos);
	ret = mtrack->get_focus_value(&focus_pos);
	if (ret < 0)
		return;
	ui.spinBox_focus_pos->setValue(focus_pos);
	
//	ostringstream all_pos;
//	all_pos << "pan: " << pan_pos << ", ";
//	all_pos << "tilt: " << tilt_pos << ", ";
//	all_pos << "zoom: " << zoom_pos << ", ";
//	all_pos << "focus: " << focus_pos << " ";
//	ui.lineEdit_all_pos->setText(QString::fromStdString(all_pos.str()));
}


void ptz_client::on_comboBox_track_mode_currentIndexChanged(int index)
{
	mtrack->set_track_mode(index);
}

void ptz_client::on_comboBox_pid_id_currentIndexChanged(int index)
{
	double kp = mtrack->get_kp(index);
	double ki = mtrack->get_ki(index);
	double kd = mtrack->get_kd(index);
	int dead_zone = mtrack->get_dead_zone(index);
	int min_limit = mtrack->get_min_limit(index);
	int max_limit = mtrack->get_max_limit(index);
	
	ui.doubleSpinBox_pan_kp->setValue(kp);
	ui.doubleSpinBox_pan_ki->setValue(ki);
	ui.doubleSpinBox_pan_kd->setValue(kd);
	ui.spinBox_pan_dead_zone->setValue(dead_zone);
	ui.spinBox_pan_min_limit->setValue(min_limit);
	ui.spinBox_pan_max_limit->setValue(max_limit);
}

void ptz_client::on_pushButton_set_pid_para_clicked()
{
	int pid_id = ui.comboBox_pid_id->currentIndex();
	double kp =  ui.doubleSpinBox_pan_kp->value();
	double ki =  ui.doubleSpinBox_pan_ki->value();
	double kd =  ui.doubleSpinBox_pan_kd->value();
	int dead_zone = ui.spinBox_pan_dead_zone->value();
	int min_limit = ui.spinBox_pan_min_limit->value();
	int max_limit = ui.spinBox_pan_max_limit->value();
	
	mtrack->set_kp(pid_id, kp);
	mtrack->set_ki(pid_id, ki);
	mtrack->set_kd(pid_id, kd);
	mtrack->set_dead_zone(pid_id, (float)dead_zone);
	mtrack->set_min_limit(pid_id, (float)min_limit);
	mtrack->set_max_limit(pid_id, (float)max_limit);
}

void ptz_client::on_horizontalSlider_pan_dst_valueChanged(int value)
{
	mtrack->set_target(PTZ_PID_PAN, (float)value);
	ui.spinBox_pan_dst->setValue(value);
}

void ptz_client::on_horizontalSlider_tilt_dst_valueChanged(int value)
{
	mtrack->set_target(PTZ_PID_TILT, (float)value);
	ui.spinBox_tilt_dst->setValue(value);
}

void ptz_client::on_horizontalSlider_zoom_dst_valueChanged(int value)
{
	mtrack->set_target(PTZ_PID_ZOOM, (float)value);
	ui.spinBox_zoom_dst->setValue(value);
}

void ptz_client::on_pushButton_get_ptz_offset_clicked()
{
	int ret;
	int pan_pos = 0, tilt_pos = 0, zoom_pos = 0, focus_pos = 0;
	ret = mtrack->get_pantilt_position(&pan_pos, &tilt_pos);
	if (ret < 0)
		return;
	ui.spinBox_pan_offset->setValue(pan_pos);
	ui.spinBox_tilt_offset->setValue(tilt_pos);
	ret = mtrack->get_zoom_value(&zoom_pos);
	if (ret < 0)
		return;
	ui.spinBox_zoom_offset->setValue(zoom_pos);
}

void ptz_client::on_pushButton_set_ptz_offset_clicked()
{
	int pan_pos = ui.spinBox_pan_offset->value();
	int tilt_pos = ui.spinBox_tilt_offset->value();
	int zoom_pos = ui.spinBox_zoom_offset->value();
	
	mtrack->set_pos_offset(PTZ_PID_PAN, pan_pos);
	mtrack->set_pos_offset(PTZ_PID_TILT, tilt_pos);
	mtrack->set_pos_offset(PTZ_PID_ZOOM, zoom_pos);
}

void ptz_client::on_pushButton_do_ptz_offset_clicked()
{
	mtrack->move_ptz_origin(20, 20);
}

void ptz_client::on_pushButton_set_ptz_scale_clicked()
{
	float pan_scale = ui.doubleSpinBox_pan_scale->value();
	float tilt_scale = ui.doubleSpinBox_tilt_scale->value();
	
	mtrack->set_pos_scale(PTZ_PID_PAN, pan_scale);
	mtrack->set_pos_scale(PTZ_PID_TILT, tilt_scale);
}


void ptz_client::on_comboBox_zoom_map_num_currentIndexChanged(int index)
{
	int map_num = index + 2;//ui.comboBox_zoom_map_num->count();
	if (map_num > 2) {
		ui.spinBox_item2_zoom_pos->setEnabled(true);
		ui.doubleSpinBox_item2_zoom_dist->setEnabled(true);
	} else {
		ui.spinBox_item2_zoom_pos->setEnabled(false);
		ui.doubleSpinBox_item2_zoom_dist->setEnabled(false);
	}
	
	if (map_num > 3) {
		ui.spinBox_item3_zoom_pos->setEnabled(true);
		ui.doubleSpinBox_item3_zoom_dist->setEnabled(true);
	} else {
		ui.spinBox_item3_zoom_pos->setEnabled(false);
		ui.doubleSpinBox_item3_zoom_dist->setEnabled(false);
	}
	
	if (map_num > 4) {
		ui.spinBox_item4_zoom_pos->setEnabled(true);
		ui.doubleSpinBox_item4_zoom_dist->setEnabled(true);
	} else {
		ui.spinBox_item4_zoom_pos->setEnabled(false);
		ui.doubleSpinBox_item4_zoom_dist->setEnabled(false);
	}	
	
	if (map_num > 5) {
		ui.spinBox_item5_zoom_pos->setEnabled(true);
		ui.doubleSpinBox_item5_zoom_dist->setEnabled(true);
	} else {
		ui.spinBox_item5_zoom_pos->setEnabled(false);
		ui.doubleSpinBox_item5_zoom_dist->setEnabled(false);
	}	
		
	if (map_num > 6) {
		ui.spinBox_item6_zoom_pos->setEnabled(true);
		ui.doubleSpinBox_item6_zoom_dist->setEnabled(true);
	} else {
		ui.spinBox_item6_zoom_pos->setEnabled(false);
		ui.doubleSpinBox_item6_zoom_dist->setEnabled(false);
	}

	if (map_num > 7) {
		ui.spinBox_item7_zoom_pos->setEnabled(true);
		ui.doubleSpinBox_item7_zoom_dist->setEnabled(true);
	} else {
		ui.spinBox_item7_zoom_pos->setEnabled(false);
		ui.doubleSpinBox_item7_zoom_dist->setEnabled(false);
	}
}


void ptz_client::on_pushButton_set_zoom_map_clicked()
{
	int map_num = ui.comboBox_zoom_map_num->currentIndex() + 2;
	pair<int, float> item[8];
	item[0].first = ui.spinBox_item0_zoom_pos->value();
	item[0].second = ui.doubleSpinBox_item0_zoom_dist->value();
	item[1].first = ui.spinBox_item1_zoom_pos->value();
	item[1].second = ui.doubleSpinBox_item1_zoom_dist->value();
	item[2].first = ui.spinBox_item2_zoom_pos->value();
	item[2].second = ui.doubleSpinBox_item2_zoom_dist->value();
	item[3].first = ui.spinBox_item3_zoom_pos->value();
	item[3].second = ui.doubleSpinBox_item3_zoom_dist->value();
	item[4].first = ui.spinBox_item4_zoom_pos->value();
	item[4].second = ui.doubleSpinBox_item4_zoom_dist->value();
	item[5].first = ui.spinBox_item5_zoom_pos->value();
	item[5].second = ui.doubleSpinBox_item5_zoom_dist->value();
	item[6].first = ui.spinBox_item6_zoom_pos->value();
	item[6].second = ui.doubleSpinBox_item6_zoom_dist->value();
	item[7].first = ui.spinBox_item7_zoom_pos->value();
	item[7].second = ui.doubleSpinBox_item7_zoom_dist->value();

	vector<pair<int, float> > zoom_map;
	for (int i = 0; i < map_num; i++)
	{
		zoom_map.push_back(item[i]);
	}	
	
	mtrack->set_zoom_map(zoom_map);
}


void ptz_client::on_pushButton_calc_ball_cord_clicked()
{
	int pan_pos = 0, tilt_pos = 0, zoom_pos = 0;
	mtrack->get_pantilt_position(&pan_pos, &tilt_pos);
	mtrack->get_zoom_value(&zoom_pos);
	
	float azimuth_angle = 0, zenith_angle = 0, radius = 0;
	
	mtrack->pan_pos_to_azimuth_angle(pan_pos, azimuth_angle);
	mtrack->tilt_pos_to_zenith_angle(tilt_pos, zenith_angle);
	mtrack->zoom_pos_to_radius(zoom_pos, radius);
	
	ui.doubleSpinBox_azimuth_angle->setValue(azimuth_angle);
	ui.doubleSpinBo_zenith_angle->setValue(zenith_angle);
	ui.doubleSpinBox_radius->setValue(radius);
}






