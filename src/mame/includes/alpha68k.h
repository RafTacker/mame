// license:BSD-3-Clause
// copyright-holders:Pierpaolo Prazzoli, Bryan McPhail
/*************************************************************************

    SNK/Alpha 68000 based games

*************************************************************************/

#include "machine/74259.h"
#include "machine/gen_latch.h"

class alpha68k_state : public driver_device
{
public:
	alpha68k_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		m_shared_ram(*this, "shared_ram"),
		m_spriteram(*this, "spriteram"),
		m_videoram(*this, "videoram"),
		m_audiocpu(*this, "audiocpu"),
		m_maincpu(*this, "maincpu"),
		m_gfxdecode(*this, "gfxdecode"),
		m_palette(*this, "palette"),
		m_outlatch(*this, "outlatch"),
		m_soundlatch(*this, "soundlatch") { }

	void tnextspc(machine_config &config);
	void alpha68k_II(machine_config &config);
	void btlfieldb(machine_config &config);
	void alpha68k_I(machine_config &config);
	void kyros(machine_config &config);
	void alpha68k_V_sb(machine_config &config);
	void sstingry(machine_config &config);
	void jongbou(machine_config &config);
	void alpha68k_V(machine_config &config);
	void alpha68k_II_gm(machine_config &config);

	void init_paddlema();
	void init_btlfield();
	void init_jongbou();
	void init_goldmedl();
	void init_skyadvnt();
	void init_goldmedla();
	void init_gangwarsu();
	void init_gangwars();
	void init_tnextspc();
	void init_timesold1();
	void init_sbasebal();
	void init_sbasebalj();
	void init_skysoldr();
	void init_skyadvntu();
	void init_btlfieldb();
	void init_timesold();
	void init_kyros();
	void init_sstingry();

private:
	DECLARE_WRITE16_MEMBER(tnextspc_coin_counters_w);
	DECLARE_WRITE16_MEMBER(tnextspc_unknown_w);
	DECLARE_WRITE16_MEMBER(alpha_microcontroller_w);
	DECLARE_READ16_MEMBER(kyros_dip_r);
	DECLARE_READ16_MEMBER(control_1_r);
	DECLARE_READ16_MEMBER(control_2_r);
	DECLARE_READ16_MEMBER(control_2_V_r);
	DECLARE_READ16_MEMBER(control_3_r);
	DECLARE_READ16_MEMBER(control_4_r);
	DECLARE_READ16_MEMBER(jongbou_inputs_r);
	DECLARE_WRITE8_MEMBER(outlatch_w);
	DECLARE_WRITE8_MEMBER(paddlema_soundlatch_w);
	DECLARE_WRITE8_MEMBER(tnextspc_soundlatch_w);
	DECLARE_READ16_MEMBER(kyros_alpha_trigger_r);
	DECLARE_READ16_MEMBER(alpha_II_trigger_r);
	DECLARE_READ16_MEMBER(alpha_V_trigger_r);
	DECLARE_READ16_MEMBER(sound_cpu_r);
	DECLARE_WRITE8_MEMBER(sound_bank_w);
	DECLARE_WRITE8_MEMBER(porta_w);
	DECLARE_WRITE16_MEMBER(alpha68k_videoram_w);
	DECLARE_WRITE_LINE_MEMBER(video_control2_w);
	DECLARE_WRITE_LINE_MEMBER(video_control3_w);

	TILE_GET_INFO_MEMBER(get_tile_info);
	DECLARE_MACHINE_START(common);
	DECLARE_MACHINE_RESET(common);
	DECLARE_PALETTE_INIT(kyros);
	DECLARE_PALETTE_INIT(paddlem);
	DECLARE_MACHINE_START(alpha68k_II);
	DECLARE_MACHINE_RESET(alpha68k_II);
	DECLARE_VIDEO_START(alpha68k);
	DECLARE_MACHINE_START(alpha68k_V);
	DECLARE_MACHINE_RESET(alpha68k_V);
	uint32_t screen_update_sstingry(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	uint32_t screen_update_kyros(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	uint32_t screen_update_alpha68k_I(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	uint32_t screen_update_alpha68k_II(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	uint32_t screen_update_alpha68k_V(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	uint32_t screen_update_alpha68k_V_sb(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	INTERRUPT_GEN_MEMBER(alpha68k_sound_nmi);
	void alpha68k_flipscreen_w( int flip );
	DECLARE_WRITE8_MEMBER(video_bank_w);
	void draw_sprites( bitmap_ind16 &bitmap, const rectangle &cliprect, int j, int s, int e );
	void draw_sprites_V( bitmap_ind16 &bitmap, const rectangle &cliprect, int j, int s, int e, int fx_mask, int fy_mask, int sprite_mask );
	void draw_sprites_I( bitmap_ind16 &bitmap, const rectangle &cliprect, int c, int d, int yshift );
	void kyros_video_banking(int *bank, int data);
	void jongbou_video_banking(int *bank, int data);
	void kyros_draw_sprites( bitmap_ind16 &bitmap, const rectangle &cliprect, int c, int d );
	void sstingry_draw_sprites( bitmap_ind16 &bitmap, const rectangle &cliprect, int c, int d );
	void alpha68k_II_map(address_map &map);
	void alpha68k_I_map(address_map &map);
	void alpha68k_I_s_map(address_map &map);
	void alpha68k_V_map(address_map &map);
	void jongbou_sound_map(address_map &map);
	void jongbou_sound_portmap(address_map &map);
	void kyros_map(address_map &map);
	void kyros_sound_map(address_map &map);
	void kyros_sound_portmap(address_map &map);
	void sound_map(address_map &map);
	void sound_portmap(address_map &map);
	void sstingry_sound_map(address_map &map);
	void tnextspc_map(address_map &map);
	void tnextspc_sound_map(address_map &map);
	void tnextspc_sound_portmap(address_map &map);

	/* memory pointers */
	optional_shared_ptr<uint16_t> m_shared_ram;
	required_shared_ptr<uint16_t> m_spriteram;
	optional_shared_ptr<uint16_t> m_videoram;

	/* devices */
	required_device<cpu_device> m_audiocpu;
	required_device<cpu_device> m_maincpu;
	required_device<gfxdecode_device> m_gfxdecode;
	required_device<palette_device> m_palette;
	optional_device<ls259_device> m_outlatch;
	required_device<generic_latch_8_device> m_soundlatch;

	uint8_t       m_sound_nmi_mask;
	uint8_t       m_sound_pa_latch;

	/* video-related */
	tilemap_t     *m_fix_tilemap;
	int         m_bank_base;
	int         m_flipscreen;
	int         m_last_bank;

	/* misc */
	int         m_invert_controls;
	int         m_microcontroller_id;
	int         m_coin_id;
	unsigned    m_trigstate;
	unsigned    m_deposits1;
	unsigned    m_deposits2;
	unsigned    m_credits;
	unsigned    m_coinvalue;
	unsigned    m_microcontroller_data;
	int         m_latch;
	unsigned    m_game_id;  // see below
};

/* game_id - used to deal with a few game specific situations */
enum
{
	ALPHA68K_BTLFIELDB = 1,     // used in alpha_II_trigger_r
	ALPHA68K_JONGBOU,           // used in kyros_alpha_trigger_r & kyros_draw_sprites
	ALPHA68K_KYROS          // used in kyros_draw_sprites
};
