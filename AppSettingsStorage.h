/*
 * AppSettingsStorage.h
 *
 *  Created on: Mar 29, 2016
 *      Author: andri
 */

#ifndef APPSETTINGSSTORAGE_H_
#define APPSETTINGSSTORAGE_H_

#include "StuffDefs.h"
#include "FlashStorage.h"

struct AppSettingsStorage {

	StuffProps props;
	void init();
	void load();
	void save();
};

#endif /* APPSETTINGSSTORAGE_H_ */
