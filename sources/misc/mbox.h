#ifndef __MBOX_H
#define __MBOX_H

typedef struct
{
    event_t  event;
    uint32_t data;
}mbox_t;

bool   mbox_init(mbox_t *mbox);
void   mbox_delete(mbox_t *mbox);
void   mbox_post(mbox_t *mbox, uint32_t data);
void   mbox_wait(mbox_t *mbox, uint32_t *pdata);
bool   mbox_timed_wait(mbox_t *mbox, uint32_t *pdata, uint32_t timeout);

#endif
