/*
 * This code supports multiple displays since GPIO pin handles have been
 * moved into user_data_struct. I2C and SPI handles are global since they can be
 * shared by multiple devices (think I2C with different address sharing bus).
 *
 * So far I have tested this with 3 software I2C displays. See examples.
 */

#include "u8g2.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <limits.h>

#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#define MAX_I2C_HANDLES 8

/*
 * User data passed in user_ptr of u8x8_struct.
 */
struct user_data_struct {
	// For I2C /dev/i2c-%d and for SPI /dev/spidev%d.%d using high and low 4 bits
	uint8_t bus;
	// Index into buffer
	uint8_t index;
	// Callback buffer, I2C should send 32 bytes max and SPI 128 bytes max
	uint8_t buffer[256];  // issue 2666
	// Nanosecond delay for U8X8_MSG_DELAY_I2C
	unsigned long delay;
	// Internal buffer
	uint8_t *int_buf;
};
typedef struct user_data_struct user_data_t;

enum i2c_error_code {
    I2C_ERROR_ARG               = -1, /* Invalid arguments */
    I2C_ERROR_OPEN              = -2, /* Opening I2C device */
    I2C_ERROR_QUERY             = -3, /* Querying I2C device attributes */
    I2C_ERROR_NOT_SUPPORTED     = -4, /* I2C not supported on this device */
    I2C_ERROR_TRANSFER          = -5, /* I2C transfer */
    I2C_ERROR_CLOSE             = -6, /* Closing I2C device */
};
typedef struct i2c_handle i2c_t;

/* Primary Functions */
i2c_t *i2c_new(void);
int i2c_open(i2c_t *i2c, const char *path);
int i2c_transfer(i2c_t *i2c, struct i2c_msg *msgs, size_t count);
int i2c_close(i2c_t *i2c);
void i2c_free(i2c_t *i2c);
const char *i2c_errmsg(i2c_t *i2c);

// c-periphery I2C handles
static i2c_t *i2c_handles[MAX_I2C_HANDLES] = { NULL };

/*
 * Sleep milliseconds.
 */
void sleep_ms(unsigned long milliseconds) {
	struct timespec ts;
	ts.tv_sec = milliseconds / 1000;
	ts.tv_nsec = (milliseconds % 1000) * 1000000;
	nanosleep(&ts, NULL);
}

/*
 * Sleep microseconds.
 */
void sleep_us(unsigned long microseconds) {
	struct timespec ts;
	ts.tv_sec = microseconds / 1000 / 1000;
	ts.tv_nsec = (microseconds % 1000000) * 1000;
	nanosleep(&ts, NULL);
}

/**
 * Sleep nanoseconds.
 */
void sleep_ns(unsigned long nanoseconds) {
	struct timespec ts;
	ts.tv_sec = 0;
	ts.tv_nsec = nanoseconds;
	nanosleep(&ts, NULL);
}

// void SetBufferPtr(u8g2_t *u8g2, uint8_t *buf) {
// 	u8g2->tile_buf_ptr = buf;
// }

// uint16_t GetBufferSize(u8g2_t *u8g2) {
// 	return u8g2->u8x8.display_info->tile_width * 8 * u8g2->tile_buf_height;
// }

/*
 * Allocate user_data_struct, set common values and set user_ptr.
 */
user_data_t* init_user_data(u8g2_t *u8g2) {
	// Dynamically allocate user data_struct
	user_data_t *user_data = (user_data_t*) malloc(sizeof(user_data_t));
	// Dynamically allocate internal buffer
	user_data->int_buf = (uint8_t*)malloc(u8g2_GetBufferSize(u8g2));
	// We need a unique buffer for each display in order to be thread friendly
	u8g2_SetBufferPtr(u8g2, user_data->int_buf);
	u8g2_SetUserPtr(u8g2, user_data);
	return user_data;
}

/*
 * Allocate user_data_struct for I2C hardware.
 */
void init_i2c_hw(u8g2_t *u8g2, uint8_t bus) {
	user_data_t *user_data = init_user_data(u8g2);
	user_data->bus = bus;
}

/*
 * Close GPIO pins and free user_data_struct.
 */
void done_user_data(u8g2_t *u8g2) {
	user_data_t *user_data = u8g2_GetUserPtr(u8g2);
	if (user_data != NULL) {
		// Free internal buffer
		free(user_data->int_buf);
		// Free user data struct
		free(user_data);
		u8g2_SetUserPtr(u8g2, NULL);
	}
}

/*
 * Initialize I2C bus.
 */
void init_i2c(u8x8_t *u8x8) {
	char filename[11];
	user_data_t *user_data = u8x8_GetUserPtr(u8x8);
	// Only open bus once
	if (i2c_handles[user_data->bus] == NULL) {
		snprintf(filename, sizeof(filename), "/dev/i2c-%d", user_data->bus);
		i2c_handles[user_data->bus] = i2c_new();
		int error = i2c_open(i2c_handles[user_data->bus], filename);
		if (error) {
			fprintf(stderr, "i2c_open(): %s\n",
					i2c_errmsg(i2c_handles[user_data->bus]));
			i2c_free(i2c_handles[user_data->bus]);
			i2c_handles[user_data->bus] = NULL;
		}
	}
}

/*
 * Close and free all i2c_t.
 */
void done_i2c() {
	for (int i = 0; i < MAX_I2C_HANDLES; ++i) {
		if (i2c_handles[i] != NULL) {
			i2c_close(i2c_handles[i]);
			i2c_free(i2c_handles[i]);
			i2c_handles[i] = NULL;
		}
	}
}

/**
 * GPIO callback.
 */
uint8_t u8x8_arm_linux_gpio_and_delay(u8x8_t *u8x8, uint8_t msg,
		uint8_t arg_int, void *arg_ptr) {
	user_data_t *user_data;

	(void) arg_ptr; /* suppress unused parameter warning */
	switch (msg) {
	case U8X8_MSG_DELAY_NANO:
		// delay arg_int * 1 nano second or 0 for none
		user_data = u8x8_GetUserPtr(u8x8);
		if (user_data->delay != 0) {
			sleep_ns(user_data->delay);
		}
		break;

	case U8X8_MSG_DELAY_100NANO:
		// delay arg_int * 100 nano seconds
		sleep_ns(arg_int * 100);
		break;

	case U8X8_MSG_DELAY_10MICRO:
		// delay arg_int * 10 micro seconds
		sleep_us(arg_int * 10);
		break;

	case U8X8_MSG_DELAY_MILLI:
		// delay arg_int * 1 milli second
		sleep_ms(arg_int);
		break;

	case U8X8_MSG_DELAY_I2C:
		// arg_int is the I2C speed in 100KHz, e.g. 4 = 400 KHz, but we ignore
		// that and use user_data->delay
		user_data = u8x8_GetUserPtr(u8x8);
		if (user_data->delay != 0) {
			sleep_ns(user_data->delay);
		}
		break;

	default:
		return 0;
	}
	return 1;
}

/*
 * I2c callback.
 */
uint8_t u8x8_byte_arm_linux_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int,
		void *arg_ptr) {
	user_data_t *user_data;
	uint8_t *data;
	struct i2c_msg msgs[1];

	switch (msg) {
	case U8X8_MSG_BYTE_SEND:
		user_data = u8x8_GetUserPtr(u8x8);
		data = (uint8_t*) arg_ptr;
		while (arg_int > 0) {
			user_data->buffer[user_data->index++] = *data;
			data++;
			arg_int--;
		}
		break;

	case U8X8_MSG_BYTE_INIT:
		init_i2c(u8x8);
		break;

	case U8X8_MSG_BYTE_START_TRANSFER:
		user_data = u8x8_GetUserPtr(u8x8);
		user_data->index = 0;
		break;

	case U8X8_MSG_BYTE_END_TRANSFER:
		user_data = u8x8_GetUserPtr(u8x8);
		msgs[0].addr = u8x8_GetI2CAddress(u8x8) >> 1;
		msgs[0].flags = 0; // Write
		msgs[0].len = user_data->index;
		msgs[0].buf = user_data->buffer;
		i2c_transfer(i2c_handles[user_data->bus], msgs, 1);
		break;

	default:
		return 0;
	}
	return 1;
}

int libu8g2_Setup(u8g2_t *u8g2, uint8_t i2c_bus) {
	if (u8g2 == NULL) {
		perror("libu8g2_Setup: u8g2 is NULL");
		return -1;
	}
	u8g2_Setup_ssd1312_i2c_128x64_noname_f(u8g2, U8G2_R0,
                                           u8x8_byte_arm_linux_hw_i2c, u8x8_arm_linux_gpio_and_delay);
    init_i2c_hw(u8g2, i2c_bus);
    u8g2_InitDisplay(u8g2);
    u8g2_SetPowerSave(u8g2, 0);
    return 0;
}

int libu8g2_Done(u8g2_t *u8g2) {
	if (u8g2 == NULL) {
		perror("libu8g2_Done: u8g2 is NULL");
		return -1;
	}
	u8g2_SetPowerSave(u8g2, 1);
    done_i2c();
    done_user_data(u8g2);
    return 0;
}