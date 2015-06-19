/*
* AP_IRLock_SITL.h
*
* Created on: June 18, 2015
* Author: Ianc Chen
*/
#ifndef AP_IRLOCK_SITL_H_
#define AP_IRLOCK_SITL_H_
#include "IRLock.h"
class AP_IRLock_SITL : public IRLock
{
public:
AP_IRLock_SITL(const AP_AHRS &ahrs);
virtual void init();
virtual void update();
private:
int _fd;
uint64_t _last_timestamp;
};
#endif /* AP_IRLOCK_SITL_H_ */
