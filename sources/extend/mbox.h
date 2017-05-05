#ifndef __MBOX_H
#define __MBOX_H

typedef void* mbox_t;

mbox_t mbox_create(void);
void   mbox_delete(mbox_t mbox);
void   mbox_post(mbox_t mbox, int data);
void   mbox_wait(mbox_t mbox, int* pdata);
int    mbox_timedwait(mbox_t mbox, int* pdata, int timeout);

#endif
