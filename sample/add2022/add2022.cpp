/****************************************************************************
 *
 *   Copyright (c) 2022= ADD Development Team. All rights reserved.
 *
 ****************************************************************************/

/**
 * @file add2022.cpp
 * @author Jeyong Shin
 *
 * Driver for the Add2022 Sensor connected via I2C.
 */

#include <fcntl.h>
#include <math.h>
#include <poll.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

#include <board_config.h>
#include <containers/Array.hpp>
#include <drivers/device/device.h>
#include <drivers/device/i2c.h>
#include <drivers/drv_hrt.h>
#include <perf/perf_counter.h>
#include <px4_platform_common/px4_config.h>
#include <px4_platform_common/getopt.h>
#include <px4_platform_common/module_params.h>
#include <px4_platform_common/module.h>
#include <px4_platform_common/i2c_spi_buses.h>
//----------------------------------------------------------------

#include <uORB/uORB.h>
#include <uORB/topics/distance_sensor.h>

using namespace time_literals;

/* Configuration Constants */
#define ADD2022_BASE_ADDR                        0x70   // 7-bit address is 0x70 = 112. 8-bit address is 0xE0 = 224.
#define ADD2022_MIN_ADDR                         0x5A   // 7-bit address is 0x5A = 90.  8-bit address is 0xB4 = 180.
#define ADD2022_BUS_SPEED                        100000 // 100kHz bus speed.

/* MB12xx Registers addresses */
#define ADD2022_TAKE_RANGE_REG                   0x51 // Measure range Register.
#define ADD2022_SET_ADDRESS_1                    0xAA // Change address 1 Register.
#define ADD2022_SET_ADDRESS_2                    0xA5 // Change address 2 Register.

/* Device limits */
#define ADD2022_MIN_DISTANCE                     (0.20f)
#define ADD2022_MAX_DISTANCE                     (7.65f)

#define ADD2022_MEASURE_INTERVAL                 100_ms // 60ms minimum for one sonar.
#define ADD2022_INTERVAL_BETWEEN_SUCCESIVE_FIRES 100_ms // 30ms minimum between each sonar measurement (watch out for interference!).

class ADD2022 : public device::I2C, public ModuleParams, public I2CSPIDriver<ADD2022>
{
public:
	ADD2022(I2CSPIBusOption bus_option, const int bus, int bus_frequency, int address);
	virtual ~ADD2022();

	static I2CSPIDriverBase *instantiate(const BusCLIArguments &cli, const BusInstanceIterator &iterator,
					     int runtime_instance);
	static void print_usage();

	virtual int init() override;

	/**
	 * Diagnostics - print some basic information about the driver.
	 */
	void print_status() override;

	/**
	 * Sets a new device address.
	 * @param address The new sensor address to be set: 200-224 even addresses only.
	 * @return Returns PX4_OK iff successful, PX4_ERROR otherwise.
	 */
	int set_address(const uint8_t address = ADD2022_BASE_ADDR);

	/**
	 * Initialise the automatic measurement state machine and start it.
	 *
	 * @note This function is called at open and error time.  It might make sense
	 *       to make it more aggressive about resetting the bus in case of errors.
	 */
	void start();

	/**
	 * Perform a poll cycle; collect from the previous measurement
	 * and start a new one.
	 */
	void RunImpl();

protected:
	void custom_method(const BusCLIArguments &cli) override;

private:

	/**
	 * Collects the most recent sensor measurement data from the i2c bus.
	 */
	int collect();

	/**
	 * Sends an i2c measure command to start the next sonar ping.
	 */
	int measure();

	int _measure_interval{ADD2022_MEASURE_INTERVAL};	// Initialize the measure interval for a single sensor.

	orb_advert_t _distance_sensor_topic{nullptr};

	perf_counter_t _comms_error{perf_alloc(PC_ELAPSED, "mb12xx_comms_error")};
	perf_counter_t _sample_perf{perf_alloc(PC_COUNT, "mb12xx_sample_perf")};
};

ADD2022::ADD2022(I2CSPIBusOption bus_option, const int bus, int bus_frequency, int address) :
	I2C(DRV_DIST_DEVTYPE_ADD2022, MODULE_NAME, bus, address, bus_frequency),
	ModuleParams(nullptr),
	I2CSPIDriver(MODULE_NAME, px4::device_bus_to_wq(get_device_id()), bus_option, bus, address)
{
	set_device_type(DRV_DIST_DEVTYPE_ADD2022);
}

ADD2022::~ADD2022()
{
	// Unadvertise the distance sensor topic.
	if (_distance_sensor_topic != nullptr) {
		orb_unadvertise(_distance_sensor_topic);
	}

	// Free perf counters.
	perf_free(_comms_error);
	perf_free(_sample_perf);
}

int
ADD2022::collect()
{
	perf_begin(_sample_perf);
	uint8_t val[2] = {};

	// Set the sensor i2c adress for the active cycle.
	set_device_address(ADD2022_BASE_ADDR);

	// Transfer data from the bus.
	int ret_val = transfer(nullptr, 0, &val[0], 2);

	if (ret_val < 0) {
		PX4_ERR("read failed, address: 0x%02X", get_device_address());
		perf_count(_comms_error);
		perf_end(_sample_perf);
		return ret_val;
	}

	uint16_t distance_cm = val[0] << 8 | val[1];
	float distance_m = static_cast<float>(distance_cm) * 1e-2f;

	distance_sensor_s report;
	report.current_distance = distance_m;
	report.device_id        = get_device_id();
	report.max_distance     = ADD2022_MAX_DISTANCE;
	report.min_distance     = ADD2022_MIN_DISTANCE;
	report.orientation      = 0;
	report.signal_quality   = -1;
	report.timestamp        = hrt_absolute_time();
	report.type             = distance_sensor_s::MAV_DISTANCE_SENSOR_ULTRASOUND;
	report.variance         = 0.0f;

	int instance_id;
	orb_publish_auto(ORB_ID(distance_sensor), &_distance_sensor_topic, &report, &instance_id);
	PX4_ERR("Hello World!");
	// Begin the next measurement.
	if (measure() != PX4_OK) {
		PX4_INFO("measurement error, address 0x%02X", get_device_address());
		perf_count(_comms_error);
		perf_end(_sample_perf);
		return ret_val;
	}

	perf_end(_sample_perf);
	return PX4_OK;
}


int
ADD2022::init()
{
	// Initialize the I2C device
	if (I2C::init() != OK) {
		PX4_ERR("I2C::init() failed!!");
		return PX4_ERROR;
	}

	// Check for connected rangefinders on each i2c port by decrementing from the base address,
	// (ADD2022_BASE_ADDR = 112).
	uint8_t address = ADD2022_BASE_ADDR;
	{
		set_device_address(address);

		if (measure() == PX4_OK) {
			PX4_INFO("address 0x%02X added", get_device_address());
			px4_usleep(_measure_interval);
		}
	}

	return PX4_OK;
}

int
ADD2022::measure()
{
	// Send the command to take a measurement.
	uint8_t cmd = ADD2022_TAKE_RANGE_REG;
	int ret_val = transfer(&cmd, 1, nullptr, 0);

	return ret_val;
}

void
ADD2022::print_status()
{
	I2CSPIDriverBase::print_status();
	perf_print_counter(_sample_perf);
	perf_print_counter(_comms_error);
	PX4_INFO("poll interval:  %ums", _measure_interval / 1000);

	PX4_INFO("I2C address %u", get_device_address());
}

void
ADD2022::RunImpl()
{
	// Collect the sensor data.
	if (collect() != PX4_OK) {
		PX4_INFO("collection error");
	}
}

int
ADD2022::set_address(const uint8_t address)
{

	if (address < 2    ||
	    address == 80  ||
	    address == 164 ||
	    address == 170 ||
	    address > 224) {
		PX4_ERR("incompatible address requested");
		return PX4_ERROR;
	}

	PX4_INFO("requested address: %u", address);

	uint8_t shifted_address = (address << 1);
	uint8_t cmd[3] = {ADD2022_SET_ADDRESS_1, ADD2022_SET_ADDRESS_2, shifted_address};

	if (transfer(cmd, sizeof(cmd), nullptr, 0) != PX4_OK) {
		PX4_INFO("could not set the address");
	}

	set_device_address(address);
	PX4_INFO("device address: %u", get_device_address());
	return PX4_OK;
}

void
ADD2022::start()
{
	// Fetch parameter values.
	ModuleParams::updateParams();

	// Schedule the driver cycle at regular intervals.
	ScheduleOnInterval(_measure_interval);
}

void
ADD2022::custom_method(const BusCLIArguments &cli)
{
	set_address(cli.i2c_address);
}

I2CSPIDriverBase *ADD2022::instantiate(const BusCLIArguments &cli, const BusInstanceIterator &iterator,
				      int runtime_instance)
{
	ADD2022 *instance = new ADD2022(iterator.configuredBusOption(), iterator.bus(), cli.bus_frequency, cli.i2c_address);

	if (instance == nullptr) {
		PX4_ERR("alloc failed");
		return nullptr;
	}

	if (instance->init() != PX4_OK) {
		delete instance;
		return nullptr;
	}

	instance->start();
	return instance;
}


void
ADD2022::print_usage()
{
	PRINT_MODULE_USAGE_NAME("add2022", "driver");
	PRINT_MODULE_USAGE_SUBCATEGORY("distance_sensor");
	PRINT_MODULE_USAGE_COMMAND("start");
	PRINT_MODULE_USAGE_PARAMS_I2C_SPI_DRIVER(true, false);
	PRINT_MODULE_USAGE_PARAMS_I2C_ADDRESS(0x70);
	PRINT_MODULE_USAGE_COMMAND("set_address");
	PRINT_MODULE_USAGE_PARAMS_I2C_ADDRESS(0x70);
	PRINT_MODULE_USAGE_DEFAULT_COMMANDS();
}

extern "C" __EXPORT int add2022_main(int argc, char *argv[])
{
	using ThisDriver = ADD2022;
	BusCLIArguments cli{true, false};
	cli.i2c_address = ADD2022_BASE_ADDR;
	cli.default_i2c_frequency = ADD2022_BUS_SPEED;

	const char *verb = cli.parseDefaultArguments(argc, argv);

	if (!verb) {
		ThisDriver::print_usage();
		return -1;
	}

	BusInstanceIterator iterator(MODULE_NAME, cli, DRV_DIST_DEVTYPE_ADD2022);

	if (!strcmp(verb, "start")) {
		return ThisDriver::module_start(cli, iterator);
	}

	if (!strcmp(verb, "stop")) {
		return ThisDriver::module_stop(iterator);
	}

	if (!strcmp(verb, "status")) {
		return ThisDriver::module_status(iterator);
	}

	if (!strcmp(verb, "set_address")) {
		return ThisDriver::module_custom_method(cli, iterator);
	}

	ThisDriver::print_usage();
	return -1;
}
