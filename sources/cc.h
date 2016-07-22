#ifndef __CC_H
#define __CC_H

#if defined (__ICCARM__)
	#define __inline	inline
	
#elif defined (__CC_ARM)
	//#define 
	
#elif defined (__GNUC__)
	#define __inline	inline

#elif defined (__TASKING__)
	#define __inline	inline

#endif


#endif
