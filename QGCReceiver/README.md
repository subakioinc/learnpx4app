# QGC Custom Widget
 * Widgets -> Custom Command
 * 'Load Custom Qml file' 버튼
 * 아래 파일 : mycustom.qml

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
            text: "Set Home to current position"
            // Arguments to CustomCommandWidgetController::sendCommand (MAVLink COMMAND_LONG)
            //   command id
            //   component id
            //   confirmation
            //   param 1-7
            onClicked: controller.sendCommand(179, 50, 0, 1, 0, 0, 0, 0, 0, 0)
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
 * mavlink_receiver.cpp
```c++
//추가
#include <px4_config.h>
#include <px4_tasks.h>
#include <px4_posix.h>
#include <unistd.h>
#include <stdio.h>
#include <poll.h>
#include <string.h>
#include <math.h>
#include <uORB/uORB.h>
#include <uORB/topics/subak_info.h>
//

void MavlinkReceiver::handle_message_command_both(mavlink_message_t *msg, const T &cmd_mavlink,
		const vehicle_command_s &vehicle_command)
{
	bool target_ok = evaluate_target_ok(cmd_mavlink.command, cmd_mavlink.target_system, cmd_mavlink.target_component);

	bool send_ack = true;
	uint8_t result = vehicle_command_ack_s::VEHICLE_RESULT_ACCEPTED;

	// 
	if(cmd_mavlink.command == 20010) {
		PX4_INFO("I got message");
	    struct subak_info_s raw;
    	memset(&raw, 0, sizeof(raw));
	    orb_advert_t subak_pub=orb_advertise(ORB_ID(subak_info), &raw);
		raw.x = 100;
		raw.y = 200;
		orb_publish(ORB_ID(subak_info), subak_pub, &raw);
		return ;
	}
    //...
}   
```
