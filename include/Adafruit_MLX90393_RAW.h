#define private protected
#include <Adafruit_MLX90393.h>

class Adafruit_MLX90393_RAW : public Adafruit_MLX90393 {
   public:
	Adafruit_MLX90393_RAW() = default;

	/**
	 * Performs a single measurement and returns the results.
	 *
	 * @param x     Pointer to where the 'x' value should be stored.
	 * @param y     Pointer to where the 'y' value should be stored.
	 * @param z     Pointer to where the 'z' value should be stored.
	 *
	 * @return True if the operation succeeded, otherwise false.
	 */
	bool readRawData(int16_t *x, int16_t *y, int16_t *z) {
		if (!startSingleMeasurement()) return false;
		// See MLX90393 Getting Started Guide for fancy formula
		// tconv = f(OSR, DIG_FILT, OSR2, ZYXT)
		// For now, using Table 18 from datasheet
		// Without +10ms delay measurement doesn't always seem to work
		delay(mlx90393_tconv[_dig_filt][_osr] + 10);

		uint8_t tx[1] = {MLX90393_REG_RM | MLX90393_AXIS_ALL};
		uint8_t rx[6] = {0};

		/* Read a single data sample. */
		if (transceive(tx, sizeof(tx), rx, sizeof(rx), 0) != MLX90393_STATUS_OK) {
			return false;
		}

		/* Convert data to uT and float. */
		*x = (rx[0] << 8) | rx[1];
		*y = (rx[2] << 8) | rx[3];
		*z = (rx[4] << 8) | rx[5];

		if (_res_x == MLX90393_RES_18) *x -= 0x8000;
		if (_res_x == MLX90393_RES_19) *x -= 0x4000;
		if (_res_y == MLX90393_RES_18) *y -= 0x8000;
		if (_res_y == MLX90393_RES_19) *y -= 0x4000;
		if (_res_z == MLX90393_RES_18) *z -= 0x8000;
		if (_res_z == MLX90393_RES_19) *z -= 0x4000;

		return true;
	}
};
#undef private