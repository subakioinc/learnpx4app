# QGC Custom Widget
 * Widgets -> Custom Command
 * 'Load Custom Qml file' 버튼
 * 아래 파일 : mycustom.qml
 * mysubmodule_subscribe 실행
```console
> mysubmodule_subscribe start
```

```qml
import QtQuick 2.2

import QGroundControl.Controls      1.0
import QGroundControl.FactSystem    1.0
import QGroundControl.FactControls  1.0
import QGroundControl.Palette       1.0
import QGroundControl.ScreenTools   1.0
import QGroundControl.Controllers   1.0

Rectangle {
    anchors.fill:   parent
    color:          qgcPal.window

    CustomCommandWidgetController {
        id:         controller
        factPanel:  panel
    }

    QGCPalette { id: qgcPal; colorGroupEnabled: enabled }

    Column {
        spacing: ScreenTools.defaultFontPixelHeight

        QGCButton {
            text: "COMMAND_LONG : 20010 command"
            // Arguments to CustomCommandWidgetController::sendCommand (MAVLink COMMAND_LONG)
            //   command id
            //   component id
            //   confirmation
            //   param 1-7
            onClicked: controller.sendCommand(20010, 50, 0, 1, 0, 0, 0, 0, 0, 0)
        }

        // The FactTextField control is bound to the specified parameter. Note that there is no validation.
        FactTextField {
            // The -1 signals default component id.
            // You can replace it with a specific component id if you like
            fact: controller.getParameterFact(-1, "MAV_SYS_ID")
        }
    }
}
```
# mavlink module
 * mavlink_receiver.h
```c++
#include <uORB/topics/mission_subak.h>

orb_advert_t _mission_subak_pub{nullptr};
```

```
#include <uORB/topics/mission_subak.h>

uORB::Publication<misson_subak_s>	_mission_subak_pub{ORB_ID(mission_subak)};
```
 * mavlink_receiver.cpp
```c++
template <class T>
void MavlinkReceiver::handle_message_command_both(mavlink_message_t *msg, const T &cmd_mavlink,
		const vehicle_command_s &vehicle_command)
{
	bool target_ok = evaluate_target_ok(cmd_mavlink.command, cmd_mavlink.target_system, cmd_mavlink.target_component);

	bool send_ack = true;
	uint8_t result = vehicle_command_ack_s::VEHICLE_RESULT_ACCEPTED;

	if(cmd_mavlink.command == 2011){
		int instance_id = 0;
		struct mission_subak_s raw = {};
		raw.x = 33;
		raw.y = 22;
		orb_publish_auto(ORB_ID(mission_subak), &_mission_subak_pub, &raw, &instance_id, ORB_PRIO_HIGH);
		PX4_INFO("I received command from QGC!!! 2011");
	}
	if (!target_ok) {
		acknowledge(msg->sysid, msg->compid, cmd_mavlink.command, vehicle_command_ack_s::VEHICLE_RESULT_FAILED);
		return;
	}
```
```c++
template <class T>
void MavlinkReceiver::handle_message_command_both(mavlink_message_t *msg, const T &cmd_mavlink,
		const vehicle_command_s &vehicle_command)
{
	bool target_ok = evaluate_target_ok(cmd_mavlink.command, cmd_mavlink.target_system, cmd_mavlink.target_component);

	bool send_ack = true;
	uint8_t result = vehicle_command_ack_s::VEHICLE_RESULT_ACCEPTED;

	if(cmd_mavlink.command == 2011){
//		int instance_id = 0;
		struct mission_subak_s raw = {};
		raw.x = 33;
		raw.y = 22;
		_mission_subak_s.publish(raw); //orb_publish_auto(ORB_ID(mission_subak), &_mission_subak_pub, &raw, &instance_id, ORB_PRIO_HIGH);
		PX4_INFO("I received command from QGC!!! 2011");
	}
	if (!target_ok) {
		acknowledge(msg->sysid, msg->compid, cmd_mavlink.command, vehicle_command_ack_s::VEHICLE_RESULT_FAILED);
		return;
	}
```

