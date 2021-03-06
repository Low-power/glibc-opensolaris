/* Copyright (C) 1996, 1997, 1998, 1999, 2000, 2008
    Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#ifndef _NETDB_H
# error "Never include <bits/netdb.h> directly; use <netdb.h> instead."
#endif


/* Description of data base entry for a single network.  NOTE: here a
   poor assumption is made.  The network number is expected to fit
   into an unsigned long int variable.  */
struct netent
{
  char *n_name;			/* Official name of network.  */
  char **n_aliases;		/* Alias list.  */
  int n_addrtype;		/* Net address type.  */
  uint32_t n_net;		/* Network number.  */
};

#define MAXALIASES	35
#define MAXADDRS	35

#ifdef __USE_MISC

typedef struct ipsecalgent
{
  char **a_names;
  int a_proto_num;
  int a_alg_num;
  char *a_mech_name;
  int *a_block_sizes;
  int *a_key_sizes;
  int a_key_increment;
} ipsecalgent_t;

#define IPSEC_PROTO_AH	2
#define IPSEC_PROTO_ESP	3

#define MAXHOSTNAMELEN	256

#ifdef __USE_MISC
# define AI_DEFAULT	(AI_V4MAPPED | AI_ADDRCONFIG)
#endif

#endif /* __USE_MISC */
