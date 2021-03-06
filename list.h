/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef LIST_H_
#define LIST_H_

#ifdef __cplusplus
extern "C" {
#endif

struct list;

struct list*
list_new();

void
list_delete(struct list *l);

int
list_append_data(struct list *l, void *data);

void *
list_pop_head(struct list *l);

void *
list_next(struct list *l);

#ifdef __cplusplus
}
#endif

#endif /* LIST_H_ */
