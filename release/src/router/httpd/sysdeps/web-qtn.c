/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
/*
 * ASUS Home Gateway Reference Design
 * Web Page Configuration Support Routines
 *
 * Copyright 2014, ASUSTeK Inc.
 * All Rights Reserved.
 *
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of ASUSTeK Inc.;
 * the contents of this file may not be disclosed to third parties, copied or
 * duplicated in any form, in whole or in part, without the prior written
 * permission of ASUSTeK Inc..
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <bcmnvram.h>
#include <wlioctl.h>

#include <httpd.h>

#include <shutils.h>
#include <shared.h>

#include "web-qtn.h"

#define MAX_RETRY_TIMES 30
#define WIFINAME "wifi0"

static int s_c_rpc_use_udp = 0;

extern uint8 wf_chspec_ctlchan(chanspec_t chspec);
extern chanspec_t wf_chspec_aton(const char *a);
extern char *wl_ether_etoa(const struct ether_addr *n);

int rpc_qcsapi_init()
{
	const char *host;
	CLIENT *clnt;
	int retry = 0;

	nvram_set("111", "222");
	/* setup RPC based on udp protocol */
	do {
		dbG("#%d attempt to create RPC connection\n", retry + 1);

		host = client_qcsapi_find_host_addr(0, NULL);
		if (!host) {
			dbG("Cannot find the host\n");
			sleep(1);
			continue;
		}

		if (!s_c_rpc_use_udp) {
			clnt = clnt_create(host, QCSAPI_PROG, QCSAPI_VERS, "tcp");
		} else {
			clnt = clnt_create(host, QCSAPI_PROG, QCSAPI_VERS, "udp");
		}

		if (clnt == NULL) {
			clnt_pcreateerror(host);
			sleep(1);
			continue;
		} else {
			client_qcsapi_set_rpcclient(clnt);
			return 0;
		}
	} while (retry++ < MAX_RETRY_TIMES);

	return -1;
}

int rpc_qcsapi_set_SSID(const char *ssid)
{
	int ret;

	ret = qcsapi_wifi_set_SSID(WIFINAME, ssid);
	if (ret < 0) {
		dbG("Qcsapi qcsapi_wifi_set_SSID error, return: %d\n", ret);
		return ret;
	}
	dbG("Set SSID as: %s\n", ssid);

	return 0;
}

int rpc_qcsapi_set_SSID_broadcast(const char *option)
{
	int ret;
	int OPTION = 1 - atoi(option);

	ret = qcsapi_wifi_set_option(WIFINAME, qcsapi_SSID_broadcast, OPTION);
	if (ret < 0) {
		dbG("Qcsapi qcsapi_wifi_set_option::SSID_broadcast error, return: %d\n", ret);
		return ret;
	}
	dbG("Set Broadcast SSID as: %s\n", OPTION ? "TRUE" : "FALSE");

	return 0;
}

int rpc_qcsapi_set_vht(const char *mode)
{
	int ret;
	int VHT;

	switch (atoi(mode))
	{
		case 0:
			VHT = 1;
			break;
		default:
			VHT = 0;
			break;
	}

	ret = qcsapi_wifi_set_vht(WIFINAME, VHT);
	if (ret < 0) {
		dbG("Qcsapi qcsapi_wifi_set_vht error, return: %d\n", ret);
		return ret;
	}
	dbG("Set wireless mode as: %s\n", VHT ? "11ac" : "11n");

	return 0;
}

int rpc_qcsapi_set_bw(const char *bw)
{
	int ret;
	int BW = 20;

	switch (atoi(bw))
	{
		case 1:
			BW = 20;
			break;
		case 2:
			BW = 40;
			break;
		case 0:
		case 3:
			BW = 80;
			break;
	}

	ret = qcsapi_wifi_set_bw(WIFINAME, BW);
	if (ret < 0) {
		dbG("Qcsapi qcsapi_wifi_set_bw error, return: %d\n", ret);
		return ret;
	}
	dbG("Set bw as: %d MHz\n", BW);

	return 0;
}

int rpc_qcsapi_set_channel(const char *chspec_buf)
{
	int ret;
	int channel = 0;

	channel = wf_chspec_ctlchan(wf_chspec_aton(chspec_buf));

	ret = qcsapi_wifi_set_channel(WIFINAME, channel);
	if (ret < 0) {
		dbG("Qcsapi qcsapi_wifi_set_channel error, return: %d\n", ret);
		return ret;
	}
	dbG("Set channel as: %d\n", channel);

	return 0;
}

int rpc_qcsapi_set_beacon_type(const char *auth_mode)
{
	int ret;
	char *p_new_beacon = NULL;

	if (!strcmp(auth_mode, "open"))
		p_new_beacon = strdup("Basic");
        else if (!strcmp(auth_mode, "psk"))
		p_new_beacon = strdup("WPA");
        else if (!strcmp(auth_mode, "psk2"))
		p_new_beacon = strdup("11i");
        else if (!strcmp(auth_mode, "pskpsk2"))
		p_new_beacon = strdup("WPAand11i");
	else
		p_new_beacon = strdup("Basic");

	ret = qcsapi_wifi_set_beacon_type(WIFINAME, p_new_beacon);
	if (ret < 0) {
		dbG("Qcsapi qcsapi_wifi_set_beacon_type error, return: %d\n", ret);
		return ret;
	}
	dbG("Set beacon type as: %s\n", p_new_beacon);

	if (p_new_beacon) free(p_new_beacon);

	return 0;
}

int rpc_qcsapi_set_WPA_encryption_modes(const char *crypto)
{
	int ret;
	string_32 encryption_modes;

	if (!strcmp(crypto, "tkip"))
		strcpy(encryption_modes, "TKIPEncryption");
        else if (!strcmp(crypto, "aes"))
		strcpy(encryption_modes, "AESEncryption");
        else if (!strcmp(crypto, "tkip+aes"))
		strcpy(encryption_modes, "TKIPandAESEncryption");
	else
		strcpy(encryption_modes, "AESEncryption");

	ret = qcsapi_wifi_set_WPA_encryption_modes(WIFINAME, encryption_modes);
	if (ret < 0) {
		dbG("Qcsapi qcsapi_wifi_set_WPA_encryption_modes error, return: %d\n", ret);
		return ret;
	}
	dbG("Set WPA encryption mode as: %s\n", encryption_modes);

	return 0;
}

int rpc_qcsapi_set_key_passphrase(const char *wpa_psk)
{
	int ret;

	ret = qcsapi_wifi_set_key_passphrase(WIFINAME, 0, wpa_psk);
	if (ret < 0) {
		dbG("Qcsapi qcsapi_wifi_set_key_passphrase error, return: %d\n", ret);
		return ret;
	}
	dbG("Set WPA preshared key as: %s\n", wpa_psk);

	return 0;
}

int rpc_qcsapi_set_dtim(const char *dtim)
{
	int ret;
	int DTIM = atoi(dtim);

	ret = qcsapi_wifi_set_dtim(WIFINAME, DTIM);
	if (ret < 0) {
		dbG("Qcsapi qcsapi_wifi_set_dtim error, return: %d\n", ret);
		return ret;
	}
	dbG("Set dtim as: %d\n", DTIM);

	return 0;
}

int rpc_qcsapi_set_beacon_interval(const char *beacon_interval)
{
	int ret;
	int BCN = atoi(beacon_interval);

	ret = qcsapi_wifi_set_beacon_interval(WIFINAME, BCN);
	if (ret < 0) {
		dbG("Qcsapi qcsapi_wifi_set_beacon_interval error, return: %d\n", ret);
		return ret;
	}
	dbG("Set beacon_interval as: %d\n", BCN);

	return 0;
}

int rpc_qcsapi_set_mac_address_filtering(const char *ifname, const char *mac_address_filtering)
{
	int ret;
	qcsapi_mac_address_filtering MAF;
	qcsapi_mac_address_filtering orig_mac_address_filtering;

	ret = rpc_qcsapi_get_mac_address_filtering(ifname, &orig_mac_address_filtering);
	if (ret < 0) {
		dbG("Qcsapi qcsapi_wifi_get_mac_address_filtering error, return: %d\n", ret);
		return ret;
	}
	dbG("Original mac_address_filtering setting: %d\n", orig_mac_address_filtering);

	if (!strcmp(mac_address_filtering, "disabled"))
		MAF = qcsapi_disable_mac_address_filtering;
	else if (!strcmp(mac_address_filtering, "deny"))
		MAF = qcsapi_accept_mac_address_unless_denied;
	else if (!strcmp(mac_address_filtering, "allow"))
		MAF = qcsapi_deny_mac_address_unless_authorized;
	else
		MAF = qcsapi_disable_mac_address_filtering;

	ret = qcsapi_wifi_set_mac_address_filtering(ifname, MAF);
	if (ret < 0) {
		dbG("Qcsapi qcsapi_wifi_set_mac_address_filtering error, return: %d\n", ret);
		return ret;
	}
	dbG("Set mac_address_filtering as: %d (%s)\n", MAF, mac_address_filtering);

	if ((orig_mac_address_filtering == 0) && (mac_address_filtering != 0))
		rpc_update_wlmaclist(ifname);

	return 0;
}

int rpc_qcsapi_authorize_mac_address(const char *ifname, const char *macaddr)
{
	int ret;
	qcsapi_mac_addr address_to_authorize;

	ether_atoe(macaddr, address_to_authorize);
	ret = qcsapi_wifi_authorize_mac_address(ifname, address_to_authorize);
	if (ret < 0) {
		dbG("Qcsapi qcsapi_wifi_authorize_mac_address error, return: %d\n", ret);
		return ret;
	}
//	dbG("authorize MAC addresss: %s\n", macaddr);

	return 0;
}

int rpc_qcsapi_deny_mac_address(const char *ifname, const char *macaddr)
{
	int ret;
	qcsapi_mac_addr address_to_deny;

	ether_atoe(macaddr, address_to_deny);
	ret = qcsapi_wifi_deny_mac_address(ifname, address_to_deny);
	if (ret < 0) {
		dbG("Qcsapi qcsapi_wifi_deny_mac_address error, return: %d\n", ret);
		return ret;
	}
//	dbG("deny MAC addresss: %s\n", macaddr);

	return 0;
}

int rpc_qcsapi_wds_set_psk(const char *macaddr, const char *wpa_psk)
{
	int ret;
	qcsapi_mac_addr peer_address;

	ether_atoe(macaddr, peer_address);
	ret = qcsapi_wds_set_psk(WIFINAME, peer_address, wpa_psk);
	if (ret < 0) {
		dbG("Qcsapi qcsapi_wds_set_psk error, return: %d\n", ret);
		return ret;
	}
	dbG("remove WDS Peer: %s\n", macaddr);

	return 0;
}

int rpc_qcsapi_get_SSID(qcsapi_SSID *ssid)
{
	int ret;

	ret = qcsapi_wifi_get_SSID(WIFINAME, (char *) ssid);
	if (ret < 0) {
		dbG("Qcsapi qcsapi_wifi_get_bw error, return: %d\n", ret);
		return ret;
	}

	return 0;
}

int rpc_qcsapi_get_SSID_broadcast(int *p_current_option)
{
	int ret;

	ret = qcsapi_wifi_get_option(WIFINAME, qcsapi_SSID_broadcast, p_current_option);
	if (ret < 0) {
		dbG("Qcsapi qcsapi_wifi_get_option::SSID_broadcast error, return: %d\n", ret);
		return ret;
	}

	return 0;
}

int rpc_qcsapi_get_vht(qcsapi_unsigned_int *vht)
{
	int ret;

	ret = qcsapi_wifi_get_vht(WIFINAME, vht);
	if (ret < 0) {
		dbG("Qcsapi qcsapi_wifi_get_vht error, return: %d\n", ret);
		return ret;
	}

	return 0;
}

int rpc_qcsapi_get_bw(qcsapi_unsigned_int *p_bw)
{
	int ret;

	ret = qcsapi_wifi_get_bw(WIFINAME, p_bw);
	if (ret < 0) {
		dbG("Qcsapi qcsapi_wifi_get_bw error, return: %d\n", ret);
		return ret;
	}

	return 0;
}

int rpc_qcsapi_get_channel(qcsapi_unsigned_int *p_channel)
{
	int ret;

	ret = qcsapi_wifi_get_channel(WIFINAME, p_channel);
	if (ret < 0) {
		dbG("Qcsapi qcsapi_wifi_get_channel error, return: %d\n", ret);
		return ret;
	}

	return 0;
}

int rpc_qcsapi_get_channel_list(string_1024* list_of_channels)
{
	int ret;

	ret = qcsapi_wifi_get_list_channels(WIFINAME, *list_of_channels);
	if (ret < 0) {
		dbG("Qcsapi qcsapi_wifi_get_list_channels error, return: %d\n", ret);
		return ret;
	}

	return 0;
}

int rpc_qcsapi_get_beacon_type(char *p_current_beacon)
{
	int ret;

	ret = qcsapi_wifi_get_beacon_type(WIFINAME, p_current_beacon);
	if (ret < 0) {
		dbG("Qcsapi qcsapi_wifi_get_beacon_type error, return: %d\n", ret);
		return ret;
	}

	return 0;
}

int rpc_qcsapi_get_WPA_encryption_modes(char *p_current_encryption_mode)
{
	int ret;

	ret = qcsapi_wifi_get_WPA_encryption_modes(WIFINAME, p_current_encryption_mode);
	if (ret < 0) {
		dbG("Qcsapi qcsapi_wifi_get_WPA_encryption_modes error, return: %d\n", ret);
		return ret;
	}

	return 0;
}

int rpc_qcsapi_get_key_passphrase(char *p_current_key_passphrase)
{
	int ret;

	ret = qcsapi_wifi_get_key_passphrase(WIFINAME, 0, p_current_key_passphrase);
	if (ret < 0) {
		dbG("Qcsapi qcsapi_wifi_get_key_passphrase error, return: %d\n", ret);
		return ret;
	}

	return 0;
}

int rpc_qcsapi_get_dtim(qcsapi_unsigned_int *p_dtim)
{
	int ret;

	ret = qcsapi_wifi_get_dtim(WIFINAME, p_dtim);
	if (ret < 0) {
		dbG("Qcsapi qcsapi_wifi_get_dtim error, return: %d\n", ret);
		return ret;
	}

	return 0;
}

int rpc_qcsapi_get_beacon_interval(qcsapi_unsigned_int *p_beacon_interval)
{
	int ret;

	ret = qcsapi_wifi_get_beacon_interval(WIFINAME, p_beacon_interval);
	if (ret < 0) {
		dbG("Qcsapi qcsapi_wifi_get_beacon_interval error, return: %d\n", ret);
		return ret;
	}

	return 0;
}

int rpc_qcsapi_get_mac_address_filtering(const char* ifname, qcsapi_mac_address_filtering *p_mac_address_filtering)
{
	int ret;

	ret = qcsapi_wifi_get_mac_address_filtering(ifname, p_mac_address_filtering);
	if (ret < 0) {
		dbG("Qcsapi qcsapi_wifi_get_mac_address_filtering error, return: %d\n", ret);
		return ret;
	}

	return 0;
}

int rpc_qcsapi_get_authorized_mac_addresses(const char *ifname, char *list_mac_addresses, const unsigned int sizeof_list)
{
	int ret;

	ret = qcsapi_wifi_get_authorized_mac_addresses(ifname, list_mac_addresses, sizeof_list);
	if (ret < 0) {
		dbG("Qcsapi qcsapi_wifi_get_authorized_mac_addresses error, return: %d\n", ret);
		return ret;
	}

	return 0;
}

int rpc_qcsapi_get_denied_mac_addresses(const char *ifname, char *list_mac_addresses, const unsigned int sizeof_list)
{
	int ret;

	ret = qcsapi_wifi_get_denied_mac_addresses(ifname, list_mac_addresses, sizeof_list);
	if (ret < 0) {
		dbG("Qcsapi qcsapi_wifi_get_denied_mac_addresses error, return: %d\n", ret);
		return ret;
	}

	return 0;
}

int rpc_qcsapi_interface_get_mac_addr(const char *ifname, qcsapi_mac_addr *current_mac_addr)
{
	int ret;

	ret = qcsapi_interface_get_mac_addr(ifname, (uint8_t *) current_mac_addr);
	if (ret < 0) {
		dbG("Qcsapi qcsapi_interface_get_mac_addr error, return: %d\n", ret);
		return ret;
	}

	return 0;
}

int rpc_qcsapi_restore_default_config(int flag)
{
	int ret;

	ret = qcsapi_restore_default_config(flag);
	if (ret < 0) {
		dbG("Qcsapi qcsapi_restore_default_config error, return: %d\n", ret);
		return ret;
	}
	dbG("QTN restore default config successfully\n");

	return 0;
}

int rpc_qcsapi_bootcfg_commit(void)
{
	int ret;

	ret = qcsapi_bootcfg_commit();
	if (ret < 0) {
		dbG("Qcsapi qcsapi_bootcfg_commit error, return: %d\n", ret);
		return ret;
	}
	dbG("QTN commi bootcfg successfully\n");

	return 0;
}

void rpc_show_config(void)
{
	int ret;
	char mac_address_filtering_mode[][8] = {"Disable", "Reject", "Accept"};

	qcsapi_SSID ssid;
	ret = rpc_qcsapi_get_SSID(&ssid);
	if (ret < 0)
		dbG("rpc_qcsapi_get_SSID error, return: %d\n", ret);
	else
		dbG("current SSID: %s\n", ssid);

	int current_option;
	ret = rpc_qcsapi_get_SSID_broadcast(&current_option);
	if (ret < 0)
		dbG("rpc_qcsapi_get_SSID_broadcast error, return: %d\n", ret);
	else
		dbG("current SSID broadcast option: %s\n", current_option ? "TRUE" : "FALSE");

	qcsapi_unsigned_int vht;
	ret = rpc_qcsapi_get_vht(&vht);
	if (ret < 0)
		dbG("rpc_qcsapi_get_vht error, return: %d\n", ret);
	else
		dbG("current wireless mode: %s\n", (unsigned int) vht ? "11ac" : "11n");

	qcsapi_unsigned_int bw;
	ret = rpc_qcsapi_get_bw(&bw);
	if (ret < 0)
		dbG("rpc_qcsapi_get_bw error, return: %d\n", ret);
	else
		dbG("current channel bandwidth: %d MHz\n", bw);

	qcsapi_unsigned_int channel;
	ret = rpc_qcsapi_get_channel(&channel);
	if (ret < 0)
		dbG("rpc_qcsapi_get_channel error, return: %d\n", ret);
	else
		dbG("current channel: %d\n", channel);

	string_1024 list_of_channels;
	ret = rpc_qcsapi_get_channel_list(&list_of_channels);
	if (ret < 0)
		dbG("rpc_qcsapi_get_channel_list error, return: %d\n", ret);
	else
		dbG("current channel list: %s\n", list_of_channels);

	string_16 current_beacon_type;
	ret = rpc_qcsapi_get_beacon_type((char *) &current_beacon_type);
	if (ret < 0)
		dbG("rpc_qcsapi_get_beacon_type error, return: %d\n", ret);
	else
		dbG("current beacon type: %s\n", current_beacon_type);

	string_32 encryption_mode;
	ret = rpc_qcsapi_get_WPA_encryption_modes((char *) &encryption_mode);
	if (ret < 0)
		dbG("rpc_qcsapi_get_WPA_encryption_modes error, return: %d\n", ret);
	else
		dbG("current WPA encryption mode: %s\n", encryption_mode);

	string_64 key_passphrase;
	ret = rpc_qcsapi_get_key_passphrase((char *) &key_passphrase);
	if (ret < 0)
		dbG("rpc_qcsapi_get_key_passphrase error, return: %d\n", ret);
	else
		dbG("current WPA preshared key: %s\n", key_passphrase);

	qcsapi_unsigned_int dtim;
	ret = rpc_qcsapi_get_dtim(&dtim);
	if (ret < 0)
		dbG("rpc_qcsapi_get_dtim error, return: %d\n", ret);
	else
		dbG("current DTIM interval: %d\n", dtim);

	qcsapi_unsigned_int beacon_interval;
	ret = rpc_qcsapi_get_beacon_interval(&beacon_interval);
	if (ret < 0)
		dbG("rpc_qcsapi_get_beacon_interval error, return: %d\n", ret);
	else
		dbG("current Beacon interval: %d\n", beacon_interval);

	qcsapi_mac_address_filtering mac_address_filtering;
	ret = rpc_qcsapi_get_mac_address_filtering(WIFINAME, &mac_address_filtering);
	if (ret < 0)
		dbG("rpc_qcsapi_get_mac_address_filtering error, return: %d\n", ret);
	else
		dbG("current MAC filter mode: %s\n", mac_address_filtering_mode[mac_address_filtering]);
}

void rpc_parse_nvram(const char *name, const char *value)
{
	if (!strcmp(name, "wl1_ssid"))
		rpc_qcsapi_set_SSID(value);
	else if (!strcmp(name, "wl1_closed"))
		rpc_qcsapi_set_SSID_broadcast(value);
	else if (!strcmp(name, "wl1_nmode_x"))
		rpc_qcsapi_set_vht(value);
	else if (!strcmp(name, "wl1_bw"))
		rpc_qcsapi_set_bw(value);
	else if (!strcmp(name, "wl1_chanspec"))
	{
		rpc_qcsapi_set_channel(value);
		rpc_qcsapi_set_channel(value);
	}
	else if (!strcmp(name, "wl1_auth_mode_x"))
		rpc_qcsapi_set_beacon_type(value);
	else if (!strcmp(name, "wl1_crypto"))
		rpc_qcsapi_set_WPA_encryption_modes(value);
	else if (!strcmp(name, "wl1_wpa_psk"))
		rpc_qcsapi_set_key_passphrase(value);
	else if (!strcmp(name, "wl1_dtim"))
		rpc_qcsapi_set_dtim(value);
	else if (!strcmp(name, "wl1_bcn"))
		rpc_qcsapi_set_beacon_interval(value);
	else if (!strcmp(name, "wl1_macmode"))
		rpc_qcsapi_set_mac_address_filtering(WIFINAME, value);
	else if (!strcmp(name, "wl1_maclist_x"))
		rpc_update_wlmaclist(WIFINAME);
	else if (!strcmp(name, "wl1_mode_x"))
		rpc_update_wdslist();
	else if (!strcmp(name, "wl1_wdslist"))
		rpc_update_wdslist();
	else if (!strcmp(name, "wl1_wds_psk"))
		rpc_update_wds_psk();

	rpc_show_config();
}

void rpc_update_wlmaclist(const char *ifname)
{
	int ret;
	qcsapi_mac_address_filtering mac_address_filtering;
	char list_mac_addresses[1024];
        char *m = NULL;
        char *p, *pp;

	ret = rpc_qcsapi_get_mac_address_filtering(ifname, &mac_address_filtering);
	if (ret < 0)
		dbG("rpc_qcsapi_get_mac_address_filtering error, return: %d\n", ret);
	else
	{
		if (mac_address_filtering == qcsapi_accept_mac_address_unless_denied)
		{
//			ret = rpc_qcsapi_get_denied_mac_addresses(ifname, list_mac_addresses, sizeof(list_mac_addresses));
//			if (ret < 0)
//				dbG("rpc_qcsapi_get_denied_mac_addresses error, return: %d\n", ret);
//			else
			{
//				dbG("current denied MAC addresses: %s\n", list_mac_addresses);
#if 0
				if (strlen(list_mac_addresses))
				{
					pp = p = strdup(list_mac_addresses);
					while ((m = strsep(&p, ",")) != NULL) {
						rpc_qcsapi_authorize_mac_address(ifname, m);
					}
					free(pp);
				}
#else
				ret = qcsapi_wifi_clear_mac_address_filters(WIFINAME);
				if (ret < 0)
					dbG("qcsapi_wifi_clear_mac_address_filters error, return: %d\n", ret);
#endif
				pp = p = strdup(nvram_safe_get("wl1_maclist_x"));
				if (pp) {
					while ((m = strsep(&p, "<")) != NULL) {
						if (!strlen(m)) continue;
						rpc_qcsapi_deny_mac_address(ifname, m);
					}
					free(pp);
				}

				ret = rpc_qcsapi_get_denied_mac_addresses(ifname, list_mac_addresses, sizeof(list_mac_addresses));
				if (ret < 0)
					dbG("rpc_qcsapi_get_denied_mac_addresses error, return: %d\n", ret);
				else
					dbG("current denied MAC addresses: %s\n", list_mac_addresses);
			}
		}
		else if (mac_address_filtering == qcsapi_deny_mac_address_unless_authorized)
		{
//			ret = rpc_qcsapi_get_authorized_mac_addresses(ifname, list_mac_addresses, sizeof(list_mac_addresses));
//			if (ret < 0)
//				dbG("rpc_qcsapi_get_authorized_mac_addresses error, return: %d\n", ret);
//			else
			{
//				dbG("current authorized MAC addresses: %s\n", list_mac_addresses);
#if 0
				if (strlen(list_mac_addresses))
				{
					pp = p = strdup(list_mac_addresses);
					while ((m = strsep(&p, ",")) != NULL) {
						rpc_qcsapi_deny_mac_address(ifname, m);
					}
					free(pp);
				}
#else
				ret = qcsapi_wifi_clear_mac_address_filters(WIFINAME);
				if (ret < 0)
					dbG("qcsapi_wifi_clear_mac_address_filters error, return: %d\n", ret);
#endif
				pp = p = strdup(nvram_safe_get("wl1_maclist_x"));
				if (pp) {
					while ((m = strsep(&p, "<")) != NULL) {
						if (!strlen(m)) continue;
						rpc_qcsapi_authorize_mac_address(ifname, m);
					}
					free(pp);
				}

				ret = rpc_qcsapi_get_authorized_mac_addresses(ifname, list_mac_addresses, sizeof(list_mac_addresses));
				if (ret < 0)
					dbG("rpc_qcsapi_get_authorized_mac_addresses error, return: %d\n", ret);
				else
					dbG("current authorized MAC addresses: %s\n", list_mac_addresses);
			}
		}
	}
}

void rpc_update_wdslist()
{
	int ret, i;
	qcsapi_mac_addr peer_address;
        char *m = NULL;
        char *p, *pp;

	for (i = 0; i < 8; i++)
	{
		ret = qcsapi_wds_get_peer_address(WIFINAME, 0, (uint8_t *) &peer_address);
		if (ret < 0)
		{
			if (ret == -19) break;	// No such device
			dbG("qcsapi_wds_get_peer_address error, return: %d\n", ret);
		}
		else
		{
//			dbG("current WDS peer index 0 addresse: %s\n", wl_ether_etoa((struct ether_addr *) &peer_address));
			ret = qcsapi_wds_remove_peer(WIFINAME, peer_address);
			if (ret < 0)
				dbG("qcsapi_wds_remove_peer error, return: %d\n", ret);
		}
	}

	if (nvram_match("wl1_mode_x", "0"))
		return;

	pp = p = strdup(nvram_safe_get("wl1_wdslist"));
	if (pp) {
		while ((m = strsep(&p, "<")) != NULL) {
			if (!strlen(m)) continue;

			ether_atoe(m, peer_address);
			ret = qcsapi_wds_add_peer(WIFINAME, peer_address);
			if (ret < 0)
				dbG("qcsapi_wds_add_peer error, return: %d\n", ret);
			else
			{
				ret = rpc_qcsapi_wds_set_psk(m, nvram_safe_get("wl1_wds_psk"));
				if (ret < 0)
					dbG("rpc_qcsapi_wds_set_psk error, return: %d\n", ret);
			}
		}
		free(pp);
	}

	for (i = 0; i < 8; i++)
	{
		ret = qcsapi_wds_get_peer_address(WIFINAME, i, (uint8_t *) &peer_address);
		if (ret < 0)
		{
			if (ret == -19) break;	// No such device
			dbG("qcsapi_wds_get_peer_address error, return: %d\n", ret);
		}
		else
			dbG("current WDS peer index 0 addresse: %s\n", wl_ether_etoa((struct ether_addr *) &peer_address));
	}
}

void rpc_update_wds_psk()
{
	int ret, i;
	qcsapi_mac_addr peer_address;

	if (nvram_match("wl1_mode_x", "0"))
		return;

	for (i = 0; i < 8; i++)
	{
		ret = qcsapi_wds_get_peer_address(WIFINAME, i, (uint8_t *) &peer_address);
		if (ret < 0)
		{
			if (ret == -19) break;	// No such device
			dbG("qcsapi_wds_get_peer_address error, return: %d\n", ret);
		}
		else
		{
//			dbG("current WDS peer index 0 addresse: %s\n", wl_ether_etoa((struct ether_addr *) &peer_address));
			ret = rpc_qcsapi_wds_set_psk(wl_ether_etoa((struct ether_addr *) &peer_address), nvram_safe_get("wl1_wds_psk"));
			if (ret < 0)
				dbG("rpc_qcsapi_wds_set_psk error, return: %d\n", ret);
		}
	}
}

int
ej_wl_channel_list_5g(int eid, webs_t wp, int argc, char_t **argv)
{
	int ret;
	int retval = 0;
	char tmp[256];
	string_1024 list_of_channels;
	char *p;
	int i = 0;;

	sprintf(tmp, "[\"%d\"]", 0);

	ret = qcsapi_wifi_get_list_channels(WIFINAME, (char *) &list_of_channels);
	if (ret < 0) {
		dbG("Qcsapi qcsapi_wifi_get_list_channels error, return: %d\n", ret);
		goto ERROR;
	}

	p = strtok((char *) list_of_channels, ",");
	while (p)
	{
		if (i == 0)
			sprintf(tmp, "[\"%s\"", (char *) p);
		else
			sprintf(tmp,  "%s, \"%s\"", tmp, (char *) p);

		p = strtok(NULL, ",");
		i++;
	}

	if (i)
		sprintf(tmp,  "%s]", tmp);

ERROR:
	retval += websWrite(wp, "%s", tmp);
	return retval;
}

int
ej_wl_scan_5g(int eid, webs_t wp, int argc, char_t **argv)
{
	char ssid_str[128];
	char macstr[18];
	int retval = 0;
	int ret, i, j;
	unsigned int ap_count = 0;
	qcsapi_ap_properties ap_current;

	ret = qcsapi_wifi_start_scan(WIFINAME);
	if (ret < 0) {
		dbG("Qcsapi qcsapi_wifi_start_scan error, return: %d\n", ret);
		retval += websWrite(wp, "[]");
		return retval;
	}

	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < i+1; j++)
			dbg(".");
		sleep(1);
		dbg("\n");
	}

	//Get the scaned AP count
	ret = qcsapi_wifi_get_results_AP_scan(WIFINAME, &ap_count);
	if (ret < 0) {
		dbG("Qcsapi qcsapi_wifi_get_results_AP_scan error, return: %d\n", ret);
		retval += websWrite(wp, "[]");
		return retval;
	}
	if (ap_count == 0) {
		dbG("Scaned ap count is 0\n");
		retval += websWrite(wp, "[]");
		return retval;
	}

	dbg("%-4s%-33s%-18s\n\n", "Ch", "SSID", "BSSID");

	retval += websWrite(wp, "[");

	for (i = 0; i < ap_count; i++) {
		ret = qcsapi_wifi_get_properties_AP(WIFINAME, i, &ap_current);
		if (ret < 0) {
			dbG("Qcsapi qcsapi_wifi_get_properties_AP error, return: %d\n", ret);
			goto END;
		}

		memset(ssid_str, 0, sizeof(ssid_str));
		char_to_ascii(ssid_str, ap_current.ap_name_SSID);

		sprintf(macstr, "%02X:%02X:%02X:%02X:%02X:%02X",
			ap_current.ap_mac_addr[0],
			ap_current.ap_mac_addr[1],
			ap_current.ap_mac_addr[2],
			ap_current.ap_mac_addr[3],
			ap_current.ap_mac_addr[4],
			ap_current.ap_mac_addr[5]);

		dbg("%-4d%-33s%-18s\n",
			ap_current.ap_channel,
			ap_current.ap_name_SSID,
			macstr
		);

		if (!i)
			retval += websWrite(wp, "[\"%s\", \"%s\"]", ssid_str, macstr);
		else
			retval += websWrite(wp, ", [\"%s\", \"%s\"]", ssid_str, macstr);
	}

	dbg("\n");
END:
	retval += websWrite(wp, "]");
	return retval;
}

int
ej_wl_sta_list_5g(int eid, webs_t wp, int argc, char_t **argv)
{
	int ret, retval = 0;
	qcsapi_unsigned_int association_count;
	qcsapi_mac_addr sta_address;
	int i, firstRow = 1;

	ret = qcsapi_wifi_get_count_associations(WIFINAME, &association_count);
	if (ret < 0) {
		dbG("Qcsapi qcsapi_wifi_get_count_associations, return: %d\n", ret);
		return retval;
	} else {
		for (i = 0; i < association_count; ++i) {
			ret = qcsapi_wifi_get_associated_device_mac_addr(WIFINAME, i, (uint8_t *) &sta_address);
			if (ret < 0) {
				dbG("Qcsapi qcsapi_wifi_get_associated_device_mac_addr, return: %d\n", ret);
				return retval;
			} else {
				if (firstRow == 1)
					firstRow = 0;
				else
					retval += websWrite(wp, ", ");

				retval += websWrite(wp, "[");

				retval += websWrite(wp, "\"%s\"", wl_ether_etoa((struct ether_addr *) &sta_address));
				retval += websWrite(wp, ", \"%s\"", "Yes");
				retval += websWrite(wp, ", \"%s\"", !nvram_match("wl1_auth_mode_x", "open") ? "Yes" : "No");

				retval += websWrite(wp, "]");
			}
		}
	}

	return retval;
}
