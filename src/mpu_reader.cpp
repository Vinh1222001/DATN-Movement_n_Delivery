#include "mpu_reader.hpp"

MPUReader::MPUReader(Monitor *monitor)
		: BaseModule(
					"MPU_READER",
					MPU_READER_TASK_PRIORITY,
					MPU_READER_TASK_DELAY,
					MPU_READER_TASK_STACK_DEPTH_LEVEL,
					MPU_READER_TASK_PINNED_CORE_ID)
{
	this->init();

	this->data.xMutex = xSemaphoreCreateMutex();
	this->data.value.lastTime = millis();

	this->monitor = monitor;
}

MPUReader::~MPUReader() {}

void MPUReader::setup()
{
	const char *accRange, *gyroRange, *filterBandwidth;

	this->sensor->setAccelerometerRange(MPU6050_RANGE_8_G);
	switch (this->sensor->getAccelerometerRange())
	{
	case MPU6050_RANGE_2_G:
		accRange = "+-2G";
		break;
	case MPU6050_RANGE_4_G:
		accRange = "+-4G";
		break;
	case MPU6050_RANGE_8_G:
		accRange = "+-8G";
		break;
	case MPU6050_RANGE_16_G:
		accRange = "+-16G";
		break;
	}

	ESP_LOGI(this->NAME, "Accelerometer range set to: %s\n", accRange);

	// Setting Gyro Range
	this->sensor->setGyroRange(MPU6050_RANGE_500_DEG);
	switch (this->sensor->getGyroRange())
	{
	case MPU6050_RANGE_250_DEG:
		gyroRange = "+- 250 deg/s";
		break;
	case MPU6050_RANGE_500_DEG:
		gyroRange = "+- 500 deg/s";
		break;
	case MPU6050_RANGE_1000_DEG:
		gyroRange = "+- 1000 deg/s";
		break;
	case MPU6050_RANGE_2000_DEG:
		gyroRange = "+- 2000 deg/s";
		break;
	}

	ESP_LOGI(this->NAME, "Gyro range set to: %s\n", gyroRange);

	// Setting Filter Bandwidth
	this->sensor->setFilterBandwidth(MPU6050_BAND_5_HZ);
	switch (this->sensor->getFilterBandwidth())
	{
	case MPU6050_BAND_260_HZ:
		filterBandwidth = "260 Hz";
		break;
	case MPU6050_BAND_184_HZ:
		filterBandwidth = "184 Hz";
		break;
	case MPU6050_BAND_94_HZ:
		filterBandwidth = "94 Hz";
		break;
	case MPU6050_BAND_44_HZ:
		filterBandwidth = "44 Hz";
		break;
	case MPU6050_BAND_21_HZ:
		filterBandwidth = "21 Hz";
		break;
	case MPU6050_BAND_10_HZ:
		filterBandwidth = "10 Hz";
		break;
	case MPU6050_BAND_5_HZ:
		filterBandwidth = "5 Hz";
		break;
	}

	ESP_LOGI(this->NAME, "Filter Bandwidth set to: %s\n", filterBandwidth);
}

void MPUReader::init()
{
	this->sensor = new Adafruit_MPU6050();

	if (!this->sensor->begin())
	{
		ESP_LOGE(this->NAME, "Failed to find MPU6050 chip\n");
		while (1)
		{
			delay(10);
		}
	}
	ESP_LOGI(this->NAME, "MPU6050 Found!\n");

	this->setup();
}

void MPUReader::taskFn()
{
	this->setData();

	ESP_LOGI(
			this->NAME,
			"Velocity x: %.2f, y: %.2f, z: %.2f",
			this->data.value.velocity.x,
			this->data.value.velocity.y,
			this->data.value.velocity.z);

	Acceleration accel = this->getAccelerationData();
	Velocity vel = this->getVelocity();
	Gyroscope gyro = this->getGyroData();

	this->monitor->display(2, "Acc x: %.2f, y: %.2f, z: %.2f", accel.x, accel.y, accel.z);
	this->monitor->display(3, "Vel x: %.2f, y: %.2f, z: %.2f", vel.x, vel.y, vel.z);
	this->monitor->display(4, "Gyro x: %.2f, y: %.2f, z: %.2f", gyro.x, gyro.y, gyro.z);
}

void MPUReader::computeVelocity()
{
	float accelX = this->data.value.acceleration.x; // Already in m/s²
	float accelY = this->data.value.acceleration.y;
	float accelZ = this->data.value.acceleration.z - 9.81; // Remove gravity

	// Get time difference (dt) in seconds
	unsigned long currentTime = esp_timer_get_time() / 1000;			 // Convert to milliseconds
	float dt = (currentTime - this->data.value.lastTime) / 1000.0; // Convert to seconds
	this->data.value.lastTime = currentTime;

	if (dt > 0 && dt < 1) // Prevent division errors
	{
		this->data.value.velocity.x =
				this->setWithThreshold<float>(
						accelX,
						MPU_READER_VELOCITY_X_THRESHOLD);
		this->data.value.velocity.y =
				this->setWithThreshold<float>(
						accelY,
						MPU_READER_VELOCITY_Y_THRESHOLD);
		this->data.value.velocity.z =
				this->setWithThreshold<float>(
						accelZ,
						MPU_READER_VELOCITY_Z_THRESHOLD);
	}
}

template <typename T>
T MPUReader::setWithThreshold(T value, T threshold)
{
	return abs(value) <= threshold ? 0 : value;
}

void MPUReader::setData()
{
	if (this->sensor == nullptr)
		return; // Tránh lỗi nếu sensor chưa được khởi tạo

	if (xSemaphoreTake(this->data.xMutex, portMAX_DELAY) == pdTRUE)
	{
		sensors_event_t currAccel, currGyro, currTemp;
		this->sensor->getEvent(&currAccel, &currGyro, &currTemp);

		// Cập nhật dữ liệu gia tốc
		this->data.value.acceleration.x =
				this->setWithThreshold<float>(
						currAccel.acceleration.x,
						MPU_READER_ACCELERATION_X_THRESHOLD);

		this->data.value.acceleration.y =
				this->setWithThreshold<float>(
						currAccel.acceleration.y,
						MPU_READER_ACCELERATION_Y_THRESHOLD);
		this->data.value.acceleration.z =
				this->setWithThreshold<float>(
						currAccel.acceleration.z,
						MPU_READER_ACCELERATION_Z_THRESHOLD);

		// Cập nhật dữ liệu con quay hồi chuyển
		this->data.value.gyroscope.x =
				this->setWithThreshold<float>(
						currGyro.gyro.x,
						MPU_READER_GYROSCOPE_X_THRESHOLD);

		this->data.value.gyroscope.y =
				this->setWithThreshold<float>(
						currGyro.gyro.x,
						MPU_READER_GYROSCOPE_Y_THRESHOLD);
		this->data.value.gyroscope.z =
				this->setWithThreshold<float>(
						currGyro.gyro.x,
						MPU_READER_GYROSCOPE_Z_THRESHOLD);

		this->computeVelocity();

		xSemaphoreGive(this->data.xMutex);
	}
}

MotionState MPUReader::getMpuReaderData()
{
	MotionState data;
	if (xSemaphoreTake(this->data.xMutex, portMAX_DELAY) == pdTRUE)
	{
		data = this->data.value;
		xSemaphoreGive(this->data.xMutex);
	}

	return data;
}

Gyroscope MPUReader::getGyroData()
{
	Gyroscope data;
	if (xSemaphoreTake(this->data.xMutex, portMAX_DELAY) == pdTRUE)
	{
		data = this->data.value.gyroscope;
		xSemaphoreGive(this->data.xMutex);
	}

	return data;
}

Acceleration MPUReader::getAccelerationData()
{
	Acceleration data;
	if (xSemaphoreTake(this->data.xMutex, portMAX_DELAY) == pdTRUE)
	{
		data = this->data.value.acceleration;
		xSemaphoreGive(this->data.xMutex);
	}

	return data;
}

Velocity MPUReader::getVelocity()
{
	Velocity data;
	if (xSemaphoreTake(this->data.xMutex, portMAX_DELAY) == pdTRUE)
	{
		data = this->data.value.velocity;
		xSemaphoreGive(this->data.xMutex);
	}

	return data;
}
