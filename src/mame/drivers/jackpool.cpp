// license:BSD-3-Clause
// copyright-holders:David Haywood, Angelo Salese
/*******************************************************************************************

Jackpot Cards / Jackpot Pool (c) 1997 Electronic Projects

driver by David Haywood & Angelo Salese

Notes:
-There's a "(c) 1992 HI-TECH Software..Brisbane, QLD Australia" string in the program roms,
 this is actually the m68k C compiler used for doing this game.

TODO:
-Correct NVRAM emulation (and default eeprom too?), you cannot save settings to the EEPROM
 right now, also remove the patch (it doesn't boot otherwise);
-UART;

*******************************************************************************************/

#include "emu.h"
#include "cpu/m68000/m68000.h"
#include "machine/74259.h"
#include "machine/eepromser.h"
#include "machine/ins8250.h"
#include "sound/okim6295.h"
#include "screen.h"
#include "speaker.h"


class jackpool_state : public driver_device
{
public:
	jackpool_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		m_vram(*this, "vram"),
		m_maincpu(*this, "maincpu"),
		m_eeprom(*this, "eeprom"),
		m_gfxdecode(*this, "gfxdecode"),
		m_palette(*this, "palette")  { }

	DECLARE_READ8_MEMBER(jackpool_io_r);
	DECLARE_WRITE_LINE_MEMBER(map_vreg_w);
	void init_jackpool();
	virtual void video_start() override;
	uint32_t screen_update_jackpool(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	INTERRUPT_GEN_MEMBER(jackpool_interrupt);

	required_shared_ptr<uint16_t> m_vram;
	uint8_t m_map_vreg;
	required_device<cpu_device> m_maincpu;
	required_device<eeprom_serial_93cxx_device> m_eeprom;
	required_device<gfxdecode_device> m_gfxdecode;
	required_device<palette_device> m_palette;
	void jackpool(machine_config &config);
	void jackpool_mem(address_map &map);
};


void jackpool_state::video_start()
{
}

uint32_t jackpool_state::screen_update_jackpool(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	gfx_element *gfx = m_gfxdecode->gfx(0);
	int count;// = 0x00000/2;

	int y,x;

	{
		count = m_map_vreg*(0x4000/2);
		for (y=0;y<32;y++)
		{
			for (x=0;x<64;x++)
			{
				int tile = (m_vram[count+(0x2000/2)] & 0x7fff);
				int attr = (m_vram[count+(0x2000/2)+0x800] & 0x1f00)>>8;

				gfx->opaque(bitmap,cliprect,tile,attr,0,0,x*8,y*8);
				count++;
			}
		}

		count = m_map_vreg*(0x4000/2);
		for (y=0;y<32;y++)
		{
			for (x=0;x<64;x++)
			{
				int tile = (m_vram[count] & 0x7fff);

				if(tile != 0)
				{
					int attr = (m_vram[count+0x800] & 0x1f00)>>8;
					int t_pen = (m_vram[count+0x800] & 0x1000);

					gfx->transpen(bitmap,cliprect,tile,attr,0,0,x*8,y*8,(t_pen) ? 0 : -1);
				}

				count++;
			}
		}
	}

	return 0;
}

READ8_MEMBER(jackpool_state::jackpool_io_r)
{
	switch(offset*2)
	{
		case 0x00: return ioport("COIN1")->read();
		case 0x04: return ioport("UNK1")->read();
		case 0x06: return ioport("UNK2")->read();
		case 0x08: return ioport("SERVICE1")->read();
		case 0x0a: return ioport("SERVICE2")->read();//probably not a button, remote?
		case 0x0c: return ioport("PAYOUT")->read();
		case 0x0e: return ioport("START2")->read();
		case 0x10: return ioport("HOLD3")->read();
		case 0x12: return ioport("HOLD4")->read();
		case 0x14: return ioport("HOLD2")->read();
		case 0x16: return ioport("HOLD1")->read();
		case 0x18: return ioport("HOLD5")->read();
		case 0x1a: return ioport("START1")->read();
		case 0x1c: return ioport("BET")->read();
		case 0x1e: return 0xff; //ticket motor
		case 0x20: return 0xff; //hopper motor
		case 0x2c: return m_eeprom->do_read();
		case 0x2e: return m_eeprom->do_read();
//      default: printf("R %02x\n",offset*2); break;
	}

//  printf("R %02x\n",offset*2);
	return 0xff;
}

WRITE_LINE_MEMBER(jackpool_state::map_vreg_w)
{
	m_map_vreg = state;
}

void jackpool_state::jackpool_mem(address_map &map)
{
	map(0x000000, 0x03ffff).rom();
	map(0x100000, 0x10ffff).ram();
	map(0x120000, 0x1200ff).ram();
	map(0x340000, 0x347fff).ram().share("vram");
	map(0x348000, 0x34ffff).ram(); //<- vram banks 2 & 3?

	map(0x360000, 0x3603ff).ram().w(m_palette, FUNC(palette_device::write16)).share("palette");
	map(0x380000, 0x38002f).r(this, FUNC(jackpool_state::jackpool_io_r)).umask16(0x00ff);
	map(0x380030, 0x38003f).w("latch1", FUNC(ls259_device::write_d0)).umask16(0x00ff);
	map(0x380040, 0x38004f).w("latch2", FUNC(ls259_device::write_d0)).umask16(0x00ff);
	map(0x380050, 0x38005f).w("latch3", FUNC(ls259_device::write_d0)).umask16(0x00ff);
	map(0x380060, 0x380061).nopw(); // another single-bit output?

	map(0x800000, 0x80000f).rw("uart", FUNC(ns16550_device::ins8250_r), FUNC(ns16550_device::ins8250_w)).umask16(0x00ff);
	map(0xa00001, 0xa00001).rw("oki", FUNC(okim6295_device::read), FUNC(okim6295_device::write));
}


static INPUT_PORTS_START( jackpool )
	PORT_START("COIN1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0xfe, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_START("SERVICE1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0xfe, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_START("SERVICE2") //toggle this to change game to Jackpot Pool,with different gfxs for cards.
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_SERVICE2 )
	PORT_BIT( 0xfe, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_START("PAYOUT")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_GAMBLE_PAYOUT )
	PORT_BIT( 0xfe, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_START("START1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_GAMBLE_DEAL ) PORT_CODE(KEYCODE_1) PORT_NAME("Deal / W-Up")
	PORT_BIT( 0xfe, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_START("START2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0xfe, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_START("BET")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_GAMBLE_BET ) PORT_NAME("Bet / Cancel / Take")
	PORT_BIT( 0xfe, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_START("HOLD1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_POKER_HOLD1 )
	PORT_BIT( 0xfe, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_START("HOLD2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_POKER_HOLD2 ) PORT_NAME("Hold 2 / Low")
	PORT_BIT( 0xfe, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_START("HOLD3")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_POKER_HOLD3 )
	PORT_BIT( 0xfe, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_START("HOLD4")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_POKER_HOLD4 ) PORT_NAME("Hold 4 / High")
	PORT_BIT( 0xfe, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_START("HOLD5")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_POKER_HOLD5 )
	PORT_BIT( 0xfe, IP_ACTIVE_LOW, IPT_UNUSED )
	/* these two both crashes the CPU*/
	PORT_START("UNK1")
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_START("UNK2")
	PORT_BIT( 0xff, IP_ACTIVE_LOW, IPT_UNUSED )
INPUT_PORTS_END


static const gfx_layout tiles8x8_layout =
{
	8,8,
	RGN_FRAC(1,4),
	4,
	{ RGN_FRAC(0,4),RGN_FRAC(1,4),RGN_FRAC(2,4),RGN_FRAC(3,4) },
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	8*8
};

static GFXDECODE_START( gfx_jackpool )
	GFXDECODE_ENTRY( "gfx1", 0, tiles8x8_layout,   0x000, 0x20  ) /* sprites */
GFXDECODE_END


/*irq 2 used for communication stuff.3 is just a rte*/
INTERRUPT_GEN_MEMBER(jackpool_state::jackpool_interrupt)
{
	device.execute().set_input_line(1, HOLD_LINE);
}


MACHINE_CONFIG_START(jackpool_state::jackpool)
	MCFG_DEVICE_ADD("maincpu", M68000, 12000000) // ?
	MCFG_DEVICE_PROGRAM_MAP(jackpool_mem)
	MCFG_DEVICE_VBLANK_INT_DRIVER("screen", jackpool_state, jackpool_interrupt)  // ?

	MCFG_DEVICE_ADD("gfxdecode", GFXDECODE, "palette", gfx_jackpool)

	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_REFRESH_RATE(60)
	MCFG_SCREEN_VBLANK_TIME(ATTOSECONDS_IN_USEC(2500))
	MCFG_SCREEN_SIZE(64*8, 64*8)
	MCFG_SCREEN_VISIBLE_AREA(0*8, 64*8-1, 0*8, 32*8-1)
	MCFG_SCREEN_UPDATE_DRIVER(jackpool_state, screen_update_jackpool)
	MCFG_SCREEN_PALETTE("palette")

	MCFG_DEVICE_ADD("latch1", LS259, 0)
	MCFG_ADDRESSABLE_LATCH_Q0_OUT_CB(NOOP) // HOLD3 lamp
	MCFG_ADDRESSABLE_LATCH_Q1_OUT_CB(NOOP) // HOLD4 lamp
	MCFG_ADDRESSABLE_LATCH_Q2_OUT_CB(NOOP) // HOLD2 lamp
	MCFG_ADDRESSABLE_LATCH_Q3_OUT_CB(NOOP) // HOLD1 lamp
	MCFG_ADDRESSABLE_LATCH_Q4_OUT_CB(NOOP) // HOLD5 lamp
	MCFG_ADDRESSABLE_LATCH_Q5_OUT_CB(NOOP) // START1 lamp
	MCFG_ADDRESSABLE_LATCH_Q6_OUT_CB(NOOP) // BET lamp

	MCFG_DEVICE_ADD("latch2", LS259, 0)
	MCFG_ADDRESSABLE_LATCH_Q0_OUT_CB(NOOP) // PAYOUT lamp
	MCFG_ADDRESSABLE_LATCH_Q3_OUT_CB(NOOP) // Coin counter
	MCFG_ADDRESSABLE_LATCH_Q5_OUT_CB(NOOP) // Ticket motor
	MCFG_ADDRESSABLE_LATCH_Q6_OUT_CB(NOOP) // Hopper motor
	MCFG_ADDRESSABLE_LATCH_Q7_OUT_CB(WRITELINE(*this, jackpool_state, map_vreg_w))

	MCFG_DEVICE_ADD("latch3", LS259, 0)
	MCFG_ADDRESSABLE_LATCH_Q0_OUT_CB(WRITELINE("eeprom", eeprom_serial_93cxx_device, cs_write))
	MCFG_ADDRESSABLE_LATCH_Q1_OUT_CB(WRITELINE("eeprom", eeprom_serial_93cxx_device, clk_write))
	MCFG_ADDRESSABLE_LATCH_Q2_OUT_CB(WRITELINE("eeprom", eeprom_serial_93cxx_device, di_write))

	MCFG_EEPROM_SERIAL_93C46_ADD("eeprom")

	MCFG_DEVICE_ADD("uart", NS16550, 1843200) // exact type and clock unknown

	MCFG_PALETTE_ADD("palette", 0x200)
	MCFG_PALETTE_FORMAT(xxxxBBBBGGGGRRRR)

	SPEAKER(config, "mono").front_center();

	MCFG_DEVICE_ADD("oki", OKIM6295, 1056000, okim6295_device::PIN7_HIGH) // clock frequency & pin 7 not verified
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)
MACHINE_CONFIG_END


ROM_START( jackpool )
	ROM_REGION( 0x40000, "maincpu", 0 ) /* 68000 Code */
	ROM_LOAD16_BYTE( "jpc2", 0x00001, 0x20000,CRC(5aad51ff) SHA1(af504d15c356c241efb6410a5dad09494d693eca) )
	ROM_LOAD16_BYTE( "jpc3", 0x00000, 0x20000,CRC(249c7073) SHA1(e654232d5f454932a108591deacadc9da9fd8055) )

	ROM_REGION( 0x080000, "oki", 0 ) /* Samples */
	ROM_LOAD( "jpc1", 0x00000, 0x40000, CRC(0f1372a1) SHA1(cec8a9bfb03945af4e1e2d2b916b9ded52a8d0bd) )

	ROM_REGION( 0x100000, "gfx1", 0 ) /* Sprites */
	ROM_LOAD( "jpc4", 0x00000, 0x40000,  CRC(b719f138) SHA1(82799cbccab4e39627e48855f6003917602b42c7) )
	ROM_LOAD( "jpc5", 0x40000, 0x40000,  CRC(09661ed9) SHA1(fb298252c95a9040441c12c9d0e9280843d56a0d) )
	ROM_LOAD( "jpc6", 0x80000, 0x40000,  CRC(c3117411) SHA1(8ed044beb1d6ab7ac48595f7d6bf879f1264454a) )
	ROM_LOAD( "jpc7", 0xc0000, 0x40000,  CRC(b1d40623) SHA1(fb76ae6b53474bd4bee19dbce9537da0f2b63ff4) )
ROM_END

void jackpool_state::init_jackpool()
{
	uint16_t *rom = (uint16_t *)memregion("maincpu")->base();

	/* patch NVRAM routine */
	rom[0x9040/2] = 0x6602;
}

GAME( 1997, jackpool, 0, jackpool, jackpool, jackpool_state, init_jackpool, ROT0, "Electronic Projects", "Jackpot Cards / Jackpot Pool (Italy)",MACHINE_NOT_WORKING )
