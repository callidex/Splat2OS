/*
 * LCDController.h
 *
 *  Created on: Feb 7, 2022
 *      Author: paul
 */

#ifndef SRC_LCDCONTROLLER_H_
#define SRC_LCDCONTROLLER_H_
#ifdef __cplusplus
namespace Integration {

class LCDController {
public:
	LCDController(int portID);
	virtual ~LCDController();
	void PushPage();
	void Refresh();

private:
	int currentPage;
	int pID;
};

} /* namespace Integration */
#endif
#endif /* SRC_LCDCONTROLLER_H_ */
