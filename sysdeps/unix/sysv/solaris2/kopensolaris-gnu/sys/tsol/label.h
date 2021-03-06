/* Copyright (C) 2008, 2009 Free Software Foundation, Inc.
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

#ifndef _SYS_TSOL_LABEL_H
#define _SYS_TSOL_LABEL_H

#include <sys/types.h>

typedef struct _mac_label_impl m_label_t;
typedef m_label_t blevel_t;
typedef m_label_t bslabel_t;
typedef m_label_t bclear_t;

typedef struct _tsol_binary_level_lrange
  {
	m_label_t *lower_bound;
	m_label_t *upper_bound;
  } m_range_t;

typedef m_range_t blrange_t;

typedef struct tsol_mlp_s
  {
	unsigned char mlp_ipp;
	uint16_t mlp_port;
	uint16_t mlp_port_upper;
  } tsol_mlp_t;

#endif /* _SYS_TSOL_LABEL_H */
