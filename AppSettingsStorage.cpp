/*
 * AppSettingsStorage.cpp
 *
 *  Created on: Mar 29, 2016
 *      Author: andri
 */

#include "AppSettingsStorage.h"

FlashStorage(my_flash_store, StuffProps);

void AppSettingsStorage::load()
{
	props = my_flash_store.read();

	if (props.config.configInitialized == false) {
		init();
	}
	else {
		Serial.println("CONFIG ready!");
	}
}

void AppSettingsStorage::init()
{
	Serial.println("CONFIG Initted!");
	props.init();
	props.config.configInitialized = true;
	save();
}

void AppSettingsStorage::save() {
	my_flash_store.write(props);
}
