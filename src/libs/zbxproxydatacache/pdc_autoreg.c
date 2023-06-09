/*
** Zabbix
** Copyright (C) 2001-2023 Zabbix SIA
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
**/

#include "pdc_autoreg.h"
#include "zbxproxydatacache.h"
#include "zbxdbhigh.h"

static void	pdc_autoreg_write_host_db(const char *host, const char *ip, const char *dns, unsigned short port,
		unsigned int connection_type, const char *host_metadata, int flags, int clock)
{
	zbx_db_insert_t	db_insert;
	zbx_uint64_t	id;

	id = zbx_db_get_maxid("proxy_autoreg_host");

	zbx_db_insert_prepare(&db_insert, "proxy_autoreg_host", "id", "host", "listen_ip", "listen_dns", "listen_port",
			"tls_accepted", "host_metadata", "flags", "clock", NULL);

	zbx_db_insert_add_values(&db_insert, id, host, ip, dns, (int)port, (int)connection_type, host_metadata, flags,
			clock);

	zbx_db_insert_execute(&db_insert);
	zbx_db_insert_clean(&db_insert);
}

/******************************************************************************
 *                                                                            *
 * Purpose: write host data into autoregistraion data cache                   *
 *                                                                            *
 ******************************************************************************/
void	zbx_pdc_autoreg_write_host(const char *host, const char *ip, const char *dns,
		unsigned short port, unsigned int connection_type, const char *host_metadata, int flags,
		int clock)
{
	if (PDC_MEMORY == pdc_dst[pdc_cache->state])
	{
		zabbix_log(LOG_LEVEL_WARNING, "proxy data memory cache not implemented, switching to database");
		pdc_cache->state = PDC_DATABASE_ONLY;
		/* TODO: change to 'else' after memory cache implementation */
	}

	if (PDC_DATABASE == pdc_dst[pdc_cache->state])
		pdc_autoreg_write_host_db(host, ip, dns, port, connection_type, host_metadata, flags, clock);
}
