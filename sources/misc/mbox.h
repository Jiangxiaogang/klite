#ifndef __MBOX_H
#define __MBOX_H

typedef void *mbox_t;

mbox_t mbox_create(void);
void   mbox_delete(mbox_t mbox);
void   mbox_post(mbox_t mbox, uint32_t data);
void   mbox_wait(mbox_t mbox, uint32_t *pdata);
bool   mbox_timedwait(mbox_t mbox, uint32_t timeout, uint32_t *pdata);

#endif
