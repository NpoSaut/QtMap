#ifndef PACKING_H
#define PACKING_H

// ┌───────────────────────────────────────────────┐
// │   Описываются дефайны для упаковки структур   │
// └───────────────────────────────────────────────┘

#ifdef _WIN32
#define PACKED(__STRUCT__)      __pragma( pack(push, 1) ) __STRUCT__ __pragma( pack(pop) )
#endif

#ifdef __GNUC__
#define PACKED(__STRUCT__)      __STRUCT__ __attribute__((packed))
#endif

#endif // PACKING_H
