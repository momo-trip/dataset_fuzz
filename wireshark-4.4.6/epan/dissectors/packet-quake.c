/* packet-quake.c
 * Routines for Quake packet dissection
 *
 * Uwe Girlich <uwe@planetquake.com>
 *	http://www.idsoftware.com/q1source/q1source.zip
 *
 * Wireshark - Network traffic analyzer
 * By Gerald Combs <gerald@wireshark.org>
 * Copyright 1998 Gerald Combs
 *
 * Copied from packet-tftp.c
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "config.h"

#include <epan/packet.h>
#include <epan/conversation.h>

void proto_register_quake(void);

static int proto_quake;
static int hf_quake_header_flags;
static int hf_quake_header_flags_data;
static int hf_quake_header_flags_ack;
static int hf_quake_header_flags_no_ack;
static int hf_quake_header_flags_endmsg;
static int hf_quake_header_flags_unreliable;
static int hf_quake_header_flags_control;
static int hf_quake_header_length;
static int hf_quake_header_sequence;
static int hf_quake_control_command;

static int hf_quake_CCREQ_CONNECT_game;
static int hf_quake_CCREQ_CONNECT_version;
static int hf_quake_CCREQ_SERVER_INFO_game;
static int hf_quake_CCREQ_SERVER_INFO_version;
static int hf_quake_CCREQ_PLAYER_INFO_player;
static int hf_quake_CCREQ_RULE_INFO_lastrule;

static int hf_quake_CCREP_ACCEPT_port;
static int hf_quake_CCREP_REJECT_reason;
static int hf_quake_CCREP_SERVER_INFO_address;
static int hf_quake_CCREP_SERVER_INFO_server;
static int hf_quake_CCREP_SERVER_INFO_map;
static int hf_quake_CCREP_SERVER_INFO_num_player;
static int hf_quake_CCREP_SERVER_INFO_max_player;
static int hf_quake_CCREP_PLAYER_INFO_name;
static int hf_quake_CCREP_PLAYER_INFO_colors;
static int hf_quake_CCREP_PLAYER_INFO_colors_shirt;
static int hf_quake_CCREP_PLAYER_INFO_colors_pants;
static int hf_quake_CCREP_PLAYER_INFO_frags;
static int hf_quake_CCREP_PLAYER_INFO_connect_time;
static int hf_quake_CCREP_PLAYER_INFO_address;
static int hf_quake_CCREP_RULE_INFO_rule;
static int hf_quake_CCREP_RULE_INFO_value;


static int ett_quake;
static int ett_quake_control;
static int ett_quake_control_colors;
static int ett_quake_flags;

static dissector_handle_t quake_handle;

/* I took these names directly out of the Q1 source. */
#define NET_HEADERSIZE 8
#define DEFAULTnet_hostport 26000

#define NETFLAG_DATA            0x0001
#define NETFLAG_ACK             0x0002
#define NETFLAG_NAK             0x0004
#define NETFLAG_EOM             0x0008
#define NETFLAG_UNRELIABLE      0x0010
#define NETFLAG_CTL             0x8000


#define CCREQ_CONNECT           0x01
#define CCREQ_SERVER_INFO       0x02
#define CCREQ_PLAYER_INFO       0x03
#define CCREQ_RULE_INFO         0x04

#define CCREP_ACCEPT            0x81
#define CCREP_REJECT            0x82
#define CCREP_SERVER_INFO       0x83
#define CCREP_PLAYER_INFO       0x84
#define CCREP_RULE_INFO         0x85

static const value_string names_control_command[] = {
	{	CCREQ_CONNECT, "connect" },
	{	CCREQ_SERVER_INFO, "server_info" },
	{	CCREQ_PLAYER_INFO, "player_info" },
	{	CCREQ_RULE_INFO, "rule_info" },
	{	CCREP_ACCEPT, "accept" },
	{	CCREP_REJECT, "reject" },
	{	CCREP_SERVER_INFO, "server_info" },
	{	CCREP_PLAYER_INFO, "player_info" },
	{	CCREP_RULE_INFO, "rule_info" },
	{ 0, NULL }
};

#define CCREQ 0x00
#define CCREP 0x80

#define QUAKE_MAXSTRING 0x800

static const value_string names_control_direction[] = {
	{ CCREQ, "Request" },
	{ CCREP, "Reply" },
	{ 0, NULL }
};


static const value_string names_colors[] = {
	{  0, "White" },
	{  1, "Brown" },
	{  2, "Lavender" },
	{  3, "Khaki" },
	{  4, "Red" },
	{  5, "Lt Brown" },
	{  6, "Peach" },
	{  7, "Lt Peach" },
	{  8, "Purple" },
	{  9, "Dk Purple" },
	{ 10, "Tan" },
	{ 11, "Green" },
	{ 12, "Yellow" },
	{ 13, "Blue" },
	{ 14, "Fire" },
	{ 15, "Brights" },
	{  0, NULL }
};

static void
dissect_quake_CCREQ_CONNECT
(tvbuff_t *tvb, proto_tree *tree)
{
	int offset = 0;
	int item_len;

	proto_tree_add_item_ret_length(tree, hf_quake_CCREQ_CONNECT_game,
			tvb, offset, -1, ENC_ASCII|ENC_NA, &item_len);
	offset += item_len;

	proto_tree_add_item(tree, hf_quake_CCREQ_CONNECT_version,
			tvb, offset, 1, ENC_LITTLE_ENDIAN);
}


static void
dissect_quake_CCREQ_SERVER_INFO
(tvbuff_t *tvb, proto_tree *tree)
{
	int offset = 0;
	int item_len;

	proto_tree_add_item_ret_length(tree, hf_quake_CCREQ_SERVER_INFO_game,
			tvb, offset, -1, ENC_ASCII|ENC_NA, &item_len);
	offset += item_len;
	proto_tree_add_item(tree, hf_quake_CCREQ_SERVER_INFO_version,
			tvb, offset, 1, ENC_LITTLE_ENDIAN);
}


static void
dissect_quake_CCREQ_PLAYER_INFO
(tvbuff_t *tvb, proto_tree *tree)
{
	proto_tree_add_item(tree, hf_quake_CCREQ_PLAYER_INFO_player,
			tvb, 0, 1, ENC_LITTLE_ENDIAN);
}


static void
dissect_quake_CCREQ_RULE_INFO
(tvbuff_t *tvb, proto_tree *tree)
{
	proto_tree_add_item(tree, hf_quake_CCREQ_RULE_INFO_lastrule,
			tvb, 0, -1, ENC_ASCII);
}


static void
dissect_quake_CCREP_ACCEPT
(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
	uint32_t port;
	conversation_t *c;

	port = tvb_get_letohl(tvb, 0);
	c = find_or_create_conversation(pinfo);
	conversation_set_dissector(c, quake_handle);

	proto_tree_add_uint(tree, hf_quake_CCREP_ACCEPT_port,
			tvb, 0, 4, port);
}


static void
dissect_quake_CCREP_REJECT
(tvbuff_t *tvb, proto_tree *tree)
{
	proto_tree_add_item(tree, hf_quake_CCREP_REJECT_reason,
			tvb, 0, -1, ENC_ASCII);
}


static void
dissect_quake_CCREP_SERVER_INFO
(tvbuff_t *tvb, proto_tree *tree)
{
	int offset = 0;
	int item_len;

	proto_tree_add_item_ret_length(tree,
			hf_quake_CCREP_SERVER_INFO_address, tvb, offset, -1,
			ENC_ASCII|ENC_NA, &item_len);
	offset += item_len;

	proto_tree_add_item_ret_length(tree,
			hf_quake_CCREP_SERVER_INFO_server, tvb, offset, -1,
			ENC_ASCII|ENC_NA, &item_len);
	offset += item_len;

	proto_tree_add_item_ret_length(tree,
			hf_quake_CCREP_SERVER_INFO_map, tvb, offset, -1,
			ENC_ASCII|ENC_NA, &item_len);
	offset += item_len;

	proto_tree_add_item(tree, hf_quake_CCREP_SERVER_INFO_num_player,
			tvb, offset, 1, ENC_LITTLE_ENDIAN);
	offset += 1;
	proto_tree_add_item(tree, hf_quake_CCREP_SERVER_INFO_max_player,
			tvb, offset, 1, ENC_LITTLE_ENDIAN);
	offset += 1;
	proto_tree_add_item(tree, hf_quake_CCREQ_SERVER_INFO_version,
			tvb, offset, 1, ENC_LITTLE_ENDIAN);
}


static void
dissect_quake_CCREP_PLAYER_INFO
(tvbuff_t *tvb, proto_tree *tree)
{
	int offset = 0;
	uint32_t colors;
	uint32_t color_shirt;
	uint32_t color_pants;
	proto_item *colors_item;
	proto_tree *colors_tree;
	int item_len;

	proto_tree_add_item(tree, hf_quake_CCREQ_PLAYER_INFO_player,
			tvb, offset, 1, ENC_LITTLE_ENDIAN);
	offset += 1;

	proto_tree_add_item_ret_length(tree, hf_quake_CCREP_PLAYER_INFO_name,
			tvb, offset, -1, ENC_ASCII|ENC_NA, &item_len);
	offset += item_len;

	colors       = tvb_get_letohl(tvb, offset + 0);
	color_shirt = (colors >> 4) & 0x0f;
	color_pants = (colors     ) & 0x0f;

	colors_item = proto_tree_add_uint(tree,
			hf_quake_CCREP_PLAYER_INFO_colors,
			tvb, offset, 4, colors);
	colors_tree = proto_item_add_subtree(colors_item,
			ett_quake_control_colors);
	proto_tree_add_uint(colors_tree,
			hf_quake_CCREP_PLAYER_INFO_colors_shirt,
			tvb, offset, 1, color_shirt);
	proto_tree_add_uint(colors_tree,
			hf_quake_CCREP_PLAYER_INFO_colors_pants,
			tvb, offset, 1, color_pants);
	offset += 4;
	proto_tree_add_item(tree, hf_quake_CCREP_PLAYER_INFO_frags,
			tvb, offset, 4, ENC_LITTLE_ENDIAN);
	offset += 4;
	proto_tree_add_item(tree, hf_quake_CCREP_PLAYER_INFO_connect_time,
			tvb, offset, 4, ENC_LITTLE_ENDIAN);
	offset += 4;

	proto_tree_add_item(tree, hf_quake_CCREP_PLAYER_INFO_address,
			tvb, offset, -1, ENC_ASCII);
}


static void
dissect_quake_CCREP_RULE_INFO
(tvbuff_t *tvb, proto_tree *tree)
{
	int offset = 0;
	int item_len;

	if (tvb_reported_length(tvb) == 0) return;

	proto_tree_add_item_ret_length(tree, hf_quake_CCREP_RULE_INFO_rule,
			tvb, offset, -1, ENC_ASCII|ENC_NA, &item_len);
	offset += item_len;

	proto_tree_add_item(tree, hf_quake_CCREP_RULE_INFO_value,
			tvb, offset, -1, ENC_ASCII);
}


static void
dissect_quake_control(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
	uint8_t		command;
	int		direction;
	proto_tree	*control_tree;
	tvbuff_t	*next_tvb;

	command = tvb_get_uint8(tvb, 0);
	direction = (command & 0x80) ? CCREP : CCREQ;

	col_add_fstr(pinfo->cinfo, COL_INFO, "%s %s",
			val_to_str(command,names_control_command, "%u"),
			val_to_str(direction,names_control_direction,"%u"));

	control_tree = proto_tree_add_subtree_format(tree, tvb,
			0, -1, ett_quake_control, NULL, "Control %s: %s",
			val_to_str(direction, names_control_direction, "%u"),
			val_to_str(command, names_control_command, "%u"));
	proto_tree_add_uint(control_tree, hf_quake_control_command,
			tvb, 0, 1, command);

	next_tvb = tvb_new_subset_remaining(tvb, 1);
	switch (command) {
		case CCREQ_CONNECT:
			dissect_quake_CCREQ_CONNECT
				(next_tvb, control_tree);
		break;
		case CCREQ_SERVER_INFO:
			dissect_quake_CCREQ_SERVER_INFO
				(next_tvb, control_tree);
		break;
		case CCREQ_PLAYER_INFO:
			dissect_quake_CCREQ_PLAYER_INFO
				(next_tvb, control_tree);
		break;
		case CCREQ_RULE_INFO:
			dissect_quake_CCREQ_RULE_INFO
				(next_tvb, control_tree);
		break;
		case CCREP_ACCEPT:
			dissect_quake_CCREP_ACCEPT
				(next_tvb, pinfo, control_tree);
		break;
		case CCREP_REJECT:
			dissect_quake_CCREP_REJECT
				(next_tvb, control_tree);
		break;
		case CCREP_SERVER_INFO:
			dissect_quake_CCREP_SERVER_INFO
				(next_tvb, control_tree);
		break;
		case CCREP_PLAYER_INFO:
			dissect_quake_CCREP_PLAYER_INFO
				(next_tvb, control_tree);
		break;
		case CCREP_RULE_INFO:
			dissect_quake_CCREP_RULE_INFO
				(next_tvb, control_tree);
		break;
		default:
			call_data_dissector(next_tvb, pinfo, control_tree);
		break;
	}
}


static int
dissect_quake(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void* data _U_)
{
	proto_tree	*quake_tree;
	proto_item	*quake_item;
	uint16_t		flags;
	proto_item	*flags_item;
	proto_tree	*flags_tree;
	uint32_t		sequence = 0;
	tvbuff_t	*next_tvb;

	col_set_str(pinfo->cinfo, COL_PROTOCOL, "QUAKE");
	col_clear(pinfo->cinfo, COL_INFO);

	flags = tvb_get_ntohs(tvb, 0);

	quake_item = proto_tree_add_item(tree, proto_quake, tvb, 0, -1, ENC_NA);
	quake_tree = proto_item_add_subtree(quake_item, ett_quake);

	flags_item = proto_tree_add_item(quake_tree, hf_quake_header_flags,
			tvb, 0, 2, ENC_BIG_ENDIAN);
	flags_tree = proto_item_add_subtree(flags_item, ett_quake_flags);
	proto_tree_add_item(flags_tree, hf_quake_header_flags_data,
			tvb, 0, 2, ENC_BIG_ENDIAN);
	proto_tree_add_item(flags_tree, hf_quake_header_flags_ack,
			tvb, 0, 2, ENC_BIG_ENDIAN);
	proto_tree_add_item(flags_tree, hf_quake_header_flags_no_ack,
			tvb, 0, 2, ENC_BIG_ENDIAN);
	proto_tree_add_item(flags_tree, hf_quake_header_flags_endmsg,
			tvb, 0, 2, ENC_BIG_ENDIAN);
	proto_tree_add_item(flags_tree, hf_quake_header_flags_unreliable,
			tvb, 0, 2, ENC_BIG_ENDIAN);
	proto_tree_add_item(flags_tree, hf_quake_header_flags_control,
			tvb, 0, 2, ENC_BIG_ENDIAN);

	proto_tree_add_item(quake_tree, hf_quake_header_length, tvb, 2, 2, ENC_BIG_ENDIAN);

	if (flags == NETFLAG_CTL) {
		next_tvb = tvb_new_subset_remaining(tvb, 4);
		dissect_quake_control(next_tvb, pinfo, quake_tree);
		return tvb_captured_length(tvb);
	}

	sequence = tvb_get_ntohl(tvb, 4);
	col_add_fstr(pinfo->cinfo, COL_INFO, "seq 0x%x", sequence);
	proto_tree_add_uint(quake_tree, hf_quake_header_sequence,
			tvb, 4, 4, sequence);

	next_tvb = tvb_new_subset_remaining(tvb, 8);
	call_data_dissector(next_tvb, pinfo, quake_tree);
	return tvb_captured_length(tvb);
}


void proto_reg_handoff_quake(void);

void
proto_register_quake(void)
{
	static hf_register_info hf[] = {
		{ &hf_quake_header_flags,
		  { "Flags", "quake.header.flags",
		    FT_UINT16, BASE_HEX, NULL, 0x0,
		    NULL, HFILL }},
		{ &hf_quake_header_flags_data,
		  { "Data", "quake.header.flags.data",
		    FT_BOOLEAN, 16, TFS(&tfs_set_notset), NETFLAG_DATA,
		    NULL, HFILL }},
		{ &hf_quake_header_flags_ack,
		  { "Acknowledgment", "quake.header.flags.ack",
		    FT_BOOLEAN, 16, TFS(&tfs_set_notset), NETFLAG_ACK,
		    NULL, HFILL }},
		{ &hf_quake_header_flags_no_ack,
		  { "No Acknowledgment", "quake.header.flags.no_ack",
		    FT_BOOLEAN, 16, TFS(&tfs_set_notset), NETFLAG_NAK,
		    NULL, HFILL }},
		{ &hf_quake_header_flags_endmsg,
		  { "End Of Message", "quake.header.flags.endmsg",
		    FT_BOOLEAN, 16, TFS(&tfs_set_notset), NETFLAG_EOM,
		    NULL, HFILL }},
		{ &hf_quake_header_flags_unreliable,
		  { "Unreliable", "quake.header.flags.unreliable",
		    FT_BOOLEAN, 16, TFS(&tfs_set_notset), NETFLAG_UNRELIABLE,
		    NULL, HFILL }},
		{ &hf_quake_header_flags_control,
		  { "Control", "quake.header.flags.control",
		    FT_BOOLEAN, 16, TFS(&tfs_set_notset), NETFLAG_CTL,
		    NULL, HFILL }},
		{ &hf_quake_header_length,
		  { "Length", "quake.header.length",
		    FT_UINT16, BASE_DEC, NULL, 0x0,
		    "full data length", HFILL }},
		{ &hf_quake_header_sequence,
		  { "Sequence", "quake.header.sequence",
		    FT_UINT32, BASE_HEX, NULL, 0x0,
		    "Sequence Number", HFILL }},
		{ &hf_quake_control_command,
		  { "Command", "quake.control.command",
		    FT_UINT8, BASE_HEX, VALS(names_control_command), 0x0,
		    "Control Command", HFILL }},
		{ &hf_quake_CCREQ_CONNECT_game,
		  { "Game", "quake.control.connect.game",
		    FT_STRINGZ, BASE_NONE, NULL, 0x0,
		    "Game Name", HFILL }},
		{ &hf_quake_CCREQ_CONNECT_version,
		  { "Version", "quake.control.connect.version",
		    FT_UINT8, BASE_DEC, NULL, 0x0,
		    "Game Protocol Version Number", HFILL }},
		{ &hf_quake_CCREQ_SERVER_INFO_game,
		  { "Game", "quake.control.server_info.game",
		    FT_STRINGZ, BASE_NONE, NULL, 0x0,
		    "Game Name", HFILL }},
		{ &hf_quake_CCREQ_SERVER_INFO_version,
		  { "Version", "quake.control.server_info.version",
		    FT_UINT8, BASE_DEC, NULL, 0x0,
		    "Game Protocol Version Number", HFILL }},
		{ &hf_quake_CCREQ_PLAYER_INFO_player,
		  { "Player", "quake.control.player_info.player",
		    FT_UINT8, BASE_DEC, NULL, 0x0,
		    NULL, HFILL }},
		{ &hf_quake_CCREQ_RULE_INFO_lastrule,
		  { "Last Rule", "quake.control.rule_info.lastrule",
		    FT_STRINGZ, BASE_NONE, NULL, 0x0,
		    "Last Rule Name", HFILL }},
		{ &hf_quake_CCREP_ACCEPT_port,
		  { "Port", "quake.control.accept.port",
		    FT_UINT32, BASE_DEC, NULL, 0x0,
		    "Game Data Port", HFILL }},
		{ &hf_quake_CCREP_REJECT_reason,
		  { "Reason", "quake.control.reject.reason",
		    FT_STRINGZ, BASE_NONE, NULL, 0x0,
		    "Reject Reason", HFILL }},
		{ &hf_quake_CCREP_SERVER_INFO_address,
		  { "Address", "quake.control.server_info.address",
		    FT_STRINGZ, BASE_NONE, NULL, 0x0,
		    "Server Address", HFILL }},
		{ &hf_quake_CCREP_SERVER_INFO_server,
		  { "Server", "quake.control.server_info.server",
		    FT_STRINGZ, BASE_NONE, NULL, 0x0,
		    "Server Name", HFILL }},
		{ &hf_quake_CCREP_SERVER_INFO_map,
		  { "Map", "quake.control.server_info.map",
		    FT_STRINGZ, BASE_NONE, NULL, 0x0,
		    "Map Name", HFILL }},
		{ &hf_quake_CCREP_SERVER_INFO_num_player,
		  { "Number of Players", "quake.control.server_info.num_player",
		    FT_UINT8, BASE_DEC, NULL, 0x0,
		    "Current Number of Players", HFILL }},
		{ &hf_quake_CCREP_SERVER_INFO_max_player,
		  { "Maximal Number of Players", "quake.control.server_info.max_player",
		    FT_UINT8, BASE_DEC, NULL, 0x0,
		    NULL, HFILL }},
		{ &hf_quake_CCREP_PLAYER_INFO_name,
		  { "Name", "quake.control.player_info.name",
		    FT_STRINGZ, BASE_NONE, NULL, 0x0,
		    "Player Name", HFILL }},
		{ &hf_quake_CCREP_PLAYER_INFO_colors,
		  { "Colors", "quake.control.player_info.colors",
		    FT_UINT32, BASE_HEX, NULL, 0x0,
		    "Player Colors", HFILL }},
		{ &hf_quake_CCREP_PLAYER_INFO_colors_shirt,
		  { "Shirt", "quake.control.player_info.colors.shirt",
		    FT_UINT8, BASE_DEC, VALS(names_colors), 0x0,
		    "Shirt Color", HFILL }},
		{ &hf_quake_CCREP_PLAYER_INFO_colors_pants,
		  { "Pants", "quake.control.player_info.colors.pants",
		    FT_UINT8, BASE_DEC, VALS(names_colors), 0x0,
		    "Pants Color", HFILL }},
		{ &hf_quake_CCREP_PLAYER_INFO_frags,
		  { "Frags", "quake.control.player_info.frags",
		    FT_UINT32, BASE_DEC, NULL, 0x0,
		    "Player Frags", HFILL }},
		{ &hf_quake_CCREP_PLAYER_INFO_connect_time,
		  { "Connect Time", "quake.control.player_info.connect_time",
		    FT_UINT32, BASE_DEC, NULL, 0x0,
		    "Player Connect Time", HFILL }},
		{ &hf_quake_CCREP_PLAYER_INFO_address,
		  { "Address", "quake.control.player_info.address",
		    FT_STRINGZ, BASE_NONE, NULL, 0x0,
		    "Player Address", HFILL }},
		{ &hf_quake_CCREP_RULE_INFO_rule,
		  { "Rule", "quake.control.rule_info.rule",
		    FT_STRINGZ, BASE_NONE, NULL, 0x0,
		    "Rule Name", HFILL }},
		{ &hf_quake_CCREP_RULE_INFO_value,
		  { "Value", "quake.control.rule_info.value",
		    FT_STRINGZ, BASE_NONE, NULL, 0x0,
		    "Rule Value", HFILL }},
	};
	static int *ett[] = {
		&ett_quake,
		&ett_quake_control,
		&ett_quake_control_colors,
		&ett_quake_flags,
	};

	proto_quake = proto_register_protocol("Quake Network Protocol", "QUAKE", "quake");
	proto_register_field_array(proto_quake, hf, array_length(hf));
	proto_register_subtree_array(ett, array_length(ett));

	quake_handle = register_dissector("quake", dissect_quake, proto_quake);
}


void
proto_reg_handoff_quake(void)
{
	dissector_add_uint_with_preference("udp.port", DEFAULTnet_hostport, quake_handle);
}

/*
 * Editor modelines  -  https://www.wireshark.org/tools/modelines.html
 *
 * Local variables:
 * c-basic-offset: 8
 * tab-width: 8
 * indent-tabs-mode: t
 * End:
 *
 * vi: set shiftwidth=8 tabstop=8 noexpandtab:
 * :indentSize=8:tabSize=8:noTabs=false:
 */
