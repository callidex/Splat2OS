/*
 * LCDController.cpp
 *
 *  Created on: Feb 7, 2022
 *      Author: paul
 */

#include "lcd.h"

namespace Integration {

LCDController::LCDController(int portID) {
	pID = portID;
	currentPage = 0;
}

LCDController::~LCDController() {
}

void LCDController::PushPage() {


}

void LCDController::Refresh() {

}

} /* namespace Integration */

