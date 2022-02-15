/*
 * ThreadTest.h
 *
 *  Created on: 14 Feb. 2022
 *      Author: paul
 */

#ifndef COMMON_SRC_THREADTEST_H_
#define COMMON_SRC_THREADTEST_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "realmain.h"




class ThreadTest {
public:
	ThreadTest();
	virtual ~ThreadTest();
	void Start();
	void Thread1(void const * argument);
};

#ifdef __cplusplus
}
#endif

#endif /* COMMON_SRC_THREADTEST_H_ */
