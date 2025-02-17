#define private protected
#include <Adafruit_MMC56x3.h>

class Adafruit_MMC5603_RAW : public Adafruit_MMC5603 {
	public:
	Adafruit_MMC5603_RAW() = default;
	/**
	 * Performs a single measurement and returns the results.
	 *
	 * @param x     Pointer to where the 'x' value should be stored.
	 * @param y     Pointer to where the 'y' value should be stored.
	 * @param z     Pointer to where the 'z' value should be stored.
	 *
	 * @return True if the operation succeeded, otherwise false.
	 */
	bool readRawData(int32_t *x, int32_t *y, int32_t *z) {
		/* Read new data */
		if (!isContinuousMode()) {
			_ctrl0_reg->write(0x01); // TM_M trigger

			Adafruit_BusIO_RegisterBits mag_read_done =
				Adafruit_BusIO_RegisterBits(_status_reg, 1, 6);
			while (!mag_read_done.read()) {
				delay(5);
			}
		}
		uint8_t buffer[9];
		buffer[0] = MMC56X3_OUT_X_L;

		// read 8 bytes!
		i2c_dev->write_then_read(buffer, 1, buffer, 9);

		*x = (uint32_t)buffer[0] << 12 | (uint32_t)buffer[1] << 4 |
			(uint32_t)buffer[6] >> 4;
		*y = (uint32_t)buffer[2] << 12 | (uint32_t)buffer[3] << 4 |
			(uint32_t)buffer[7] >> 4;
		*z = (uint32_t)buffer[4] << 12 | (uint32_t)buffer[5] << 4 |
			(uint32_t)buffer[8] >> 4;
		// fix center offsets
		*x -= (uint32_t)1 << 19;
		*y -= (uint32_t)1 << 19;
		*z -= (uint32_t)1 << 19;

		return true;
	}
};
#undef private