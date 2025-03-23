#include "mpu_reader.hpp"

MPUReader::MPUReader()
    : BaseModule(
          "MPU_READER",
          MPU_READER_TASK_PRIORITY,
          MPU_READER_TASK_DELAY,
          MPU_READER_TASK_STACK_DEPTH_LEVEL,
          MPU_READER_TASK_PINNED_CORE_ID)
{
    this->init();
    if (this->sensor != nullptr)
    {
        this->accel = this->sensor->getAccelerometerSensor();
        this->gyro = this->sensor->getGyroSensor();
    }
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
    this->computeVelocity();

    ESP_LOGI(
        this->NAME,
        "Velocity x: %.2f, y: %.2f, z: %.2f",
        this->velocity.x,
        this->velocity.y,
        this->velocity.z);
}

void MPUReader::computeVelocity()
{
    sensors_event_t currAcel;
    this->accel->getEvent(&currAcel);

    float accelX = currAcel.acceleration.x; // Already in m/s²
    float accelY = currAcel.acceleration.y;
    float accelZ = currAcel.acceleration.z - 9.81; // Remove gravity

    // Noise filtering (threshold to avoid drift)
    if (abs(accelX) < ACCEL_THRESHOLD)
        accelX = 0;
    if (abs(accelY) < ACCEL_THRESHOLD)
        accelY = 0;
    if (abs(accelZ) < ACCEL_THRESHOLD)
        accelZ = 0;

    // Get time difference (dt) in seconds
    unsigned long currentTime = esp_timer_get_time() / 1000; // Convert to milliseconds
    float dt = (currentTime - this->lastTime) / 1000.0;      // Convert to seconds
    this->lastTime = currentTime;

    if (dt > 0 && dt < 1) // Prevent division errors
    {
        // Integrate acceleration to get velocity
        this->velocity.x = accelX * dt;
        this->velocity.y = accelY * dt;
        this->velocity.z = accelZ * dt;
    }

    // Apply damping to simulate friction
    this->velocity.x *= SIMULATE_FRICTION;
    this->velocity.y *= SIMULATE_FRICTION;
    this->velocity.z *= SIMULATE_FRICTION;

    // If acceleration is almost zero, stop velocity
    if (accelX == ACCEL_THRESHOLD)
        this->velocity.x = 0;
    if (accelY == ACCEL_THRESHOLD)
        this->velocity.y = 0;
    if (accelZ == ACCEL_THRESHOLD)
        this->velocity.z = 0;
}