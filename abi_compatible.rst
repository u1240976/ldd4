Linux Kernel ABI/API Update
===========================

porting driver from linux 2.6 to linux 4.1x

scull

- ``uid_t`` => ``kuid_t`` (linux 3.14)

  - `LWN - A new approach to user namespaces <https://lwn.net/Articles/491310/>`_

snull

- ``linux/config.h`` => ``generated/autoconf.h``
- NAPI use new struct ``napi_struct`` and rename API from ``netif_rx_*`` to ``napi_*``
- ``alloc_netdev`` use ``name_assign_type``
  
  - `net: add name_assign_type netdev attribute <https://lkml.org/lkml/2014/7/10/88>`_
  - We can use ``NET_NAME_UNKNOWN`` for device construction.
