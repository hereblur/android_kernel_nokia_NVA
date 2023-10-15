// SPDX-License-Identifier: GPL-2.0
/*
 * File:shub_comm.c
 *
 * Copyright (C) 2018 Spreadtrum Communications Inc.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 */

#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include "shub_common.h"
#include "shub_core.h"
#include "shub_protocol.h"

static void shub_get_data(struct cmd_data *packet)
{
	switch (packet->subtype) {
	case SHUB_LOG_SUBTYPE:
		dev_info(&g_sensor->sensor_pdev->dev,
			 " [CM4]> :%s\n", packet->buff);
		break;

	case SHUB_DATA_SUBTYPE:
		g_sensor->data_callback(g_sensor, packet->buff,
					packet->length);
		break;

	case SHUB_CM4_OPERATE:
		memcpy(g_sensor->cm4_operate_data,
		       packet->buff,
		       sizeof(g_sensor->cm4_operate_data));
		break;

	case SHUB_SEND_DEBUG_DATA:
		memcpy(g_sensor->log_control.debug_data,
		       packet->buff,
		       sizeof(g_sensor->log_control.debug_data));
		break;

	case SHUB_GET_MAG_OFFSET:
		g_sensor->save_mag_offset(g_sensor, packet->buff,
					packet->length);
		break;

	case SHUB_GET_CALIBRATION_DATA_SUBTYPE:
	case SHUB_GET_ENABLE_LIST_SUBTYPE:
	case SHUB_GET_ACCELERATION_RAWDATA_SUBTYPE:
	case SHUB_GET_MAGNETIC_RAWDATA_SUBTYPE:
	case SHUB_GET_GYROSCOPE_RAWDATA_SUBTYPE:
	case SHUB_GET_LIGHT_RAWDATA_SUBTYPE:
	case SHUB_GET_PROXIMITY_RAWDATA_SUBTYPE:
	case SHUB_GET_FWVERSION_SUBTYPE:
	case SHUB_GET_SENSORINFO_SUBTYPE:
		g_sensor->readcmd_callback(g_sensor, packet->buff,
					packet->length);
		break;

	case SHUB_RESPONSE_SUBTYPE:
		g_sensor->resp_cmdstatus_callback(g_sensor, packet->buff,
						  packet->length);
		break;

	case SHUB_SET_TIMESYNC_SUBTYPE:
		g_sensor->cm4_read_callback(g_sensor,
			packet->subtype,
			packet->buff,
			packet->length);
		break;

	case DYNAMIC_SEND_DATA_TO_AP_SUBTYPE:
		g_sensor->dynamic_data_get.type = packet->type;
		g_sensor->dynamic_data_get.length = packet->length;
		memcpy(g_sensor->dynamic_data_get.customer_data,
		       packet->buff,
		       packet->length);
		g_sensor->dynamic_read(g_sensor);
		break;

	default:
		break;
	}
}

void shub_dispatch(struct cmd_data *packet)
{
	if (packet)
		shub_get_data(packet);
}

MODULE_DESCRIPTION("Sensorhub dispatch support");
MODULE_LICENSE("GPL v2");
