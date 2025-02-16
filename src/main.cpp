#define LIS3MDL 3
#define MLX90393 90393
#define MMC5603 5603
#define MMC5983MA 5983
#define SENSOR MMC5983MA

#include <Arduino.h>
#include <Comp6DOF_n0m1.h>
Comp6DOF_n0m1 sixDOF;

#if SENSOR == LIS3MDL
#include <Adafruit_LIS3MDL.h>
Adafruit_LIS3MDL lis3mdl;
#elif SENSOR == MLX90393
#include <Adafruit_MLX90393.h>
Adafruit_MLX90393 mlx90393;
#elif SENSOR == MMC5603
#include <Adafruit_MMC56x3_RAW.h>
Adafruit_MMC5603_RAW mmc5603;
#elif SENSOR == MMC5983MA
#include <SparkFun_MMC5983MA_Arduino_Library.h>
#include <Wire.h>
#define MMC5983MA_MODE_BITS 18
#define MMC5983MA_FULL_SCALE_RANGE_UTESLA 800
SFE_MMC5983MA mmc5983ma;
#endif

void setup() {
	Serial.begin(115200);
	while (!Serial) delay(10);

	Serial.println("Hello World");

	// connect sensor:
#if SENSOR == LIS3MDL
	if (!lis3mdl.begin_I2C()) {
		Serial.println("Failed to find LIS3MDL chip");
		while (true) {
			delay(10);
		}
	}
	Serial.println("LIS3MDL Found!");
#elif SENSOR == MLX90393
	if (!mlx90393.begin_I2C(0x18)) {
		Serial.println(": Failed to find MLX90393 chip");
		while (true) {
			delay(10);
		}
	}
	Serial.println("MLX90393 Found!");
#elif SENSOR == MMC5603
	if (!mmc5603.begin(MMC56X3_DEFAULT_ADDRESS, &Wire)) {
		Serial.println("Failed to find MMC5603 chip");
		while (true) {
			delay(10);
		}
	}
	Serial.println("MMC5603 Found!");
#elif SENSOR == MMC5983MA
	Wire.begin();
	if (!mmc5983ma.begin()) {
		Serial.println("Failed to find MMC5983MA chip");
		while (true) {
			delay(10);
		}
	}
	Serial.println("MMC5983MA Found!");
#endif

	// configure sensor
#if SENSOR == LIS3MDL
	lis3mdl.setPerformanceMode(LIS3MDL_MEDIUMMODE);
	Serial.print("Performance mode set to: ");
	switch (lis3mdl.getPerformanceMode()) {
		case LIS3MDL_LOWPOWERMODE: Serial.println("Low"); break;
		case LIS3MDL_MEDIUMMODE: Serial.println("Medium"); break;
		case LIS3MDL_HIGHMODE: Serial.println("High"); break;
		case LIS3MDL_ULTRAHIGHMODE: Serial.println("Ultra-High"); break;
	}

	lis3mdl.setOperationMode(LIS3MDL_CONTINUOUSMODE);
	Serial.print("Operation mode set to: ");
	// Single shot mode will complete conversion and go into power down
	switch (lis3mdl.getOperationMode()) {
		case LIS3MDL_CONTINUOUSMODE: Serial.println("Continuous"); break;
		case LIS3MDL_SINGLEMODE: Serial.println("Single mode"); break;
		case LIS3MDL_POWERDOWNMODE: Serial.println("Power-down"); break;
	}

	lis3mdl.setDataRate(LIS3MDL_DATARATE_155_HZ);
	// You can check the datarate by looking at the frequency of the DRDY pin
	Serial.print("Data rate set to: ");
	switch (lis3mdl.getDataRate()) {
		case LIS3MDL_DATARATE_0_625_HZ: Serial.println("0.625 Hz"); break;
		case LIS3MDL_DATARATE_1_25_HZ: Serial.println("1.25 Hz"); break;
		case LIS3MDL_DATARATE_2_5_HZ: Serial.println("2.5 Hz"); break;
		case LIS3MDL_DATARATE_5_HZ: Serial.println("5 Hz"); break;
		case LIS3MDL_DATARATE_10_HZ: Serial.println("10 Hz"); break;
		case LIS3MDL_DATARATE_20_HZ: Serial.println("20 Hz"); break;
		case LIS3MDL_DATARATE_40_HZ: Serial.println("40 Hz"); break;
		case LIS3MDL_DATARATE_80_HZ: Serial.println("80 Hz"); break;
		case LIS3MDL_DATARATE_155_HZ: Serial.println("155 Hz"); break;
		case LIS3MDL_DATARATE_300_HZ: Serial.println("300 Hz"); break;
		case LIS3MDL_DATARATE_560_HZ: Serial.println("560 Hz"); break;
		case LIS3MDL_DATARATE_1000_HZ: Serial.println("1000 Hz"); break;
	}

	lis3mdl.setRange(LIS3MDL_RANGE_4_GAUSS);
	Serial.print("Range set to: ");
	switch (lis3mdl.getRange()) {
		case LIS3MDL_RANGE_4_GAUSS: Serial.println("+-4 gauss"); break;
		case LIS3MDL_RANGE_8_GAUSS: Serial.println("+-8 gauss"); break;
		case LIS3MDL_RANGE_12_GAUSS: Serial.println("+-12 gauss"); break;
		case LIS3MDL_RANGE_16_GAUSS: Serial.println("+-16 gauss"); break;
	}

		// This is useful for generating an Interrupt when magnet comes close (N = 500) in the direction of the z-Axis.
		// lis3mdl.setIntThreshold(500);
		// lis3mdl.configInterrupt(false, false, true, // enable z axis
		//                         true, // polarity
		//                         false, // don't latch
		//                         true); // enabled!
#elif SENSOR == MLX90393
	mlx90393.setGain(MLX90393_GAIN_1X);
	Serial.print("Gain set to: ");
	switch (mlx90393.getGain()) {
		case MLX90393_GAIN_1X: Serial.println("1 x"); break;
		case MLX90393_GAIN_1_33X: Serial.println("1.33 x"); break;
		case MLX90393_GAIN_1_67X: Serial.println("1.67 x"); break;
		case MLX90393_GAIN_2X: Serial.println("2 x"); break;
		case MLX90393_GAIN_2_5X: Serial.println("2.5 x"); break;
		case MLX90393_GAIN_3X: Serial.println("3 x"); break;
		case MLX90393_GAIN_4X: Serial.println("4 x"); break;
		case MLX90393_GAIN_5X: Serial.println("5 x"); break;
	}

	// Set resolution, per axis. Aim for sensitivity of ~0.3 for all axes.
	mlx90393.setResolution(MLX90393_X, MLX90393_RES_17);
	Serial.print("X Resolution set to: ");
	switch (mlx90393.getResolution(MLX90393_X)) {
		case MLX90393_RES_16: Serial.println("16"); break;
		case MLX90393_RES_17: Serial.println("17"); break;
		case MLX90393_RES_18: Serial.println("18"); break;
		case MLX90393_RES_19: Serial.println("19"); break;
	}

	mlx90393.setResolution(MLX90393_Y, MLX90393_RES_17);
	Serial.print("Y Resolution set to: ");
	switch (mlx90393.getResolution(MLX90393_Y)) {
		case MLX90393_RES_16: Serial.println("16"); break;
		case MLX90393_RES_17: Serial.println("17"); break;
		case MLX90393_RES_18: Serial.println("18"); break;
		case MLX90393_RES_19: Serial.println("19"); break;
	}

	mlx90393.setResolution(MLX90393_Z, MLX90393_RES_16);
	Serial.print("Z Resolution set to: ");
	switch (mlx90393.getResolution(MLX90393_Z)) {
		case MLX90393_RES_16: Serial.println("16"); break;
		case MLX90393_RES_17: Serial.println("17"); break;
		case MLX90393_RES_18: Serial.println("18"); break;
		case MLX90393_RES_19: Serial.println("19"); break;
	}

	// Set oversampling, see OSR at 16.2.5.
	mlx90393.setOversampling(MLX90393_OSR_3);
	Serial.print("Oversampling set to: ");
	switch (mlx90393.getOversampling()) {
		case MLX90393_OSR_3: Serial.println("3");
		case MLX90393_OSR_2: Serial.println("2");
		case MLX90393_OSR_1: Serial.println("1");
		case MLX90393_OSR_0: Serial.println("0");
	}

	// Set digital filtering, see DIG_FILT at 16.2.5.
	mlx90393.setFilter(MLX90393_FILTER_5);
	Serial.print("Filter set to: ");
	switch (mlx90393.getFilter()) {
		case MLX90393_FILTER_7: Serial.println("7");
		case MLX90393_FILTER_6: Serial.println("6");
		case MLX90393_FILTER_5: Serial.println("5");
		case MLX90393_FILTER_4: Serial.println("4");
		case MLX90393_FILTER_3: Serial.println("3");
		case MLX90393_FILTER_2: Serial.println("2");
		case MLX90393_FILTER_1: Serial.println("1");
		case MLX90393_FILTER_0: Serial.println("0");
	}
#elif SENSOR == MMC5603
	mmc5603.printSensorDetails();

	mmc5603.setDataRate(100);
	Serial.print("Data Rate set to: ");
	Serial.println(mmc5603.getDataRate());

	mmc5603.setContinuousMode(true);
	Serial.print("Continuous Mode set to: ");
	Serial.println("true");
#elif SENSOR == MMC5983MA
	mmc5983ma.setFilterBandwidth(100);
	Serial.print("Filter Bandwidth set to: ");
	Serial.println(mmc5983ma.getFilterBandwidth());

	mmc5983ma.setContinuousModeFrequency(100);
	Serial.print("Continuous Mode Frequency set to: ");
	Serial.println(mmc5983ma.getContinuousModeFrequency());

	mmc5983ma.enableAutomaticSetReset();
	Serial.print("Automatic Set Reset set to: ");
	Serial.println("true");

	mmc5983ma.enableContinuousMode();
	Serial.print("Continuous Mode set to: ");
	Serial.println("true");
#endif

		// hard-iron calibrate sensor
#if SENSOR == MMC5983MA
		// do {
		//	uint32_t x_value = 0, y_value = 0, z_value = 0;
		//	do {
		//		delay(50);
		//		mmc5983ma.readFieldsXYZ(&x_value, &y_value, &z_value);
		//	} while (!sixDOF.deviantSpread(static_cast<int>(x_value) - (1 << MMC5983MA_MODE_BITS - 1), static_cast<int>(y_value) - (1 << MMC5983MA_MODE_BITS - 1), static_cast<int>(z_value) - (1 << MMC5983MA_MODE_BITS - 1)));
		// } while (!sixDOF.calOffsets());

#elif SENSOR == LIS3MDL
		// do {
		//	do {
		//		delay(50);
		//		lis3mdl.read();
		//	} while (!sixDOF.deviantSpread(lis3mdl.x, lis3mdl.y, lis3mdl.z));
		// } while (!sixDOF.calOffsets());
#elif SENSOR == MLX90393
	// do {
	//	int16_t x_value = 0, y_value = 0, z_value = 0;
	//	do {
	//		delay(50);
	//		mlx90393.readRawData(&x_value, &y_value, &z_value);
	//	} while (!sixDOF.deviantSpread(x_value, y_value, z_value));
	// } while (!sixDOF.calOffsets());
#elif SENSOR == MMC5603
	// do {
	//	int32_t x_value = 0, y_value = 0, z_value = 0;
	//	do {
	//		delay(50);
	//		mmc5603.readRawData(&x_value, &y_value, &z_value);
	//	} while (!sixDOF.deviantSpread(x_value, y_value, z_value));
	// } while (!sixDOF.calOffsets());
#endif

	// Serial.print("\tXoff: ");
	// Serial.print(sixDOF.xHardOff());
	// Serial.print(" \tYoff: ");
	// Serial.print(sixDOF.yHardOff());
	// Serial.print(" \tZoff: ");
	// Serial.print(sixDOF.zHardOff());
	// Serial.println("");
}

void loop() {
#if SENSOR == MMC5983MA
	// TODO: check if new measurement is available
	uint32_t x_value = 0, y_value = 0, z_value = 0;
	mmc5983ma.readFieldsXYZ(&x_value, &y_value, &z_value);  // 18bit Operation
	// mmc5983ma.clearMeasDoneInterrupt()

	Serial.print("\tX: ");
	Serial.print(static_cast<double>(static_cast<int>(x_value) - (1 << MMC5983MA_MODE_BITS - 1)) / (1 << MMC5983MA_MODE_BITS - 1) * MMC5983MA_FULL_SCALE_RANGE_UTESLA, 5);
	Serial.print(" \tY: ");
	Serial.print(static_cast<double>(static_cast<int>(y_value) - (1 << MMC5983MA_MODE_BITS - 1)) / (1 << MMC5983MA_MODE_BITS - 1) * MMC5983MA_FULL_SCALE_RANGE_UTESLA, 5);
	Serial.print(" \tZ: ");
	Serial.print(static_cast<double>(static_cast<int>(z_value) - (1 << MMC5983MA_MODE_BITS - 1)) / (1 << MMC5983MA_MODE_BITS - 1) * MMC5983MA_FULL_SCALE_RANGE_UTESLA, 5);
	Serial.println(" uTesla ");
#else
	// Get a new sensor event, normalized to uTesla
	sensors_event_t event;
#if SENSOR == LIS3MDL
	lis3mdl.getEvent(&event);
#elif SENSOR == MLX90393
	mlx90393.getEvent(&event);
#elif SENSOR == MMC5603
	mmc5603.getEvent(&event);
#endif

	// Display the results (magnetic field is measured in uTesla)
	Serial.print("\tX: ");
	Serial.print(event.magnetic.x, 5);
	Serial.print(" \tY: ");
	Serial.print(event.magnetic.y, 5);
	Serial.print(" \tZ: ");
	Serial.print(event.magnetic.z, 5);
	Serial.println(" uTesla ");
#endif

	delay(100);
}
