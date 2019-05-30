Unrolled linked list
--------------------

Nothing particularly fancy, pretty much a standard unrolled linked list
implementation. Just a handy data structure to have around for pulling into
projects.

* doubly-linked to speed up item retreival in certain situations, e.g. if the
  requested item is near the end we don't have to start at the beginning and
  crawl over nearly the entire list.

* Insertions and deletions at the head and tail are amortized O(1) (little bit
  of overhead for allocating/freeing list nodes when the node containing the
  target index is full/empty)

* Insertions and deletions outside the first or last node are O(N), since then
  we have to crawl the list to find the node containing the target index

* list instances have a configurable item size, so your list items can be
  whatever

* list instances have a configurable number of items per node in the list. For
  very large lists, increasing the number of items per node accordingly can
  dramatically improve performance for insertions and deletions in the middle
  of the list
