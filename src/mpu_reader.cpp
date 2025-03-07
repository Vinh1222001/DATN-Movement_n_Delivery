#include "mpu_reader.hpp"

MPUReader::MPUReader()
    : BaseModule(
          "MPU_READER",
          MPU_READER_TASK_PRIORITY,
          MPU_READER_TASK_DELAY,
          MPU_READER_TASK_STACK_DEPTH_LEVEL,
          MPU_READER_TASK_PINNED_CORE_ID)
{
    // Config pins
}

MPUReader::~MPUReader()
{
}

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
    sensors_event_t a, g, temp;
    this->sensor->getEvent(&a, &g, &temp);

    /* Print out the values */
    ESP_LOGI(this->NAME, "Acceleration X: %.2f, Y: %.2f, Z: %.2f m/s^2\n", a.acceleration.x, a.acceleration.y, a.acceleration.z);

    ESP_LOGI(this->NAME, "Rotation X: %.2f, Y: %.2f, Z: %.2f rad/s\n", g.gyro.x, g.gyro.y, g.gyro.z);

    ESP_LOGI(this->NAME, "Temperature: %.2f degC\n", temp.temperature);
}

MPUReader *mpuReader;