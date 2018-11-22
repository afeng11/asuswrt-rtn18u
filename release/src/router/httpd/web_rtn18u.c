#include <stdio.h>

#include <httpd.h>
#include <json.h>


static struct stb_port stb_x_options[] = {
	{ .name = "none", .value = "0" },
	{ .name = "LAN1", .value = "1" },
	{ .name = "LAN2", .value = "2" },
	{ .name = "LAN3", .value = "3" },
	{ .name = "LAN4", .value = "4" },
	{ .name = "LAN1 & LAN2", .value = "5" },
	{ .name = "LAN3 & LAN4", .value = "6" },
};
static unsigned int num_stb_x_option = sizeof(stb_x_options)/sizeof(*stb_x_options);

static struct iptv_profile isp_profiles[] = {
	{
		.profile_name = "none",
		.iptv_port = "",
		.voip_port = "",
		.bridge_port = "",
		.iptv_config = "0",
		.voip_config = "0",
		.switch_wantag = "none",
		.switch_stb_x = "0",
		.switch_wan0tagid = "", .switch_wan0prio = "",
		.switch_wan1tagid = "", .switch_wan1prio = "",
		.switch_wan2tagid = "", .switch_wan2prio = "",
		.mr_enable_x = "",
		.emf_enable = "",
		.wan_vpndhcp = "",
		.quagga_enable = "0",
		.mr_altnet_x = "",
		.ttl_inc_enable = "0"
	},
	{
		.profile_name = "Unifi-Business",
		.iptv_port = "",
		.voip_port = "",
		.bridge_port = "",
		.iptv_config = "0",
		.voip_config = "0",
		.switch_wantag = "unifi_biz",
		.switch_stb_x = "0",
		.switch_wan0tagid = "", .switch_wan0prio = "",
		.switch_wan1tagid = "", .switch_wan1prio = "",
		.switch_wan2tagid = "", .switch_wan2prio = "",
		.mr_enable_x = "",
		.emf_enable = "",
		.wan_vpndhcp = "",
		.quagga_enable = "0",
		.mr_altnet_x = "",
		.ttl_inc_enable = "0"
	},
	{
		.profile_name = "Unifi-Home",
		.iptv_port = "LAN4",
		.voip_port = "",
		.bridge_port = "",
		.iptv_config = "0",
		.voip_config = "0",
		.switch_wantag = "unifi_home",
		.switch_stb_x = "4",
		.switch_wan0tagid = "", .switch_wan0prio = "",
		.switch_wan1tagid = "", .switch_wan1prio = "",
		.switch_wan2tagid = "", .switch_wan2prio = "",
		.mr_enable_x = "",
		.emf_enable = "",
		.wan_vpndhcp = "",
		.quagga_enable = "0",
		.mr_altnet_x = "",
		.ttl_inc_enable = "0"
	},
	{
		.profile_name = "Singtel-MIO",
		.iptv_port = "LAN4",
		.voip_port = "LAN3",
		.bridge_port = "",
		.iptv_config = "0",
		.voip_config = "0",
		.switch_wantag = "singtel_mio",
		.switch_stb_x = "6",
		.switch_wan0tagid = "", .switch_wan0prio = "",
		.switch_wan1tagid = "", .switch_wan1prio = "",
		.switch_wan2tagid = "", .switch_wan2prio = "",
		.mr_enable_x = "",
		.emf_enable = "",
		.wan_vpndhcp = "",
		.quagga_enable = "0",
		.mr_altnet_x = "",
		.ttl_inc_enable = "0"
	},
	{
		.profile_name = "Singtel-Others",
		.iptv_port = "LAN4",
		.voip_port = "",
		.bridge_port = "",
		.iptv_config = "0",
		.voip_config = "0",
		.switch_wantag = "singtel_others",
		.switch_stb_x = "4",
		.switch_wan0tagid = "", .switch_wan0prio = "",
		.switch_wan1tagid = "", .switch_wan1prio = "",
		.switch_wan2tagid = "", .switch_wan2prio = "",
		.mr_enable_x = "",
		.emf_enable = "",
		.wan_vpndhcp = "",
		.quagga_enable = "0",
		.mr_altnet_x = "",
		.ttl_inc_enable = "0"
	},
	{
		.profile_name = "M1-Fiber",
		.iptv_port = "",
		.voip_port = "LAN3",
		.bridge_port = "",
		.iptv_config = "0",
		.voip_config = "0",
		.switch_wantag = "m1_fiber",
		.switch_stb_x = "3",
		.switch_wan0tagid = "", .switch_wan0prio = "",
		.switch_wan1tagid = "", .switch_wan1prio = "",
		.switch_wan2tagid = "", .switch_wan2prio = "",
		.mr_enable_x = "",
		.emf_enable = "",
		.wan_vpndhcp = "",
		.quagga_enable = "0",
		.mr_altnet_x = "",
		.ttl_inc_enable = "0"
	},
	{
		.profile_name = "Maxis-Fiber",
		.iptv_port = "",
		.voip_port = "LAN3",
		.bridge_port = "",
		.iptv_config = "0",
		.voip_config = "0",
		.switch_wantag = "maxis_fiber",
		.switch_stb_x = "3",
		.switch_wan0tagid = "", .switch_wan0prio = "",
		.switch_wan1tagid = "", .switch_wan1prio = "",
		.switch_wan2tagid = "", .switch_wan2prio = "",
		.mr_enable_x = "",
		.emf_enable = "",
		.wan_vpndhcp = "",
		.quagga_enable = "0",
		.mr_altnet_x = "",
		.ttl_inc_enable = "0"
	},
	{
		.profile_name = "Maxis-Fiber-Special",
		.iptv_port = "",
		.voip_port = "LAN3",
		.bridge_port = "",
		.iptv_config = "0",
		.voip_config = "0",
		.switch_wantag = "maxis_fiber_sp",
		.switch_stb_x = "3",
		.switch_wan0tagid = "", .switch_wan0prio = "",
		.switch_wan1tagid = "", .switch_wan1prio = "",
		.switch_wan2tagid = "", .switch_wan2prio = "",
		.mr_enable_x = "",
		.emf_enable = "",
		.wan_vpndhcp = "",
		.quagga_enable = "0",
		.mr_altnet_x = "",
		.ttl_inc_enable = "0"
	},
	{
		.profile_name = "Movistar Triple VLAN",
		.iptv_port = "",
		.voip_port = "",
		.bridge_port = "",
		.iptv_config = "1",
		.voip_config = "1",
		.switch_wantag = "movistar",
		.switch_stb_x = "8",
		.switch_wan0tagid = "", .switch_wan0prio = "",
		.switch_wan1tagid = "", .switch_wan1prio = "",
		.switch_wan2tagid = "", .switch_wan2prio = "",
		.mr_enable_x = "1",
		.emf_enable = "1",
		.wan_vpndhcp = "0",
		.quagga_enable = "0",
		.mr_altnet_x = "172.0.0.0/8",
		.ttl_inc_enable = "0"
	},
	{
		.profile_name = "Meo",
		.iptv_port = "",
		.voip_port = "",
		.bridge_port = "LAN4",
		.iptv_config = "0",
		.voip_config = "0",
		.switch_wantag = "meo",
		.switch_stb_x = "4",
		.switch_wan0tagid = "", .switch_wan0prio = "",
		.switch_wan1tagid = "", .switch_wan1prio = "",
		.switch_wan2tagid = "", .switch_wan2prio = "",
		.mr_enable_x = "1",
		.emf_enable = "1",
		.wan_vpndhcp = "0",
		.quagga_enable = "0",
		.mr_altnet_x = "",
		.ttl_inc_enable = "1"
	},
	{
		.profile_name = "Vodafone(Portugal)",
		.iptv_port = "LAN3",
		.voip_port = "",
		.bridge_port = "LAN4",
		.iptv_config = "0",
		.voip_config = "0",
		.switch_wantag = "vodafone",
		.switch_stb_x = "3",
		.switch_wan0tagid = "", .switch_wan0prio = "",
		.switch_wan1tagid = "", .switch_wan1prio = "",
		.switch_wan2tagid = "", .switch_wan2prio = "",
		.mr_enable_x = "1",
		.emf_enable = "1",
		.wan_vpndhcp = "0",
		.quagga_enable = "0",
		.mr_altnet_x = "",
		.ttl_inc_enable = "0"
	},
	{
		.profile_name = "Hinet MOD",
		.iptv_port = "LAN4",
		.voip_port = "",
		.bridge_port = "",
		.iptv_config = "0",
		.voip_config = "0",
		.switch_wantag = "hinet",
		.switch_stb_x = "4",
		.switch_wan0tagid = "", .switch_wan0prio = "",
		.switch_wan1tagid = "", .switch_wan1prio = "",
		.switch_wan2tagid = "", .switch_wan2prio = "",
		.mr_enable_x = "",
		.emf_enable = "",
		.wan_vpndhcp = "",
		.quagga_enable = "0",
		.mr_altnet_x = "",
		.ttl_inc_enable = "0"
	},
	{
		.profile_name = "Stuff-Fibre",
		.iptv_port = "",
		.voip_port = "",
		.bridge_port = "",
		.iptv_config = "0",
		.voip_config = "0",
		.switch_wantag = "stuff_fibre",
		.switch_stb_x = "0",
		.switch_wan0tagid = "", .switch_wan0prio = "",
		.switch_wan1tagid = "", .switch_wan1prio = "",
		.switch_wan2tagid = "", .switch_wan2prio = "",
		.mr_enable_x = "",
		.emf_enable = "",
		.wan_vpndhcp = "",
		.quagga_enable = "0",
		.mr_altnet_x = "",
		.ttl_inc_enable = "0"
	},
	{
		.profile_name = "manual",
		.iptv_port = "LAN4",
		.voip_port = "LAN3",
		.bridge_port = "",
		.iptv_config = "0",
		.voip_config = "0",
		.switch_wantag = "manual",
		.switch_stb_x = "0",
		.switch_wan0tagid = "", .switch_wan0prio = "",
		.switch_wan1tagid = "", .switch_wan1prio = "",
		.switch_wan2tagid = "", .switch_wan2prio = "",
		.mr_enable_x = "",
		.emf_enable = "",
		.wan_vpndhcp = "",
		.quagga_enable = "0",
		.mr_altnet_x = "",
		.ttl_inc_enable = "0"
	},
};
static unsigned int num_isp_profile = sizeof(isp_profiles)/sizeof(*isp_profiles);


int ej_get_iptvSettings(int eid, webs_t wp, int argc, char **argv)
{
	/*
	 * {
	 *   "stb_x_options": [ { "name": "none", "value": "0" }, { "name": "LAN1", "value": "1" }, { "name": "LAN2", "value": "2" }, { "name": "LAN3", "value": "3" }, { "name": "LAN4", "value": "4" }, { "name": "LAN1 & LAN2", "value": "5" }, { "name": "LAN3 & LAN4", "value": "6" } ],
	 *   "isp_profiles": [ { "profile_name": "none", "iptv_port": "", "voip_port": "", "bridge_port": "", "iptv_config": "0", "voip_config": "0", "switch_wantag": "none", "switch_stb_x": "0", "mr_enable_x": "", "emf_enable": "", "wan_vpndhcp": "", "quagga_enable": "0", "mr_altnet_x": "", "ttl_inc_enable": "0" }, { "profile_name": "Unifi-Business", "iptv_port": "", "voip_port": "", "bridge_port": "", "iptv_config": "0", "voip_config": "0", "switch_wantag": "unifi_biz", "switch_stb_x": "0", "mr_enable_x": "", "emf_enable": "", "wan_vpndhcp": "", "quagga_enable": "0", "mr_altnet_x": "", "ttl_inc_enable": "0" }, { "profile_name": "Unifi-Home", "iptv_port": "LAN4", "voip_port": "", "bridge_port": "", "iptv_config": "0", "voip_config": "0", "switch_wantag": "unifi_home", "switch_stb_x": "4", "mr_enable_x": "", "emf_enable": "", "wan_vpndhcp": "", "quagga_enable": "0", "mr_altnet_x": "", "ttl_inc_enable": "0" }, { "profile_name": "Singtel-MIO", "iptv_port": "LAN4", "voip_port": "LAN3", "bridge_port": "", "iptv_config": "0", "voip_config": "0", "switch_wantag": "singtel_mio", "switch_stb_x": "6", "mr_enable_x": "", "emf_enable": "", "wan_vpndhcp": "", "quagga_enable": "0", "mr_altnet_x": "", "ttl_inc_enable": "0" }, { "profile_name": "Singtel-Others", "iptv_port": "LAN4", "voip_port": "", "bridge_port": "", "iptv_config": "0", "voip_config": "0", "switch_wantag": "singtel_others", "switch_stb_x": "4", "mr_enable_x": "", "emf_enable": "", "wan_vpndhcp": "", "quagga_enable": "0", "mr_altnet_x": "", "ttl_inc_enable": "0" }, { "profile_name": "M1-Fiber", "iptv_port": "", "voip_port": "LAN3", "bridge_port": "", "iptv_config": "0", "voip_config": "0", "switch_wantag": "m1_fiber", "switch_stb_x": "3", "mr_enable_x": "", "emf_enable": "", "wan_vpndhcp": "", "quagga_enable": "0", "mr_altnet_x": "", "ttl_inc_enable": "0" }, { "profile_name": "Maxis-Fiber", "iptv_port": "", "voip_port": "LAN3", "bridge_port": "", "iptv_config": "0", "voip_config": "0", "switch_wantag": "maxis_fiber", "switch_stb_x": "3", "mr_enable_x": "", "emf_enable": "", "wan_vpndhcp": "", "quagga_enable": "0", "mr_altnet_x": "", "ttl_inc_enable": "0" }, { "profile_name": "Maxis-Fiber-Special", "iptv_port": "", "voip_port": "LAN3", "bridge_port": "", "iptv_config": "0", "voip_config": "0", "switch_wantag": "maxis_fiber_sp", "switch_stb_x": "3", "mr_enable_x": "", "emf_enable": "", "wan_vpndhcp": "", "quagga_enable": "0", "mr_altnet_x": "", "ttl_inc_enable": "0" }, { "profile_name": "Movistar Triple VLAN", "iptv_port": "", "voip_port": "", "bridge_port": "", "iptv_config": "1", "voip_config": "1", "switch_wantag": "movistar", "switch_stb_x": "8", "mr_enable_x": "1", "emf_enable": "1", "wan_vpndhcp": "0", "quagga_enable": "0", "mr_altnet_x": "172.0.0.0\/8", "ttl_inc_enable": "0" }, { "profile_name": "Meo", "iptv_port": "", "voip_port": "", "bridge_port": "LAN4", "iptv_config": "0", "voip_config": "0", "switch_wantag": "meo", "switch_stb_x": "4", "mr_enable_x": "1", "emf_enable": "1", "wan_vpndhcp": "0", "quagga_enable": "0", "mr_altnet_x": "", "ttl_inc_enable": "1" }, { "profile_name": "Vodafone(Portugal)", "iptv_port": "LAN3", "voip_port": "", "bridge_port": "LAN4", "iptv_config": "0", "voip_config": "0", "switch_wantag": "vodafone", "switch_stb_x": "3", "mr_enable_x": "1", "emf_enable": "1", "wan_vpndhcp": "0", "quagga_enable": "0", "mr_altnet_x": "", "ttl_inc_enable": "0" }, { "profile_name": "Hinet MOD", "iptv_port": "LAN4", "voip_port": "", "bridge_port": "", "iptv_config": "0", "voip_config": "0", "switch_wantag": "hinet", "switch_stb_x": "4", "mr_enable_x": "", "emf_enable": "", "wan_vpndhcp": "", "quagga_enable": "0", "mr_altnet_x": "", "ttl_inc_enable": "0" }, { "profile_name": "Stuff-Fibre", "iptv_port": "", "voip_port": "", "bridge_port": "", "iptv_config": "0", "voip_config": "0", "switch_wantag": "stuff_fibre", "switch_stb_x": "0", "mr_enable_x": "", "emf_enable": "", "wan_vpndhcp": "", "quagga_enable": "0", "mr_altnet_x": "", "ttl_inc_enable": "0" }, { "profile_name": "manual", "iptv_port": "LAN4", "voip_port": "LAN3", "bridge_port": "", "iptv_config": "0", "voip_config": "0", "switch_wantag": "manual", "switch_stb_x": "0", "mr_enable_x": "", "emf_enable": "", "wan_vpndhcp": "", "quagga_enable": "0", "mr_altnet_x": "", "ttl_inc_enable": "0" } ]
	 * }
	 */
	int i;
	struct iptv_profile *isp_profile = NULL;
	struct json_object *item = NULL;
	struct json_object *array = NULL;
	struct json_object *root = json_object_new_object();

	/* stb_x options */
	for(array = json_object_new_array(), i = 0; i < num_stb_x_option; i++) {
		item = json_object_new_object();
		json_object_object_add(item, "name", json_object_new_string(stb_x_options[i].name));
		json_object_object_add(item, "value", json_object_new_string(stb_x_options[i].value));
		json_object_array_add(array, item);
	}
	json_object_object_add(root, "stb_x_options", array);

	/* isp profiles */
	for(array = json_object_new_array(), i = 0; i < num_isp_profile; i++) {
		isp_profile = &isp_profiles[i];
		item = json_object_new_object();
		json_object_object_add(item, "profile_name", json_object_new_string(isp_profile->profile_name));
		json_object_object_add(item, "iptv_port", json_object_new_string(isp_profile->iptv_port));
		json_object_object_add(item, "voip_port", json_object_new_string(isp_profile->voip_port));
		json_object_object_add(item, "bridge_port", json_object_new_string(isp_profile->bridge_port));
		json_object_object_add(item, "iptv_config", json_object_new_string(isp_profile->iptv_config));
		json_object_object_add(item, "voip_config", json_object_new_string(isp_profile->voip_config));
		json_object_object_add(item, "switch_wantag", json_object_new_string(isp_profile->switch_wantag));
		json_object_object_add(item, "switch_stb_x", json_object_new_string(isp_profile->switch_stb_x));
		json_object_object_add(item, "mr_enable_x", json_object_new_string(isp_profile->mr_enable_x));
		json_object_object_add(item, "emf_enable", json_object_new_string(isp_profile->emf_enable));
		json_object_object_add(item, "wan_vpndhcp", json_object_new_string(isp_profile->wan_vpndhcp));
		json_object_object_add(item, "quagga_enable", json_object_new_string(isp_profile->quagga_enable));
		json_object_object_add(item, "mr_altnet_x", json_object_new_string(isp_profile->mr_altnet_x));
		json_object_object_add(item, "ttl_inc_enable", json_object_new_string(isp_profile->ttl_inc_enable));
		json_object_array_add(array, item);
	}
	json_object_object_add(root, "isp_profiles", array);

	websWrite(wp, "%s", json_object_to_json_string(root));
	json_object_put(root);
	return 0;
}

int config_iptv_vlan(char *isp)
{
	return 0;
}
