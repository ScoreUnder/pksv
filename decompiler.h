/*
		Charles Daffern (Score_Under)'s PKSV - Pokemon ROM script viewer/editor
		Copyright (C) 2007  Charles Daffern

		This program is free software: you can redistribute it and/or modify
		it under the terms of the GNU General Public License as published by
		the Free Software Foundation, either version 3 of the License, or
		(at your option) any later version.

		This program is distributed in the hope that it will be useful,
		but WITHOUT ANY WARRANTY; without even the implied warranty of
		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
		GNU General Public License for more details.

		You should have received a copy of the GNU General Public License
		along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
char gabe=0,comparetype=0;
char* GetFlagName(unsigned int a)
{
	if(mode==FIRE_RED)
	switch (a)
	{
		case 0x828:return "FR_POKEMON";
		case 0x829:return "FR_POKEDEX";
		case 0x82F:return "FR_RUNNING_SHOES";
		case 0x820:return "FR_BADGE_1";
		case 0x821:return "FR_BADGE_2";
		case 0x822:return "FR_BADGE_3";
		case 0x823:return "FR_BADGE_4";
		case 0x824:return "FR_BADGE_5";
		case 0x825:return "FR_BADGE_6";
		case 0x826:return "FR_BADGE_7";
		case 0x827:return "FR_BADGE_8";
	}
	else if(mode==RUBY)
	switch (a)
	{
		case 0x800:return "RS_POKEMON";
		case 0x801:return "RS_POKEDEX";
		case 0x802:return "RS_POKENAV";
		case 0x807:return "RS_BADGE_1";
		case 0x808:return "RS_BADGE_2";
		case 0x809:return "RS_BADGE_3";
		case 0x80A:return "RS_BADGE_4";
		case 0x80B:return "RS_BADGE_5";
		case 0x80C:return "RS_BADGE_6";
		case 0x80D:return "RS_BADGE_7";
		case 0x80E:return "RS_BADGE_8";
	}
	return NULL;
}

char* GetAttackName(unsigned int a)
{
	switch (a)
	{
	case 0x1:return "POUND";
	case 0x2:return "KARATECHOP";
	case 0x3:return "DOUBLESLAP";
	case 0x4:return "COMETPUNCH";
	case 0x5:return "MEGAPUNCH";
	case 0x6:return "PAYDAY";
	case 0x7:return "FIREPUNCH";
	case 0x8:return "ICEPUNCH";
	case 0x9:return "THUNDERPUNCH";
	case 0xA:return "SCRATCH";
	case 0xB:return "VICEGRIP";
	case 0xC:return "GUILLOTINE";
	case 0xD:return "RAZORWIND";
	case 0xE:return "SWORDSDANCE";
	case 0xF:return "CUT";
	case 0x10:return "GUST";
	case 0x11:return "WINGATTACK";
	case 0x12:return "WHIRLWIND";
	case 0x13:return "FLY";
	case 0x14:return "BIND";
	case 0x15:return "SLAM";
	case 0x16:return "VINEWHIP";
	case 0x17:return "STOMP";
	case 0x18:return "DOUBLEKICK";
	case 0x19:return "MEGAKICK";
	case 0x1A:return "JUMPKICK";
	case 0x1B:return "ROLLINGKICK";
	case 0x1C:return "SANDATTACK";
	case 0x1D:return "HEADBUTT";
	case 0x1E:return "HORNATTACK";
	case 0x1F:return "FURYATTACK";
	case 0x20:return "HORNDRILL";
	case 0x21:return "TACKLE";
	case 0x22:return "BODYSLAM";
	case 0x23:return "WRAP";
	case 0x24:return "TAKEDOWN";
	case 0x25:return "THRASH";
	case 0x26:return "DOUBLEEDGE";
	case 0x27:return "TAILWHIP";
	case 0x28:return "POISONSTING";
	case 0x29:return "TWINEEDLE";
	case 0x2A:return "PINMISSLE";
	case 0x2B:return "LEER";
	case 0x2C:return "BITE";
	case 0x2D:return "GROWL";
	case 0x2E:return "ROAR";
	case 0x2F:return "SING";
	case 0x30:return "SUPERSONIC";
	case 0x31:return "SONICBOOM";
	case 0x32:return "DISABLE";
	case 0x33:return "ACID";
	case 0x34:return "EMBER";
	case 0x35:return "FLAMETHROWER";
	case 0x36:return "MIST";
	case 0x37:return "WATERGUN";
	case 0x38:return "HYDROPUMP";
	case 0x39:return "SURF";
	case 0x3A:return "ICEBEAM";
	case 0x3B:return "BLIZZARD";
	case 0x3C:return "PSYBEAM";
	case 0x3D:return "BUBBLEBEAM";
	case 0x3E:return "AURORABEAM";
	case 0x3F:return "HYPERBEAM";
	case 0x40:return "PECK";
	case 0x41:return "DRILLPECK";
	case 0x42:return "SUBMISSION";
	case 0x43:return "LOWKICK";
	case 0x44:return "COUNTER";
	case 0x45:return "SEISMICTOSS";
	case 0x46:return "STRENGTH";
	case 0x47:return "ABSORB";
	case 0x48:return "MEGADRAIN";
	case 0x49:return "LEECHSEED";
	case 0x4A:return "GROWTH";
	case 0x4B:return "RAZORLEAF";
	case 0x4C:return "SOLARBEAM";
	case 0x4D:return "POISONPOWDER";
	case 0x4E:return "STUNSPORE";
	case 0x4F:return "SLEEPPOWDER";
	case 0x50:return "PETALDANCE";
	case 0x51:return "STRINGSHOT";
	case 0x52:return "DRAGONRAGE";
	case 0x53:return "FIRESPIN";
	case 0x54:return "THUNDERSHOCK";
	case 0x55:return "THUNDERBOLT";
	case 0x56:return "THUNDERWAVE";
	case 0x57:return "THUNDER";
	case 0x58:return "ROCKTHROW";
	case 0x59:return "EARTHQUAKE";
	case 0x5A:return "FISSURE";
	case 0x5B:return "DIG";
	case 0x5C:return "TOXIC";
	case 0x5D:return "CONFUSION";
	case 0x5E:return "PSYCHIC";
	case 0x5F:return "HYPNOSIS";
	case 0x60:return "MEDITATE";
	case 0x61:return "AGILITY";
	case 0x62:return "QUICKATTACK";
	case 0x63:return "RAGE";
	case 0x64:return "TELEPORT";
	case 0x65:return "NIGHTSHADE";
	case 0x66:return "MIMIC";
	case 0x67:return "SCREECH";
	case 0x68:return "DOUBLETEAM";
	case 0x69:return "RECOVER";
	case 0x6A:return "HARDEN";
	case 0x6B:return "MINIMIZE";
	case 0x6C:return "SMOKESCREEN";
	case 0x6D:return "CONFUSERAY";
	case 0x6E:return "WITHDRAW";
	case 0x6F:return "DEFENSECURL";
	case 0x70:return "BARRIER";
	case 0x71:return "LIGHTSCREEN";
	case 0x72:return "HAZE";
	case 0x73:return "REFLECT";
	case 0x74:return "FOCUSENERGY";
	case 0x75:return "BIDE";
	case 0x76:return "METRONOME";
	case 0x77:return "MIRRORMOVE";
	case 0x78:return "SELFDESTRUCT";
	case 0x79:return "EGGBOMB";
	case 0x7A:return "LICK";
	case 0x7B:return "SMOG";
	case 0x7C:return "SLUDGE";
	case 0x7D:return "BONECLUB";
	case 0x7E:return "FIREBLAST";
	case 0x7F:return "WATERFALL";
	case 0x80:return "CLAMP";
	case 0x81:return "SWIFT";
	case 0x82:return "SKULLBASH";
	case 0x83:return "SPIKECANNON";
	case 0x84:return "CONSTRICT";
	case 0x85:return "AMNESIA";
	case 0x86:return "KINESIS";
	case 0x87:return "SOFTBOILED";
	case 0x88:return "HIJUMPKICK";
	case 0x89:return "GLARE";
	case 0x8A:return "DREAMEATER";
	case 0x8B:return "POISONGAS";
	case 0x8C:return "BARRAGE";
	case 0x8D:return "LEECHLIFE";
	case 0x8E:return "LOVELYKISS";
	case 0x8F:return "SKYATTACK";
	case 0x90:return "TRANSFORM";
	case 0x91:return "BUBBLE";
	case 0x92:return "DIZZYPUNCH";
	case 0x93:return "SPORE";
	case 0x94:return "FLASH";
	case 0x95:return "PSYWAVE";
	case 0x96:return "SPLASH";
	case 0x97:return "ACIDARMOR";
	case 0x98:return "CRABHAMMER";
	case 0x99:return "EXPLOSION";
	case 0x9A:return "FURYSWIPES";
	case 0x9B:return "BONEMERANG";
	case 0x9C:return "REST";
	case 0x9D:return "ROCKSLIDE";
	case 0x9E:return "HYPERFANG";
	case 0x9F:return "SHARPEN";
	case 0xA0:return "CONVERSION";
	case 0xA1:return "TRIATTACK";
	case 0xA2:return "SUPERFANG";
	case 0xA3:return "SLASH";
	case 0xA4:return "SUBSTITUTE";
	case 0xA5:return "STRUGGLE";
	case 0xA6:return "SKETCH";
	case 0xA7:return "TRIPLEKICK";
	case 0xA8:return "THIEF";
	case 0xA9:return "SPIDERWEB";
	case 0xAA:return "MINDREADER";
	case 0xAB:return "NIGHTMARE";
	case 0xAC:return "FLAMEWHEEL";
	case 0xAD:return "SNORE";
	case 0xAE:return "CURSE";
	case 0xAF:return "FLAIL";
	case 0xB0:return "CONVERSION2";
	case 0xB1:return "AEROBLAST";
	case 0xB2:return "COTTONSPORE";
	case 0xB3:return "REVERSAL";
	case 0xB4:return "SPITE";
	case 0xB5:return "POWDERSNOW";
	case 0xB6:return "PROTECT";
	case 0xB7:return "MACHPUNCH";
	case 0xB8:return "SCARYFACE";
	case 0xB9:return "FAINTATTACK";
	case 0xBA:return "LOVELYKISS";
	case 0xBB:return "BELLYDRUM";
	case 0xBC:return "SLUDGEBOMB";
	case 0xBD:return "MUDSLAP";
	case 0xBE:return "OCTAZOOKA";
	case 0xBF:return "SPIKES";
	case 0xC0:return "ZAPCANNON";
	case 0xC1:return "FORESIGHT";
	case 0xC2:return "DESTINYBOND";
	case 0xC3:return "PERISHSONG";
	case 0xC4:return "ICYWIND";
	case 0xC5:return "DETECT";
	case 0xC6:return "BONERUSH";
	case 0xC7:return "LOCKON";
	case 0xC8:return "OUTRAGE";
	case 0xC9:return "SANDSTORM";
	case 0xCA:return "GIGADRAIN";
	case 0xCB:return "ENDURE";
	case 0xCC:return "CHARM";
	case 0xCD:return "ROLLOUT";
	case 0xCE:return "FALSESWIPE";
	case 0xCF:return "SWAGGER";
	case 0xD0:return "MILKDRINK";
	case 0xD1:return "SPARK";
	case 0xD2:return "FURYCUTTER";
	case 0xD3:return "STEELWING";
	case 0xD4:return "MEANLOOK";
	case 0xD5:return "ATTRACT";
	case 0xD6:return "SLEEPTALK";
	case 0xD7:return "HEALBELL";
	case 0xD8:return "RETURN";
	case 0xD9:return "PRESENT";
	case 0xDA:return "FRUSTRATION";
	case 0xDB:return "SAFEGUARD";
	case 0xDC:return "PAINSPLIT";
	case 0xDD:return "SACREDFIRE";
	case 0xDE:return "MAGNITUTDE";
	case 0xDF:return "DYNAMICPUNCH";
	case 0xE0:return "MEGAHORN";
	case 0xE1:return "DRAGONBREATH";
	case 0xE2:return "BATONPASS";
	case 0xE3:return "ENCORE";
	case 0xE4:return "PURSUIT";
	case 0xE5:return "RAPIDSPIN";
	case 0xE6:return "SWEETSCENT";
	case 0xE7:return "IRONTAIL";
	case 0xE8:return "METALCLAW";
	case 0xE9:return "VITALTHROW";
	case 0xEA:return "MORNINGSUN";
	case 0xEB:return "SYNTHESIS";
	case 0xEC:return "MOONLIGHT";
	case 0xED:return "HIDDENPOWER";
	case 0xEE:return "CROSSCHOP";
	case 0xEF:return "TWISTER";
	case 0xF0:return "RAINDANCE";
	case 0xF1:return "SUNNYDAY";
	case 0xF2:return "CRUNCH";
	case 0xF3:return "MIRRORCOAT";
	case 0xF4:return "PSYCHUP";
	case 0xF5:return "EXTREMESPEED";
	case 0xF6:return "ANCIENTPOWER";
	case 0xF7:return "SHADOWBALL";
	case 0xF8:return "FUTURESIGHT";
	case 0xF9:return "ROCKSMASH";
	case 0xFA:return "WHIRLPOOL";
	case 0xFB:return "BEATUP";
	case 0xFC:return "FAKEOUT";
	case 0xFD:return "UPROAR";
	case 0xFE:return "STOCKPILE";
	case 0xFF:return "SPITUP";
	case 0x100:return "SWALLOW";
	case 0x101:return "HEATWAVE";
	case 0x102:return "HAIL";
	case 0x103:return "TORMENT";
	case 0x104:return "FLATTER";
	case 0x105:return "WILLOWISP";
	case 0x106:return "MEMENTO";
	case 0x107:return "FACADE";
	case 0x108:return "FOCUSPUNCH";
	case 0x109:return "SMELLINGSALT";
	case 0x10A:return "FOLLOWME";
	case 0x10B:return "NATUREPOWER";
	case 0x10C:return "CHARGE";
	case 0x10D:return "TAUNT";
	case 0x10E:return "HELPINGHAND";
	case 0x10F:return "TRICK";
	case 0x110:return "ROLEPLAY";
	case 0x111:return "WISH";
	case 0x112:return "ASSIST";
	case 0x113:return "INGRAIN";
	case 0x114:return "SUPERPOWER";
	case 0x115:return "MAGICCOAT";
	case 0x116:return "RECYCLE";
	case 0x117:return "REVENGE";
	case 0x118:return "BRICKBREAK";
	case 0x119:return "YAWN";
	case 0x11A:return "KNOCKOFF";
	case 0x11B:return "ENDEAVOUR";
	case 0x11C:return "ERUPTION";
	case 0x11D:return "SKILLSWAP";
	case 0x11E:return "IMPRISON";
	case 0x11F:return "REFRESH";
	case 0x120:return "GRUDGE";
	case 0x121:return "SNATCH";
	case 0x122:return "SECRETPOWER";
	case 0x123:return "DIVE";
	case 0x124:return "ARMTHRUST";
	case 0x125:return "CAMOUFLAGE";
	case 0x126:return "TAILGLOW";
	case 0x127:return "LUSTERPURGE";
	case 0x128:return "MISTBALL";
	case 0x129:return "FEATHERDANCE";
	case 0x12A:return "TEETERDANCE";
	case 0x12B:return "BLAZEKICK";
	case 0x12C:return "MUDSPORT";
	case 0x12D:return "ICEBALL";
	case 0x12E:return "NEEDLEARM";
	case 0x12F:return "SLACKOFF";
	case 0x130:return "HYPERVOICE";
	case 0x131:return "POISONFANG";
	case 0x132:return "CRUSHCLAW";
	case 0x133:return "BLASTBURN";
	case 0x134:return "HYDROCANNON";
	case 0x135:return "METEORMASH";
	case 0x136:return "ASTONISH";
	case 0x137:return "WEATHERBALL";
	case 0x138:return "AROMATHERAPY";
	case 0x139:return "FAKETEARS";
	case 0x13A:return "AIRCUTTER";
	case 0x13B:return "OVERHEAT";
	case 0x13C:return "ODORSLEUTH";
	case 0x13D:return "ROCKTOMB";
	case 0x13E:return "SILVERWIND";
	case 0x13F:return "METALSOUND";
	case 0x140:return "GRASSWHISTLE";
	case 0x141:return "TICKLE";
	case 0x142:return "COSMICPOWER";
	case 0x143:return "WATERSPOUT";
	case 0x144:return "SIGNALBEAM";
	case 0x145:return "SHADOWPUNCH";
	case 0x146:return "EXTRASENSORY";
	case 0x147:return "SKYUPPERCUT";
	case 0x148:return "SANDTOMB";
	case 0x149:return "SHEERCOLD";
	case 0x14A:return "MUDDYWATER";
	case 0x14B:return "BULLETSEED";
	case 0x14C:return "AERIALACE";
	case 0x14D:return "ICICLESPEAR";
	case 0x14E:return "IRONDEFENSE";
	case 0x14F:return "BLOCK";
	case 0x150:return "HOWL";
	case 0x151:return "DRAGONCLAW";
	case 0x152:return "FRENZYPLANT";
	case 0x153:return "BULKUP";
	case 0x154:return "BOUNCE";
	case 0x155:return "MUDSHOT";
	case 0x156:return "POISONTAIL";
	case 0x157:return "COVET";
	case 0x158:return "VOLTTACKLE";
	case 0x159:return "MAGICALLEAF";
	case 0x15A:return "WATERSPORT";
	case 0x15B:return "CALMMIND";
	case 0x15C:return "LEAFBLADE";
	case 0x15D:return "DRAGONDANCE";
	case 0x15E:return "ROCKBLAST";
	case 0x15F:return "SHOCKWAVE";
	case 0x160:return "WATERPULSE";
	case 0x161:return "DOOMDESIRE";
	case 0x162:return "PSYCHOBOOST";
	}
	return NULL;
}

char* GetPokeName(unsigned int a)
{
	switch (a)
	{
	case 0: return "MISSINGNO";
case 1: return "BULBASAUR";
case 2: return "IVYSAUR";
case 3: return "VENUSAUR";
case 4: return "CHARMANDER";
case 5: return "CHARMELEON";
case 6: return "CHARIZARD";
case 7: return "SQUIRTLE";
case 8: return "WARTORTLE";
case 9: return "BLASTOISE";
case 10: return "CATERPIE";
case 11: return "METAPOD";
case 12: return "BUTTERFREE";
case 13: return "WEEDLE";
case 14: return "KAKUNA";
case 15: return "BEEDRILL";
case 16: return "PIDGEY";
case 17: return "PIDGEOTTO";
case 18: return "PIDGEOT";
case 19: return "RATTATA";
case 20: return "RATICATE";
case 21: return "SPEAROW";
case 22: return "FEAROW";
case 23: return "EKANS";
case 24: return "ARBOK";
case 25: return "PIKACHU";
case 26: return "RAICHU";
case 27: return "SANDSHREW";
case 28: return "SANDSLASH";
case 29: return "NIDORANF";
case 30: return "NIDORINA";
case 31: return "NIDOQUEEN";
case 32: return "NIDORANM";
case 33: return "NIDORINO";
case 34: return "NIDOKING";
case 35: return "CLEFAIRY";
case 36: return "CLEFABLE";
case 37: return "VULPIX";
case 38: return "NINETALES";
case 39: return "JIGGLYPUFF";
case 40: return "WIGGLYTUFF";
case 41: return "ZUBAT";
case 42: return "GOLBAT";
case 43: return "ODDISH";
case 44: return "GLOOM";
case 45: return "VILEPLUME";
case 46: return "PARAS";
case 47: return "PARASECT";
case 48: return "VENONAT";
case 49: return "VENOMOTH";
case 50: return "DIGLETT";
case 51: return "DUGTRIO";
case 52: return "MEOWTH";
case 53: return "PERSIAN";
case 54: return "PSYDUCK";
case 55: return "GOLDUCK";
case 56: return "MANKEY";
case 57: return "PRIMEAPE";
case 58: return "GROWLITHE";
case 59: return "ARCANINE";
case 60: return "POLIWAG";
case 61: return "POLIWHIRL";
case 62: return "POLIWRATH";
case 63: return "ABRA";
case 64: return "KADABRA";
case 65: return "ALAKAZAM";
case 66: return "MACHOP";
case 67: return "MACHOKE";
case 68: return "MACHAMP";
case 69: return "BELLSPROUT";
case 70: return "WEEPINBELL";
case 71: return "VICTREEBEL";
case 72: return "TENTACOOL";
case 73: return "TENTACRUEL";
case 74: return "GEODUDE";
case 75: return "GRAVELER";
case 76: return "GOLEM";
case 77: return "PONYTA";
case 78: return "RAPIDASH";
case 79: return "SLOWPOKE";
case 80: return "SLOWBRO";
case 81: return "MAGNEMITE";
case 82: return "MAGNETON";
case 83: return "FARFETCHD";
case 84: return "DODUO";
case 85: return "DODRIO";
case 86: return "SEEL";
case 87: return "DEWGONG";
case 88: return "GRIMER";
case 89: return "MUK";
case 90: return "SHELLDER";
case 91: return "CLOYSTER";
case 92: return "GASTLY";
case 93: return "HAUNTER";
case 94: return "GENGAR";
case 95: return "ONIX";
case 96: return "DROWZEE";
case 97: return "HYPNO";
case 98: return "KRABBY";
case 99: return "KINGLER";
case 100: return "VOLTORB";
case 101: return "ELECTRODE";
case 102: return "EXEGGCUTE";
case 103: return "EXEGGUTOR";
case 104: return "CUBONE";
case 105: return "MAROWAK";
case 106: return "HITMONLEE";
case 107: return "HITMONCHAN";
case 108: return "LICKITUNG";
case 109: return "KOFFING";
case 110: return "WEEZING";
case 111: return "RHYHORN";
case 112: return "RHYDON";
case 113: return "CHANSEY";
case 114: return "TANGELA";
case 115: return "KANGASKHAN";
case 116: return "HORSEA";
case 117: return "SEADRA";
case 118: return "GOLDEEN";
case 119: return "SEAKING";
case 120: return "STARYU";
case 121: return "STARMIE";
case 122: return "MRMIME";
case 123: return "SCYTHER";
case 124: return "JYNX";
case 125: return "ELECTABUZZ";
case 126: return "MAGMAR";
case 127: return "PINSIR";
case 128: return "TAUROS";
case 129: return "MAGIKARP";
case 130: return "GYARADOS";
case 131: return "LAPRAS";
case 132: return "DITTO";
case 133: return "EEVEE";
case 134: return "VAPOREON";
case 135: return "JOLTEON";
case 136: return "FLAREON";
case 137: return "PORYGON";
case 138: return "OMANYTE";
case 139: return "OMASTAR";
case 140: return "KABUTO";
case 141: return "KABUTOPS";
case 142: return "AERODACTYL";
case 143: return "SNORLAX";
case 144: return "ARTICUNO";
case 145: return "ZAPDOS";
case 146: return "MOLTRES";
case 147: return "DRATINI";
case 148: return "DRAGONAIR";
case 149: return "DRAGONITE";
case 150: return "MEWTWO";
case 151: return "MEW";
case 152: return "CHIKORITA";
case 153: return "BAYLEEF";
case 154: return "MEGANIUM";
case 155: return "CYNDAQUIL";
case 156: return "QUILAVA";
case 157: return "TYPHLOSION";
case 158: return "TOTODILE";
case 159: return "CROCONAW";
case 160: return "FERALIGATR";
case 161: return "SENTRET";
case 162: return "FURRET";
case 163: return "HOOTHOOT";
case 164: return "NOCTOWL";
case 165: return "LEDYBA";
case 166: return "LEDIAN";
case 167: return "SPINARAK";
case 168: return "ARIADOS";
case 169: return "CROBAT";
case 170: return "CHINCHOU";
case 171: return "LANTURN";
case 172: return "PICHU";
case 173: return "CLEFFA";
case 174: return "IGGLYBUFF";
case 175: return "TOGEPI";
case 176: return "TOGETIC";
case 177: return "NATU";
case 178: return "XATU";
case 179: return "MAREEP";
case 180: return "FLAAFFY";
case 181: return "AMPHAROS";
case 182: return "BELLOSSOM";
case 183: return "MARILL";
case 184: return "AZUMARILL";
case 185: return "SUDOWOODO";
case 186: return "POLITOED";
case 187: return "HOPPIP";
case 188: return "SKIPLOOM";
case 189: return "JUMPLUFF";
case 190: return "AIPOM";
case 191: return "SUNKERN";
case 192: return "SUNFLORA";
case 193: return "YANMA";
case 194: return "WOOPER";
case 195: return "QUAGSIRE";
case 196: return "ESPEON";
case 197: return "UMBREON";
case 198: return "MURKROW";
case 199: return "SLOWKING";
case 200: return "MISDREAVUS";
case 201: return "UNOWN";
case 202: return "WOBBUFFET";
case 203: return "GIRAFARIG";
case 204: return "PINECO";
case 205: return "FORRETRESS";
case 206: return "DUNSPARCE";
case 207: return "GLIGAR";
case 208: return "STEELIX";
case 209: return "SNUBBULL";
case 210: return "GRANBULL";
case 211: return "QWILFISH";
case 212: return "SCIZOR";
case 213: return "SHUCKLE";
case 214: return "HERACROSS";
case 215: return "SNEASEL";
case 216: return "TEDDIURSA";
case 217: return "URSARING";
case 218: return "SLUGMA";
case 219: return "MAGCARGO";
case 220: return "SWINUB";
case 221: return "PILOSWINE";
case 222: return "CORSOLA";
case 223: return "REMORAID";
case 224: return "OCTILLERY";
case 225: return "DELIBIRD";
case 226: return "MANTINE";
case 227: return "SKARMORY";
case 228: return "HOUNDOUR";
case 229: return "HOUNDOOM";
case 230: return "KINGDRA";
case 231: return "PHANPY";
case 232: return "DONPHAN";
case 233: return "PORYGON2";
case 234: return "STANTLER";
case 235: return "SMEARGLE";
case 236: return "TYROGUE";
case 237: return "HITMONTOP";
case 238: return "SMOOCHUM";
case 239: return "ELEKID";
case 240: return "MAGBY";
case 241: return "MILTANK";
case 242: return "BLISSEY";
case 243: return "RAIKOU";
case 244: return "ENTEI";
case 245: return "SUICUNE";
case 246: return "LARVITAR";
case 247: return "PUPITAR";
case 248: return "TYRANITAR";
case 249: return "LUGIA";
case 250: return "HO-OH";
case 251: return "CELEBI";
case 277: return "TREECKO";
case 278: return "GROVYLE";
case 279: return "SCEPTILE";
case 280: return "TORCHIC";
case 281: return "COMBUSKEN";
case 282: return "BLAZIKEN";
case 283: return "MUDKIP";
case 284: return "MARSHTOMP";
case 285: return "SWAMPERT";
case 286: return "POOCHYENA";
case 287: return "MIGHTYENA";
case 288: return "ZIGZAGOON";
case 289: return "LINOONE";
case 290: return "WURMPLE";
case 291: return "SILCOON";
case 292: return "BEAUTIFLY";
case 293: return "CASCOON";
case 294: return "DUSTOX";
case 295: return "LOTAD";
case 296: return "LOMBRE";
case 297: return "LUDICOLO";
case 298: return "SEEDOT";
case 299: return "NUZLEAF";
case 300: return "SHIFTRY";
case 301: return "NINCADA";
case 302: return "NINJASK";
case 303: return "SHEDINJA";
case 304: return "TAILLOW";
case 305: return "SWELLOW";
case 306: return "SHROOMISH";
case 307: return "BRELOOM";
case 308: return "SPINDA";
case 309: return "WINGULL";
case 310: return "PELIPPER";
case 311: return "SURSKIT";
case 312: return "MASQUERAIN";
case 313: return "WAILMER";
case 314: return "WAILORD";
case 315: return "SKITTY";
case 316: return "DELCATTY";
case 317: return "KECLEON";
case 318: return "BALTOY";
case 319: return "CLAYDOL";
case 320: return "NOSEPASS";
case 321: return "TORKOAL";
case 322: return "SABLEYE";
case 323: return "BARBOACH";
case 324: return "WHISCASH";
case 325: return "LUVDISC";
case 326: return "CORPHISH";
case 327: return "CRAWDAUNT";
case 328: return "FEEBAS";
case 329: return "MILOTIC";
case 330: return "CARVANHA";
case 331: return "SHARPEDO";
case 332: return "TRAPINCH";
case 333: return "VIBRAVA";
case 334: return "FLYGON";
case 335: return "MAKUHITA";
case 336: return "HARIYAMA";
case 337: return "ELECTRIKE";
case 338: return "MANECTRIC";
case 339: return "NUMEL";
case 340: return "CAMERUPT";
case 341: return "SPHEAL";
case 342: return "SEALEO";
case 343: return "WALREIN";
case 344: return "CACNEA";
case 345: return "CACTURNE";
case 346: return "SNORUNT";
case 347: return "GLALIE";
case 348: return "LUNATONE";
case 349: return "SOLROCK";
case 350: return "AZURILL";
case 351: return "SPOINK";
case 352: return "GRUMPIG";
case 353: return "PLUSLE";
case 354: return "MINUN";
case 355: return "MAWILE";
case 356: return "MEDITITE";
case 357: return "MEDICHAM";
case 358: return "SWABLU";
case 359: return "ALTARIA";
case 360: return "WYNAUT";
case 361: return "DUSKULL";
case 362: return "DUSCLOPS";
case 363: return "ROSELIA";
case 364: return "SLAKOTH";
case 365: return "VIGOROTH";
case 366: return "SLAKING";
case 367: return "GULPIN";
case 368: return "SWALOT";
case 369: return "TROPIUS";
case 370: return "WHISMUR";
case 371: return "LOUDRED";
case 372: return "EXPLOUD";
case 373: return "CLAMPERL";
case 374: return "HUNTAIL";
case 375: return "GOREBYSS";
case 376: return "ABSOL";
case 377: return "SHUPPET";
case 378: return "BANETTE";
case 379: return "SEVIPER";
case 380: return "ZANGOOSE";
case 381: return "RELICANTH";
case 382: return "ARON";
case 383: return "LAIRON";
case 384: return "AGGRON";
case 385: return "CASTFORM";
case 386: return "VOLBEAT";
case 387: return "ILLUMISE";
case 388: return "LILEEP";
case 389: return "CRADILY";
case 390: return "ANORITH";
case 391: return "ARMALDO";
case 392: return "RALTS";
case 393: return "KIRLIA";
case 394: return "GARDEVOIR";
case 395: return "BAGON";
case 396: return "SHELGON";
case 397: return "SALAMENCE";
case 398: return "BELDUM";
case 399: return "METANG";
case 400: return "METAGROSS";
case 401: return "REGIROCK";
case 402: return "REGICE";
case 403: return "REGISTEEL";
case 404: return "KYOGRE";
case 405: return "GROUDON";
case 406: return "RAYQUAZA";
case 407: return "LATIAS";
case 408: return "LATIOS";
case 409: return "JIRACHI";
case 410: return "DEOXYS";
case 411: return "CHIMECHO";
	}
	return NULL;
}
char* GetItemName(unsigned int a)
{
	switch (a)
	{
	case 0x0:
		return "NONE";
	case 0x1:
		return "MASTERBALL";
	case 0x2:
		return "ULTRABALL";
	case 0x3:
		return "GREATBALL";
	case 0x4:
		return "POKEBALL";
	case 0x5:
		return "SAFARIBALL";
	case 0x6:
		return "NETBALL";
	case 0x7:
		return "DIVEBALL";
	case 0x8:
		return "NESTBALL";
	case 0x9:
		return "REPEATBALL";
	case 0xA:
		return "TIMERBALL";
	case 0xB:
		return "LUXURYBALL";
	case 0xC:
		return "PREMIERBALL";
	case 0xD:
		return "POTION";
	case 0xE:
		return "ANTIDOTE";
	case 0xF:
		return "BURNHEAL";
	case 0x10:
		return "ICEHEAL";
	case 0x11:
		return "AWAKENING";
	case 0x12:
		return "PARLYZHEAL";
	case 0x13:
		return "FULLRESTORE";
	case 0x14:
		return "MAXPOTION";
	case 0x15:
		return "HYPERPOTION";
	case 0x16:
		return "SUPERPOTION";
	case 0x17:
		return "FULLHEAL";
	case 0x18:
		return "REVIVE";
	case 0x19:
		return "MAXREVIVE";
	case 0x1A:
		return "FRESHWATER";
	case 0x1B:
		return "SODAPOP";
	case 0x1C:
		return "LEMONADE";
	case 0x1D:
		return "MOOMOOMILK";
	case 0x1E:
		return "ENERGYPOWDER";
	case 0x1F:
		return "ENERGYROOT";
	case 0x20:
		return "HEALPOWDER";
	case 0x21:
		return "REVIVALHERB";
	case 0x22:
		return "ETHER";
	case 0x23:
		return "MAXETHER";
	case 0x24:
		return "ELIXIR";
	case 0x25:
		return "MAXELIXIR";
	case 0x26:
		return "LAVACOOKIE";
	case 0x27:
		return "BLUEFLUTE";
	case 0x28:
		return "YELLOWFLUTE";
	case 0x29:
		return "REDFLUTE";
	case 0x2A:
		return "BLACKFLUTE";
	case 0x2B:
		return "WHITEFLUTE";
	case 0x2C:
		return "BERRYJUICE";
	case 0x2D:
		return "SACREDASH";
	case 0x2E:
		return "SHOALSALT";
	case 0x2F:
		return "SHOALSHELL";
	case 0x30:
		return "REDSHARD";
	case 0x31:
		return "BLUESHARD";
	case 0x32:
		return "YELLOWSHARD";
	case 0x33:
		return "GREENSHARD";
	case 0x3F:
		return "HPUP";
	case 0x40:
		return "PROTEIN";
	case 0x41:
		return "IRON";
	case 0x42:
		return "CARBOS";
	case 0x43:
		return "CALCIUM";
	case 0x44:
		return "RARECANDY";
	case 0x45:
		return "PPUP";
	case 0x46:
		return "ZINC";
	case 0x47:
		return "PPMAX";
	case 0x49:
		return "GUARDSPEC";
	case 0x4A:
		return "DIREHIT";
	case 0x4B:
		return "XATTACK";
	case 0x4C:
		return "XDEFEND";
	case 0x4D:
		return "XSPEED";
	case 0x4E:
		return "XACCURACY";
	case 0x4F:
		return "XSPECIAL";
	case 0x50:
		return "POKEDOLL";
	case 0x51:
		return "FLUFFYTAIL";
	case 0x53:
		return "SUPERREPEL";
	case 0x54:
		return "MAXREPEL";
	case 0x55:
		return "ESCAPEROPE";
	case 0x56:
		return "REPEL";
	case 0x5D:
		return "SUNSTONE";
	case 0x5E:
		return "MOONSTONE";
	case 0x5F:
		return "FIRESTONE";
	case 0x60:
		return "THUNDERSTONE";
	case 0x61:
		return "WATERSTONE";
	case 0x62:
		return "LEAFSTONE";
	case 0x67:
		return "TINYMUSHROOM";
	case 0x68:
		return "BIGMUSHROOM";
	case 0x6A:
		return "PEARL";
	case 0x6B:
		return "BIGPEARL";
	case 0x6C:
		return "STARDUST";
	case 0x6D:
		return "STARPIECE";
	case 0x6E:
		return "NUGGET";
	case 0x6F:
		return "HEARTSCALE";
	case 0x79:
		return "ORANGEMAIL";
	case 0x7A:
		return "HARBORMAIL";
	case 0x7B:
		return "GLITTERMAIL";
	case 0x7C:
		return "MECHMAIL";
	case 0x7D:
		return "WOODMAIL";
	case 0x7E:
		return "WAVEMAIL";
	case 0x7F:
		return "BEADMAIL";
	case 0x80:
		return "SHADOWMAIL";
	case 0x81:
		return "TROPICMAIL";
	case 0x82:
		return "DREAMMAIL";
	case 0x83:
		return "FABMAIL";
	case 0x84:
		return "RETROMAIL";
	case 0x85:
		return "CHERIBERRY";
	case 0x86:
		return "CHESTOBERRY";
	case 0x87:
		return "PECHABERRY";
	case 0x88:
		return "RAWSTBERRY";
	case 0x89:
		return "ASPEARBERRY";
	case 0x8A:
		return "LEPPABERRY";
	case 0x8B:
		return "ORANBERRY";
	case 0x8C:
		return "PERSIMBERRY";
	case 0x8D:
		return "LUMBERRY";
	case 0x8E:
		return "SITRUSBERRY";
	case 0x8F:
		return "FIGYBERRY";
	case 0x90:
		return "WIKIBERRY";
	case 0x91:
		return "MAGOBERRY";
	case 0x92:
		return "AGUAVBERRY";
	case 0x93:
		return "IAPAPABERRY";
	case 0x94:
		return "RAZZBERRY";
	case 0x95:
		return "BLUKBERRY";
	case 0x96:
		return "NANABBERRY";
	case 0x97:
		return "WEPEARBERRY";
	case 0x98:
		return "PINAPBERRY";
	case 0x99:
		return "POMEGBERRY";
	case 0x9A:
		return "KELPSYBERRY";
	case 0x9B:
		return "QUALOTBERRY";
	case 0x9C:
		return "HONDEWBERRY";
	case 0x9D:
		return "GREPABERRY";
	case 0x9E:
		return "TAMATOBERRY";
	case 0x9F:
		return "CORNNBERRY";
	case 0xA0:
		return "MAGOSTBERRY";
	case 0xA1:
		return "RABUTABERRY";
	case 0xA2:
		return "NOMELBERRY";
	case 0xA3:
		return "SPELONBERRY";
	case 0xA4:
		return "PAMTREBERRY";
	case 0xA5:
		return "WATMELBERRY";
	case 0xA6:
		return "DURINBERRY";
	case 0xA7:
		return "BELUEBERRY";
	case 0xA8:
		return "LIECHIBERRY";
	case 0xA9:
		return "GANLONBERRY";
	case 0xAA:
		return "SALACBERRY";
	case 0xAB:
		return "PETAYABERRY";
	case 0xAC:
		return "APICOTBERRY";
	case 0xAD:
		return "LANSATBERRY";
	case 0xAE:
		return "STARFBERRY";
	case 0xAF:
		return "ENIGMABERRY";
	case 0xB3:
		return "BRIGHTPOWDER";
	case 0xB4:
		return "WHITEHERB";
	case 0xB5:
		return "MACHOBRACE";
	case 0xB6:
		return "EXPSHARE";
	case 0xB7:
		return "QUICKCLAW";
	case 0xB8:
		return "SOOTHEBELL";
	case 0xB9:
		return "MENTALHERB";
	case 0xBA:
		return "CHOICEBAND";
	case 0xBB:
		return "KINGSROCK";
	case 0xBC:
		return "SILVERPOWDER";
	case 0xBD:
		return "AMULETCOIN";
	case 0xBE:
		return "CLEANSETAG";
	case 0xBF:
		return "SOULDEW";
	case 0xC0:
		return "DEEPSEATOOTH";
	case 0xC1:
		return "DEEPSEASCALE";
	case 0xC2:
		return "SMOKEBALL";
	case 0xC3:
		return "EVERSTONE";
	case 0xC4:
		return "FOCUSBAND";
	case 0xC5:
		return "LUCKYEGG";
	case 0xC6:
		return "SCOPELENS";
	case 0xC7:
		return "METALCOAT";
	case 0xC8:
		return "LEFTOVERS";
	case 0xC9:
		return "DRAGONSCALE";
	case 0xCA:
		return "LIGHTBALL";
	case 0xCB:
		return "SOFTSAND";
	case 0xCC:
		return "HARDSTONE";
	case 0xCD:
		return "MIRACLESEED";
	case 0xCE:
		return "BLACKGLASSES";
	case 0xCF:
		return "BLACKBELT";
	case 0xD0:
		return "MAGNET";
	case 0xD1:
		return "MYSTICWATER";
	case 0xD2:
		return "SHARPBEAK";
	case 0xD3:
		return "POISONBARB";
	case 0xD4:
		return "NEVERMELTICE";
	case 0xD5:
		return "SPELLTAG";
	case 0xD6:
		return "TWISTEDSPOON";
	case 0xD7:
		return "CHARCOAL";
	case 0xD8:
		return "DRAGONFANG";
	case 0xD9:
		return "SILKSCARF";
	case 0xDA:
		return "UPGRADE";
	case 0xDB:
		return "SHELLBELL";
	case 0xDC:
		return "SEAINCENSE";
	case 0xDD:
		return "LAXINCENSE";
	case 0xDE:
		return "LUCKYPUNCH";
	case 0xDF:
		return "METALPOWDER";
	case 0xE0:
		return "THICKCLUB";
	case 0xE1:
		return "STICK";
	case 0xFE:
		return "REDSCARF";
	case 0xFF:
		return "BLUESCARF";
	case 0x100:
		return "PINKSCARF";
	case 0x101:
		return "GREENSCARF";
	case 0x102:
		return "YELLOWSCARF";
	case 0x103:
		return "MACHBIKE";
	case 0x104:
		return "COINCASE";
	case 0x105:
		return "ITEMFINDER";
	case 0x106:
		return "OLDROD";
	case 0x107:
		return "GOODROD";
	case 0x108:
		return "SUPERROD";
	case 0x109:
		return "SSTICKET";
	case 0x10A:
		return "CONTESTPASS";
	case 0x10C:
		return "WAILMERPAIL";
	case 0x10D:
		return "DEVONGOODS";
	case 0x10E:
		return "SOOTSACK";
	case 0x10F:
		return "BASEMENTKEY";
	case 0x110:
		return "ACROBIKE";
	case 0x111:
		return "POKEBLOCKCASE";
	case 0x112:
		return "LETTER";
	case 0x113:
		return "EONTICKET";
	case 0x114:
		return "REDORB";
	case 0x115:
		return "BLUEORB";
	case 0x116:
		return "SCANNER";
	case 0x117:
		return "GOGOGGLES";
	case 0x118:
		return "METEORITE";
	case 0x119:
		return "RM1KEY";
	case 0x11A:
		return "RM2KEY";
	case 0x11B:
		return "RM4KEY";
	case 0x11C:
		return "RM6KEY";
	case 0x11D:
		return "STORAGEKEY";
	case 0x11E:
		return "ROOTFOSSIL";
	case 0x11F:
		return "CLAWFOSSIL";
	case 0x120:
		return "DEVONSCOPE";
		case 0x121: return "TM01";
case 0x122: return "TM02";
case 0x123: return "TM03";
case 0x124: return "TM04";
case 0x125: return "TM05";
case 0x126: return "TM06";
case 0x127: return "TM07";
case 0x128: return "TM08";
case 0x129: return "TM09";
case 0x12A: return "TM10";
case 0x12B: return "TM11";
case 0x12C: return "TM12";
case 0x12D: return "TM13";
case 0x12E: return "TM14";
case 0x12F: return "TM15";
case 0x130: return "TM16";
case 0x131: return "TM17";
case 0x132: return "TM18";
case 0x133: return "TM19";
case 0x134: return "TM20";
case 0x135: return "TM21";
case 0x136: return "TM22";
case 0x137: return "TM23";
case 0x138: return "TM24";
case 0x139: return "TM25";
case 0x13A: return "TM26";
case 0x13B: return "TM27";
case 0x13C: return "TM28";
case 0x13D: return "TM29";
case 0x13E: return "TM30";
case 0x13F: return "TM31";
case 0x140: return "TM32";
case 0x141: return "TM33";
case 0x142: return "TM34";
case 0x143: return "TM35";
case 0x144: return "TM36";
case 0x145: return "TM37";
case 0x146: return "TM38";
case 0x147: return "TM39";
case 0x148: return "TM40";
case 0x149: return "TM41";
case 0x14A: return "TM42";
case 0x14B: return "TM43";
case 0x14C: return "TM44";
case 0x14D: return "TM45";
case 0x14E: return "TM46";
case 0x14F: return "TM47";
case 0x150: return "TM48";
case 0x151: return "TM49";
case 0x152: return "TM50";
case 0x153: return "HM01";
case 0x154: return "HM02";
case 0x155: return "HM03";
case 0x156: return "HM04";
case 0x157: return "HM05";
case 0x158: return "HM06";
case 0x159: return "HM07";
case 0x15A: return "HM08";
case 0x15D: return "OAKSPARCEL";
case 0x15E: return "POKEFLUTE";
case 0x15F: return "SECRETKEY";
case 0x160: return "BIKEVOUCHER";
case 0x161: return "GOLDTEETH";
case 0x162: return "OLDAMBER";
case 0x163: return "CARDKEY";
case 0x164: return "LIFTKEY";
case 0x165: return "HELIXFOSSIL";
case 0x166: return "DOMEFOSSIL";
case 0x167: return "SILPHSCOPE";
case 0x168: return "BICYCLE";
case 0x169: return "TOWNMAP";
case 0x16A: return "VSSEEKER";
case 0x16B: return "FAMECHECKER";
case 0x16C: return "TMCASE";
case 0x16D: return "BERRYPOUCH";
case 0x16E: return "TEACHYTV";
case 0x16F: return "TRIPASS";
case 0x170: return "RAINBOWPASS";
case 0x171: return "TEA";
case 0x172: return "MYSTICTICKET";
case 0x173: return "AURORATICKET";
case 0x174: return "POWDERJAR";
case 0x175: return "RUBY";
case 0x176: return "SAPPHIRE";
case 0x177: return "MAGMAEMBLEM";
case 0x178: return "OLDSEAMAP";
	}
	return NULL;
}
unsigned int chr_count(char chrs,char*string) //cannot use chr, defined as i...
{
	unsigned int i=0;
	while (*string!=0)
	{
		if (*string==chrs)
			i++;
		string++;
	}
	return i;
}

FILE* scrf;

void DecodeProc2(FILE* fileM_,
								 unsigned int narc,
								 unsigned int FileZoomPos,
								 char*filename
#ifndef DLL
								 ,FILE*fsend
#endif
								)
{
	unsigned int still_going,arg1,arg2,arg3,arg4,arg5,arg6,arg7,endat,failsafe=0,lastdata,lastdata2;
	unsigned char command,lastcmd;
	register FILE* fileM=fileM_;
	size_t read;
	char buf[1024],buf2[1024],buf3[1024];
	fseek(fileM,FileZoomPos&0x07ffffff,SEEK_SET);

	still_going=1;
	if ((FileZoomPos&0x0000ffff)==0x1337||(FileZoomPos&0x00ffffff)==1337||(FileZoomPos&0x0000ffff)==0x1EE7)
	{
		if ((FileZoomPos&0x00ffffff)==1337)
		{
			fprintf(fsend,"'Address 1337 is officially L33T!\r\n");
		}
		else
		{
			fprintf(fsend,"'Address 0x%X is officially L33T!\r\n",(FileZoomPos&0x00ffffff));
		}
	}
	if (mode==GOLD||mode==CRYSTAL)
	{
		fprintf(fsend,"#org 0x%X\r\n",FileZoomPos);
	}
	else if (mode==DIAMOND)
	{
		fprintf(fsend,"#narc 0x%X 0x%X\r\n",narc,FileZoomPos);
		fseek(fileM,0xE,SEEK_SET);
		arg3=0;
		fread(&arg3,1,2,fileM);
		fseek(fileM,0x10,SEEK_SET);
		fread(&buf,1,4,fileM);
		buf[4]=0;
		arg5=0;
		while (arg5<arg3&&strcmp(buf,"BTAF"))
		{
			arg4=0;
			fread(&arg4,1,4,fileM);
			fseek(fileM,arg4-0x8,SEEK_CUR);
			fread(&buf,1,4,fileM);
			buf[4]=0;
			arg5++;
		}
		if (!strcmp(buf,"BTAF"))
		{
			arg1=0;
			endat=0;
			fseek(fileM,8*(narc+1),SEEK_CUR);
			fread(&arg1,1,4,fileM);
			fread(&endat,1,4,fileM);
			//fprintf(fsend,"'%X %X\r\n",arg1,endat);
			fseek(fileM,0x10,SEEK_SET);
			fread(&buf,1,4,fileM);
			buf[4]=0;
			arg5=0;
			while (arg5<arg3&&strcmp(buf,"GMIF"))
			{
				arg4=0;
				fread(&arg4,1,4,fileM);
				fseek(fileM,arg4-0x8,SEEK_CUR);
				fread(&buf,1,4,fileM);
				buf[4]=0;
				arg5++;
			}
			if (!strcmp(buf,"GMIF"))
			{
				arg2=ftell(fileM);
				endat+=arg2+4;
				//fprintf(fsend,"'%X %X\r\n",arg1+FileZoomPos+4+arg2,endat);
				fseek(fileM,4+arg1+FileZoomPos,SEEK_CUR);
			}
			else
			{
				fprintf(fsend,"'ERROR: Incomplete NARC.\r\n");
				return;
			}
		}
		else
		{
			fprintf(fsend,"'ERROR: Incomplete NARC.\r\n");
			return;
		}
	}
	else
	{
		if(dyndec)
		{
			if(!Defined2(FileZoomPos|0x08000000))
			{
				if(!codenum)
					Define2(FileZoomPos|0x08000000,"@start");
				else
				{
					sprintf(buf,"@code%u",codenum);
					Define2(FileZoomPos|0x08000000,buf);
				}
				codenum++;
			}
			fprintf(fsend,"#org %s\r\n",WhatIs2(FileZoomPos|0x08000000));
		}
		else
			fprintf(fsend,"#org 0x%X\r\n",(FileZoomPos|0x08000000));
	}
	/*if(!gabe&&mode==DIAMOND)
	{
		fprintf(fsend,"'gabe_k made DIAMOND scripting possible!\r\n");
		gabe=1;
	}*/
	fprintf(fsend,"'-----------------------------------\r\n");
	if (mode==DIAMOND)
	{
		while (still_going&&(signed int)ftell(fileM)<(signed int)endat)
		{
			lastcmd=command;
			read=fread(&command,1,1,fileM);
			if (read>0)
			{
#define GENERIC(x) fprintf(fsend,"%s\r\n",x)
				arg1=arg2=arg3=arg4=arg5=arg6=arg7=0;
				switch (command)
				{
				default:
					fprintf(fsend,"#raw 0x%X\r\n",command);
					break;
				}
			}
			else
			{
				puts("'--EOF--");
				still_going=0;
			}
		}
	}
	else if (mode!=GOLD&&mode!=CRYSTAL)
	{
		while (still_going)
		{
			lastcmd=command;
			read=(signed int)fread(&command,1,1,fileM);
			if (read>0)
			{
				arg1=arg2=arg3=arg4=arg5=arg6=arg7=0;

				switch (command)
				{
				case CMD_GIVEMONEY:
					fread(&arg1,1,4,fileM);
					fread(&arg2,1,1,fileM);
					fprintf(fsend,"givemoney 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case CMD_REAPPEARAT:
					fread(&arg1,1,2,fileM);
					fread(&arg2,1,1,fileM);
					fread(&arg3,1,1,fileM);
					fprintf(fsend,"reappearat 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
					break;
				case CMD_MOVESPRITE:
					fread(&arg1,1,2,fileM);
					fread(&arg2,1,2,fileM);
					fread(&arg3,1,2,fileM);
					fprintf(fsend,"movesprite 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
					break;
				case CMD_APPLYMOVEMENTFINISHAT:
					fread(&arg1,1,2,fileM);
					fread(&arg2,1,4,fileM);
					fread(&arg3,1,1,fileM);
					fread(&arg4,1,1,fileM);
					switch(arg1)
					{
						case 0x007F:strcpy(buf,"CAMERA");break;
						case 0x00FF:strcpy(buf,"PLAYER");break;
						case 0x800D:strcpy(buf,"LASTRESULT");break;
						case 0x800F:strcpy(buf,"LASTTALKED");break;
						default:sprintf(buf,"0x%X",arg1);
					}
					if ((arg2&0xF8000000)==0x08000000)
					{
						transmove(arg2,filename);
						if (trans[20]!=0)
						{
							strcpy(trans+20,"...");
						}
						if(dyndec)
						{
							if(!Defined2(arg2|0x08000000))
							{
								movenum++;
								sprintf(buf2,"@move%u",movenum);
								Define2(arg2|0x08000000,buf2);
							}
							fprintf(fsend,"applymovementfinishat %s 0x%X 0x%X 0x%X ' %s\r\n",buf,WhatIs2(arg2|0x08000000),arg3,arg4,trans);
						}
						else
							fprintf(fsend,"applymovementfinishat %s 0x%X 0x%X 0x%X ' %s\r\n",buf,arg2,arg3,arg4,trans);
						DoMove(arg2);
					}
					else
					{
						fprintf(fsend,"applymovementfinishat %s 0x%X 0x%X 0x%X ' Not in ROM\r\n",buf,arg2,arg3,arg4);
					}
					break;
				case CMD_DISAPPEARAT:
					fread(&arg1,1,2,fileM);
					fread(&arg2,1,1,fileM);
					fread(&arg3,1,1,fileM);
					fprintf(fsend,"disappearat 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
					break;
				case CMD_CHECKFURNITURE:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"checkfurniture 0x%X\r\n",arg1);
					break;
				case CMD_TAKEFURNITURE:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"takefurniture 0x%X\r\n",arg1);
					break;
				case CMD_CHECKITEMINPC:
					fread(&arg1,1,2,fileM);
					fread(&arg2,1,2,fileM);
					if(!GetItemName(arg1))
						sprintf(buf,"0x%X",arg1);
					else
						strcpy(buf,GetItemName(arg1));
					fprintf(fsend,"checkiteminpc %s %u\r\n",buf,arg2);
					break;
				case CMD_GIVEITEMTOPC:
					fread(&arg1,1,2,fileM);
					fread(&arg2,1,2,fileM);
					if(!GetItemName(arg1))
						sprintf(buf,"0x%X",arg1);
					else
						strcpy(buf,GetItemName(arg1));
					fprintf(fsend,"giveitemtopc %s %u\r\n",buf,arg2);
					break;
				case CMD_ADDFURNITURE:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"addfurniture 0x%X\r\n",arg1);
					break;
				case CMD_CHECKITEMTYPE:
					fread(&arg1,1,2,fileM);
					if(!GetItemName(arg1))
						sprintf(buf,"0x%X",arg1);
					else
						strcpy(buf,GetItemName(arg1));
					fprintf(fsend,"checkitemtype %s\r\n",buf);
					break;
				case CMD_CHECKIFROOMFORFURNITURE:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"checkifroomforfurniture 0x%X\r\n",arg1);
					break;
				case CMD_PLAYSONG:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"playsong 0x%X\r\n",arg1);
					break;
				case CMD_FARREAPPEAR:
					fread(&arg1,1,2,fileM);
					fread(&arg2,1,1,fileM);
					fread(&arg3,1,1,fileM);
					fprintf(fsend,"farreappear 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
					break;
				case CMD_FARDISAPPEAR:
					fread(&arg1,1,2,fileM);
					fread(&arg2,1,1,fileM);
					fread(&arg3,1,1,fileM);
					fprintf(fsend,"fardisappear 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
					break;
				case CMD_STOREPARTYPOKEMON:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,2,fileM);
					fprintf(fsend,"storepartypokemon 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case CMD_SETPOKEMONPP:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fread(&arg3,1,2,fileM);
					fprintf(fsend,"setpokemonpp 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
					break;
				case CMD_MULTICHOICE3:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fread(&arg3,1,1,fileM);
					fread(&arg4,1,1,fileM);
					fread(&arg5,1,1,fileM);
					fprintf(fsend,"multichoice3 0x%X 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4,arg5);
					break;
				case CMD_SHOWYESNO:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fprintf(fsend,"showyesno 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case CMD_SETTRAINERFLAG:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"settrainerflag 0x%X\r\n",arg1);
					break;
				case CMD_STOREFIRSTPOKEMON:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"storefirstpokemon 0x%X\r\n",arg1);
					break;
				case CMD_POKEMART2:
					fread(&arg1,1,4,fileM);
					if(dyndec&&((arg1&0xF8000000)==0x08000000))
					{
						if(!Defined2(arg1|0x08000000))
						{
							martnum++;
							sprintf(buf,"@mart%u",martnum);
							Define2(arg1|0x08000000,buf);
						}
						fprintf(fsend,"pokemart2 %s\r\n",WhatIs2(arg1|0x08000000));
						DoMart(arg1);
					}
					else
					{
						if((arg1&0xF8000000)==0x08000000)
						{
							fprintf(fsend,"pokemart2 0x%X\r\n",arg1);
							DoMart(arg1);
						}
						else
							fprintf(fsend,"pokemart2 0x%X 'Not in ROM area\r\n",arg1);
					}
					break;
				case CMD_8A:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fread(&arg3,1,1,fileM);
					fprintf(fsend,"CMD_8A 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
					break;
				case CMD_FADESCREENDELAY:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					switch (arg1)
					{
					case 0:
						strcpy(buf,"FADEIN_BLACK");
						break;
					case 1:
						strcpy(buf,"FADEOUT_BLACK");
						break;
					case 2:
						strcpy(buf,"FADEIN_WHITE");
						break;
					case 3:
						strcpy(buf,"FADEOUT_WHITE");
						break;
					default:
						sprintf(buf,"0x%X",arg1);
					}
					fprintf(fsend,"fadescreendelay %s 0x%X\r\n",buf,arg2);
					break;
				case CMD_SETCATCHLOCATION:
					fread(&arg1,1,2,fileM);
					fread(&arg2,1,1,fileM);
					fprintf(fsend,"setcatchlocation 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case CMD_STOREITEMS:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,2,fileM);
					fread(&arg3,1,2,fileM);
					if(!GetItemName(arg2))
						sprintf(buf,"0x%X",arg2);
					else
						strcpy(buf,GetItemName(arg2));
					fprintf(fsend,"storeitems 0x%X %s %u\r\n",arg1,buf,arg3);
					break;
				case CMD_WARPTELEPORT2:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fread(&arg3,1,1,fileM);
					if (arg3==0xFF)
					{
						fread(&arg4,1,2,fileM);
						fread(&arg5,1,2,fileM);
						fprintf(fsend,"warpteleport2 0x%X 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4,arg5);
					} else
					{
						fprintf(fsend,"warpteleport2 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
					}
					break;
					break;
				case CMD_WARPMUTED:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fread(&arg3,1,1,fileM);
					if (arg3==0xFF)
					{
						fread(&arg4,1,2,fileM);
						fread(&arg5,1,2,fileM);
						fprintf(fsend,"warpmuted 0x%X 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4,arg5);
					} else
					{
						fprintf(fsend,"warpmuted 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
					}
					break;
				case CMD_WARPWALKING:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fread(&arg3,1,1,fileM);
					if (arg3==0xFF)
					{
						fread(&arg4,1,2,fileM);
						fread(&arg5,1,2,fileM);
						fprintf(fsend,"warpwalking 0x%X 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4,arg5);
					} else
					{
						fprintf(fsend,"warpwalking 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
					}
					break;
				case CMD_FALLDOWNHOLE:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fprintf(fsend,"falldownhole 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case CMD_WARPTELEPORT:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fread(&arg3,1,1,fileM);
					if (arg3==0xFF)
					{
						fread(&arg4,1,2,fileM);
						fread(&arg5,1,2,fileM);
						fprintf(fsend,"warpteleport 0x%X 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4,arg5);
					} else
					{
						fprintf(fsend,"warpteleport 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
					}
					break;
				case CMD_WARP3:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fread(&arg3,1,1,fileM);
					if (arg3==0xFF)
					{
						fread(&arg4,1,2,fileM);
						fread(&arg5,1,2,fileM);
						fprintf(fsend,"warp3 0x%X 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4,arg5);
					} else
					{
						fprintf(fsend,"warp3 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
					}
					break;
				case CMD_WARPELEVATOR:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fread(&arg3,1,1,fileM);
					if (arg3==0xFF)
					{
						fread(&arg4,1,2,fileM);
						fread(&arg5,1,2,fileM);
						fprintf(fsend,"warpelevator 0x%X 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4,arg5);
					} else
					{
						fprintf(fsend,"warpelevator 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
					}
					break;
				case CMD_WARP4:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fread(&arg3,1,1,fileM);
					if (arg3==0xFF)
					{
						fread(&arg4,1,2,fileM);
						fread(&arg5,1,2,fileM);
						fprintf(fsend,"warp4 0x%X 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4,arg5);
					} else
					{
						fprintf(fsend,"warp4 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
					}
					break;
				case CMD_WARP5:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fread(&arg3,1,1,fileM);
					if (arg3==0xFF)
					{
						fread(&arg4,1,2,fileM);
						fread(&arg5,1,2,fileM);
						fprintf(fsend,"warp5 0x%X 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4,arg5);
					} else
					{
						fprintf(fsend,"warp5 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
					}
					break;
				case CMD_GETPLAYERXY:
					fread(&arg1,1,2,fileM);
					fread(&arg2,1,2,fileM);
					fprintf(fsend,"getplayerxy 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case CMD_FADESONG:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"fadesong 0x%X\r\n",arg1);
					break;
				case CMD_EXECUTERAM:
					GENERIC("executeram");
					break;
				case CMD_LASTBATTLE:
					GENERIC("lastbattle");
					break;
				case CMD_RESETWEATHER:
					GENERIC("resetweather");
					break;
				case CMD_STARTWIRELESS:
					GENERIC("startwireless");
					break;
				case CMD_CHOOSECONTESTPOKEMON:
					GENERIC("choosecontestpokemon");
					break;
				case CMD_STARTCONTEST:
					GENERIC("startcontest");
					break;
				case CMD_LASTTRAINERBATTLE:
					GENERIC("lasttrainerbattle");
					break;
				case CMD_ENDTRAINERBATTLE:
					GENERIC("endtrainerbattle");
					break;
				case CMD_ENDTRAINERBATTLE2:
					GENERIC("endtrainerbattle2");
					break;
				case CMD_TAKEFROMCOINCASE:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"takefromcoincase 0x%X\r\n",arg1);
					break;
				case CMD_MSGBOX2:
					fread(&arg2,1,4,fileM);
					if ((arg2&0xF8000000)!=0x08000000)
					{
						fprintf(fsend,"msgbox2 0x%X\r\n",arg2);
					}
					else
					{
						transtxt(arg2,filename);
						if (trans[20]!=0)
						{
							strcpy(trans+20,"...");
						}
						if(dyndec)
						{
							if(!Defined2(arg2|0x08000000))
							{
								textnum++;
								sprintf(buf,"@text%u",textnum);
								Define2(arg2|0x08000000,buf);
							}
							fprintf(fsend,"msgbox2 %s ' %s\r\n",WhatIs2(arg2|0x08000000),trans);
						}
						else
							fprintf(fsend,"msgbox2 0x%X ' %s\r\n",arg2,trans);
						DoText(arg2);
					}
					if ((arg2&0xF8000000)!=0x08000000)
					{
						fprintf(fsend,"          'Pointer not in ROM area\r\n");
					}
					break;
				case CMD_B2:
					GENERIC("CMD_B2");
					break;
				case CMD_2D:
					GENERIC("CMD_2D");
					break;
				case CMD_2C:
					fread(&arg1,1,2,fileM);
					fread(&arg2,1,2,fileM);
					fprintf(fsend,"CMD_2C 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case CMD_RESETVARS:
					GENERIC("resetvars");
					break;
				case CMD_B1:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fread(&arg3,1,2,fileM);
					fread(&arg4,1,2,fileM);
					fprintf(fsend,"CMD_B1 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4);
					break;
				case CMD_SETDOOROPENEDSTATIC:
					fread(&arg1,1,2,fileM);
					fread(&arg2,1,2,fileM);
					fprintf(fsend,"setdooropenedstatic 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case CMD_SETDOORCLOSEDSTATIC:
					fread(&arg1,1,2,fileM);
					fread(&arg2,1,2,fileM);
					fprintf(fsend,"setdoorclosedstatic 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case CMD_RESETSPRITELEVEL:
					fread(&arg1,1,2,fileM);
					fread(&arg2,1,2,fileM);
					fread(&arg3,1,2,fileM);
					fprintf(fsend,"resetspritelevel 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
					break;
				case CMD_CREATETEMPSPRITE:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fread(&arg3,1,2,fileM);
					fread(&arg4,1,2,fileM);
					fread(&arg5,1,1,fileM);
					fread(&arg6,1,1,fileM);
					fprintf(fsend,"createtempsprite 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4,arg5,arg6);
					break;
				case CMD_A6:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"CMD_A6 0x%X\r\n",arg1);
					break;
				case CMD_TEMPSPRITEFACE:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fprintf(fsend,"tempspriteface 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case CMD_SETOBEDIENCE:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"setobedience 0x%X\r\n",arg1);
					break;
				case CMD_CHECKOBEDIENCE:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"checkobedience 0x%X\r\n",arg1);
					break;
				case CMD_COMPAREHIDDENVAR:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,2,fileM);
					fprintf(fsend,"comparehiddenvar 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case CMD_STOREBOXNAME:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,2,fileM);
					fprintf(fsend,"storeboxname 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case CMD_WARP6:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fread(&arg3,1,1,fileM);
					fread(&arg4,1,2,fileM);
					fread(&arg5,1,2,fileM);
					fprintf(fsend,"warp6 0x%X 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4,arg5);
					break;
				case CMD_MSGBOXSIGN:
					GENERIC("msgboxsign");
					break;
				case CMD_MSGBOXNORMAL:
					GENERIC("msgboxnormal");
					break;
				case CMD_SETHEALINGPLACE:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"sethealingplace 0x%X\r\n",arg1);
					break;
				case CMD_SPRITEFACE:
					fread(&arg1,1,2,fileM);
					fread(&arg2,1,1,fileM);
					switch(arg1)
					{
						case 0x00FF:strcpy(buf,"PLAYER");break;
						case 0x800D:strcpy(buf,"LASTRESULT");break;
						case 0x800F:strcpy(buf,"LASTTALKED");break;
						default:sprintf(buf,"0x%X",arg1);
					}
					switch(arg2)
					{
						case 1:strcpy(buf2,"DOWN");break;
						case 2:strcpy(buf2,"UP");break;
						case 3:strcpy(buf2,"LEFT");break;
						case 4:strcpy(buf2,"RIGHT");break;
						default:sprintf(buf2,"0x%X",arg2);
					}
					fprintf(fsend,"spriteface %s %s\r\n",buf,buf2);
					break;
				case CMD_RANDOM:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"random 0x%X\r\n",arg1);
					break;
				case CMD_MOVEOFFSCREEN:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"moveoffscreen 0x%X\r\n",arg1);
					break;
				case CMD_CHECKTRAINERFLAG:
					fread(&arg1,1,2,fileM);
					comparetype=1;
					fprintf(fsend,"checktrainerflag 0x%X\r\n",arg1);
					break;
				case CMD_SETDOOROPENED:
					fread(&arg1,1,2,fileM);
					fread(&arg2,1,2,fileM);
					fprintf(fsend,"setdooropened 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case CMD_SETDOORCLOSED:
					fread(&arg1,1,2,fileM);
					fread(&arg2,1,2,fileM);
					fprintf(fsend,"setdoorclosed 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case CMD_DOORCHANGE:
					GENERIC("doorchange");
					break;
				case CMD_CALLSTDIF:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					strcpy(buf,"");
					if(comparetype)
					switch(arg1)
					{
						case 0:strcpy(buf,"Flag is unset");strcpy(buf2,"false");break;
						case 1:strcpy(buf,"Flag is set");strcpy(buf2,"true");break;
						default:strcpy(buf,"Unknown Checkflag/IF condition");sprintf(buf2,"0x%X",arg1);
					}
					else
					switch(arg1)
					{
					case 0:
						strcpy(buf,"Smaller Than");
						strcpy(buf2,"<");
						break;
					case 1:
						strcpy(buf,"Equal To");
						strcpy(buf2,"==");
						break;
					case 2:
						strcpy(buf,"Larger Than");
						strcpy(buf2,">");
						break;
					case 3:
						strcpy(buf,"Smaller Than or Equal To");
						strcpy(buf2,"<=");
						break;
					case 4:
						strcpy(buf,"Larger Than or Equal To");
						strcpy(buf2,">=");
						break;
					case 5:
						strcpy(buf,"Not Equal To");
						strcpy(buf2,"!=");
						break;
					default:
						strcpy(buf,"Unknown Compare/IF condition");
						sprintf(buf2,"0x%X",arg1);
					}
					fprintf(fsend,"if %s callstd 0x%X ' %s\r\n",buf2,arg2,buf);
					break;
				case CMD_SHOWPOKEPIC:
					fread(&arg1,1,2,fileM);
					fread(&arg2,1,1,fileM);
					fread(&arg3,1,1,fileM);
					fprintf(fsend,"showpokepic 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
					break;
				case CMD_HIDEPOKEPIC:
					fprintf(fsend,"hidepokepic\r\n");
					break;
				case CMD_SETMAPFOOTER:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"setmapfooter 0x%X\r\n",arg1);
					break;
				case CMD_JUMPSTDIF:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					strcpy(buf,"");
					if(comparetype)
					switch(arg1)
					{
						case 0:strcpy(buf,"Flag is unset");strcpy(buf2,"false");break;
						case 1:strcpy(buf,"Flag is set");strcpy(buf2,"true");break;
						default:strcpy(buf,"Unknown Checkflag/IF condition");sprintf(buf2,"0x%X",arg1);
					}
					else
					switch(arg1)
					{
					case 0:
						strcpy(buf,"Smaller Than");
						strcpy(buf2,"<");
						break;
					case 1:
						strcpy(buf,"Equal To");
						strcpy(buf2,"==");
						break;
					case 2:
						strcpy(buf,"Larger Than");
						strcpy(buf2,">");
						break;
					case 3:
						strcpy(buf,"Smaller Than or Equal To");
						strcpy(buf2,"<=");
						break;
					case 4:
						strcpy(buf,"Larger Than or Equal To");
						strcpy(buf2,">=");
						break;
					case 5:
						strcpy(buf,"Not Equal To");
						strcpy(buf2,"!=");
						break;
					default:
						strcpy(buf,"Unknown Compare/IF condition");
						sprintf(buf2,"0x%X",arg1);
					}
					fprintf(fsend,"if %s jumpstd 0x%X ' %s\r\n",buf2,arg2,buf);
					break;
				case CMD_SETWORLDMAPFLAG:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"setworldmapflag 0x%X\r\n",arg1);
					break;
				case CMD_LIGHTROOM:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"lightroom 0x%X\r\n",arg1);
					break;
				case CMD_DARKENROOM:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"darkenroom 0x%X\r\n",arg1);
					break;
				case CMD_MOVESPRITE2:
					fread(&arg1,1,2,fileM);
					fread(&arg2,1,2,fileM);
					fread(&arg3,1,2,fileM);
					fprintf(fsend,"movesprite2 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
					break;
				case CMD_ADDITEM:
					fread(&arg1,1,2,fileM);
					fread(&arg2,1,2,fileM);
					if(!GetItemName(arg1))
						sprintf(buf,"0x%X",arg1);
					else
						strcpy(buf,GetItemName(arg1));
					fprintf(fsend,"additem %s %u\r\n",buf,arg2);
					break;
				case CMD_SPRITEBEHAVE:
					fread(&arg1,1,2,fileM);
					fread(&arg2,1,1,fileM);
					fprintf(fsend,"spritebehave 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case CMD_UPDATECOINS:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fprintf(fsend,"updatecoins 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case CMD_SETBYTE2:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fprintf(fsend,"setbyte2 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case CMD_SETFARBYTE:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,4,fileM);
					fprintf(fsend,"setfarbyte 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case CMD_TEXTCOLOR:
					fread(&arg1,1,1,fileM);
					switch(arg1)
					{
						case 0:strcpy(buf,"BLUE");break;
						case 1:strcpy(buf,"RED");break;
						case 3:strcpy(buf,"GRAY");break;
						default:sprintf(buf,"0x%X",arg1);
					}
					fprintf(fsend,"textcolor %s\r\n",buf);
					break;
				case CMD_FAKEJUMPSTD:
					if (mode==FIRE_RED)
					{
						fread(&arg1,1,1,fileM);
						fprintf(fsend,"fakejumpstd 0x%X\r\n",arg1);
					}
					else
					{
						fread(&arg1,1,4,fileM);
						if(dyndec&&((arg1&0xF8000000)==0x08000000))
						{
							if(!Defined2(arg1|0x08000000))
							{
								martnum++;
								sprintf(buf,"@mart%u",martnum);
								Define2(arg1|0x08000000,buf);
							}
							fprintf(fsend,"pokemart3 %s\r\n",WhatIs2(arg1|0x08000000));
							DoMart(arg1);
						}
						else
						{
							if((arg1&0xF8000000)==0x08000000)
							{
								fprintf(fsend,"pokemart3 0x%X\r\n",arg1);
								DoMart(arg1);
							}
							else
								fprintf(fsend,"pokemart3 0x%X 'Not in ROM area\r\n",arg1);
						}
					}
					break;
				case CMD_COMPAREVARS:
					fread(&arg1,1,2,fileM);
					fread(&arg2,1,2,fileM);
					comparetype=0;
					sprintf(buf,"0x%X",arg1);
					sprintf(buf2,"0x%X",arg2);
					if (arg1==0x800D) {
						strcpy(buf,"LASTRESULT");
					}
					else if (arg1==0x800C) {
						strcpy(buf,"PLAYERFACING");
					}
					else if (arg1==0x800F) {
						strcpy(buf,"LASTTALKED");
					}
					if (arg2==0x800D) {
						strcpy(buf2,"LASTRESULT");
					}
					else if (arg2==0x800C) {
						strcpy(buf,"PLAYERFACING");
					}
					else if (arg2==0x800F) {
						strcpy(buf,"LASTTALKED");
					}
					fprintf(fsend,"comparevars %s %s\r\n",buf,buf2);
					break;
				case CMD_COMPAREVARS2:
					fread(&arg1,1,2,fileM);
					fread(&arg2,1,2,fileM);
					comparetype=0;
					sprintf(buf,"0x%X",arg1);
					sprintf(buf2,"0x%X",arg2);
					if (arg1==0x800D) {
						strcpy(buf,"LASTRESULT");
					}
					else if (arg1==0x800C) {
						strcpy(buf,"PLAYERFACING");
					}
					else if (arg1==0x800F) {
						strcpy(buf,"LASTTALKED");
					}
					if (arg2==0x800D) {
						strcpy(buf2,"LASTRESULT");
					}
					else if (arg2==0x800C) {
						strcpy(buf,"PLAYERFACING");
					}
					else if (arg2==0x800F) {
						strcpy(buf,"LASTTALKED");
					}
					fprintf(fsend,"comparevars2 %s %s\r\n",buf,buf2);
					break;
				case CMD_SUBTRACTVAR:
					fread(&arg1,1,2,fileM);
					fread(&arg2,1,2,fileM);
					sprintf(buf,"0x%X",arg1);
					if (arg1==0x800D) {
						strcpy(buf,"LASTRESULT");
					}
					else if (arg1==0x800C) {
						strcpy(buf,"PLAYERFACING");
					}
					else if (arg1==0x800F) {
						strcpy(buf,"LASTTALKED");
					}
					fprintf(fsend,"subtractvar %s 0x%X\r\n",buf,arg2);
					break;
				case CMD_CLEARTRAINERFLAG:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"cleartrainerflag 0x%X\r\n",arg1);
					break;
				case CMD_FE:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"CMD_FE 0x%X\r\n",arg1);
					break;
				case CMD_ADDVAR:
					fread(&arg1,1,2,fileM);
					fread(&arg2,1,2,fileM);
					sprintf(buf,"0x%X",arg1);
					sprintf(buf2,"0x%X",arg2);
					if (arg1==0x800D) {
						strcpy(buf,"LASTRESULT");
					}
					else if (arg1==0x800C) {
						strcpy(buf,"PLAYERFACING");
					}
					else if (arg1==0x800F) {
						strcpy(buf,"LASTTALKED");
					}
					if (arg2==0x800D) {
						strcpy(buf2,"LASTRESULT");
					}
					else if (arg2==0x800C) {
						strcpy(buf,"PLAYERFACING");
					}
					else if (arg2==0x800F) {
						strcpy(buf,"LASTTALKED");
					}
					fprintf(fsend,"addvar %s %s\r\n",buf,buf2);
					break;
				case CMD_96:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"CMD_96 0x%X\r\n",arg1);
					break;
				case CMD_COINCASETOVAR:
					fread(&arg1,1,2,fileM);
					sprintf(buf,"0x%X",arg1);
					if (arg1==0x800D) {
						strcpy(buf,"LASTRESULT");
					}
					else if (arg1==0x800C) {
						strcpy(buf,"PLAYERFACING");
					}
					else if (arg1==0x800F) {
						strcpy(buf,"LASTTALKED");
					}
					fprintf(fsend,"coincasetovar %s\r\n",buf);
					break;
				case CMD_GIVETOCOINCASE:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"givetocoincase %d ' Bear in mind, it's not in hex\r\n",arg1);
					break;
				case CMD_CHECKMONEY:
					fread(&arg1,1,2,fileM);
					fread(&arg2,1,1,fileM);
					fread(&arg3,1,2,fileM);
					fprintf(fsend,"checkmoney 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
					break;
				case CMD_PAYMONEY:
					fread(&arg1,1,2,fileM);
					fread(&arg2,1,1,fileM);
					fread(&arg3,1,2,fileM);
					fprintf(fsend,"paymoney 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
					break;
				case CMD_UPDATEMONEY:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fread(&arg3,1,1,fileM);
					fprintf(fsend,"updatemoney 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
					break;
				case CMD_HIDEMONEY:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fprintf(fsend,"hidemoney 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case CMD_HIDECOINS:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fprintf(fsend,"hidecoins 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case CMD_SHOWCOINS:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fprintf(fsend,"showcoins 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case CMD_MULTICHOICE2:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fread(&arg3,1,1,fileM);
					fread(&arg4,1,2,fileM);
					fprintf(fsend,"multichoice2 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4);
					break;
				case CMD_SHOWMONEY:
					fread(&arg1,1,2,fileM);
					fread(&arg2,1,1,fileM);
					fprintf(fsend,"showmoney 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case CMD_DISAPPEAR:
					fread(&arg1,1,2,fileM);
					if(arg1==0x800F)
						strcpy(buf,"LASTTALKED");
					else
						sprintf(buf,"0x%X",arg1);
					fprintf(fsend,"disappear %s\r\n",buf);
					break;
				case CMD_REAPPEAR:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"reappear 0x%X\r\n",arg1);
					break;
				case CMD_GIVEEGG:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"giveegg 0x%X\r\n",arg1);
					break;
				case CMD_STOREFURNITURE:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,2,fileM);
					fprintf(fsend,"storefurniture 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case CMD_STOREATTACK:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,2,fileM);
					fprintf(fsend,"storeattack 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case CMD_STORECOMP:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,2,fileM);
					fprintf(fsend,"storecomp 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case CMD_SLOTMACHINE:
					if (mode==FIRE_RED)
					{
						fread(&arg1,1,1,fileM);
						fprintf(fsend,"fakecallstd 0x%X\r\n",arg1);
					}
					else if (mode==RUBY)
					{
						fread(&arg1,1,2,fileM);
						fprintf(fsend,"slotmachine 0x%X\r\n",arg1);
					}
					break;
				case CMD_STOREITEM:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,2,fileM);
					if(!GetItemName(arg2))
						sprintf(buf,"0x%X",arg2);
					else
						strcpy(buf,GetItemName(arg2));
					fprintf(fsend,"storeitem 0x%X %s\r\n",arg1,buf);
					break;
				case CMD_DOANIMATION:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"doanimation 0x%X\r\n",arg1);
					break;
				case CMD_CHECKANIMATION:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"checkanimation 0x%X\r\n",arg1);
					break;
				case CMD_SETANIMATION:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,2,fileM);
					fprintf(fsend,"setanimation 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case CMD_HIDEBOX:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fread(&arg3,1,1,fileM);
					fread(&arg4,1,1,fileM);
					fprintf(fsend,"hidebox 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4);
					break;
				case CMD_STORETEXT:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,4,fileM);
					if ((arg2&0xF8000000)!=0x08000000)
					{
						fprintf(fsend,"storetext 0x%X 0x%X 'Text not in ROM area\r\n",arg1,arg2);
					}
					else
					{
						transtxt(arg2,filename);
						if (trans[20]!=0)
						{
							strcpy(trans+20,"...");
						}
						if(dyndec)
						{
							if(!Defined2(arg2|0x08000000))
							{
								textnum++;
								sprintf(buf,"@text%u",textnum);
								Define2(arg2|0x08000000,buf);
							}
							fprintf(fsend,"storetext %s 0x%X ' %s\r\n",arg1,WhatIs2(arg2|0x08000000),trans);
						}
						else
						{
							fprintf(fsend,"storetext 0x%X 0x%X ' %s\r\n",arg1,arg2,trans);
						}
						DoText(arg2);
					}
					break;
				case CMD_SOUND:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"sound 0x%X\r\n",arg1);
					break;
				case CMD_STOREVAR:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,2,fileM);
					sprintf(buf,"0x%X",arg2);
					if (arg2==0x800D) {
						strcpy(buf,"LASTRESULT");
					}
					else if (arg2==0x800C) {
						strcpy(buf,"PLAYERFACING");
					}
					else if (arg2==0x800F) {
						strcpy(buf,"LASTTALKED");
					}
					fprintf(fsend,"storevar 0x%X %s\r\n",arg1,buf);
					break;
				case CMD_DOWEATHER:
					GENERIC("doweather");
					break;
				case CMD_SETWEATHER:
					arg2=1;
					if (mode==FIRE_RED)arg2=2;
					fread(&arg1,1,arg2,fileM);
					fprintf(fsend,"setweather 0x%X\r\n",arg1);
					break;
				case CMD_BATTLE:
					fread(&arg1,1,2,fileM);
					fread(&arg2,1,1,fileM);
					fread(&arg3,1,2,fileM);
					if(GetPokeName(arg1))
						strcpy(buf,GetPokeName(arg1));
					else
						sprintf(buf,"0x%X",arg1);
					if(GetItemName(arg3))
						strcpy(buf2,GetItemName(arg3));
					else
						sprintf(buf2,"0x%X",arg3);
					//poke lvl item
					fprintf(fsend,"battle %s %u %s\r\n",buf,arg2,buf2);
					break;
				case CMD_SETMAPTILE:
					fread(&arg1,1,2,fileM);
					fread(&arg2,1,2,fileM);
					fread(&arg3,1,2,fileM);
					fread(&arg4,1,2,fileM);
					fprintf(fsend,"setmaptile 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4);
					break;
				case CMD_END:
					still_going=0;
					GENERIC("end");
					break;
				case CMD_POKEMART:
					fread(&arg1,1,4,fileM);
					if((arg1&0xF8000000)!=0x08000000)
					{
						fprintf(fsend,"pokemart 0x%X 'Not in ROM area\r\n",arg1);
						break;
					}
					if(dyndec)
					{
						if(!Defined2(arg1|0x08000000))
						{
							martnum++;
							sprintf(buf,"@mart%u",martnum);
							Define2(arg1|0x08000000,buf);
						}
						fprintf(fsend,"pokemart 0x%X\r\n",WhatIs2(arg1|0x08000000));
					}
					else
						fprintf(fsend,"pokemart 0x%X\r\n",arg1);
					DoMart(arg1);
					break;
				case CMD_SHOWMSG:
					GENERIC("showmsg");
					break;
				case CMD_RETURN:
					still_going=0;
					GENERIC("return");
					break;
				case CMD_WRITEBYTETOOFFSET:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,4,fileM);
					fprintf(fsend,"writebytetooffset 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case CMD_CHECKFLAG:
					fread(&arg1,1,2,fileM);
					comparetype=1;
					if(GetFlagName(arg1))
						strcpy(buf,GetFlagName(arg1));
					else
						sprintf(buf,"0x%X",arg1);
					fprintf(fsend,"checkflag %s\r\n",buf);
					break;
				case CMD_SPECIAL2:
					fread(&arg1,1,2,fileM);
					fread(&arg2,1,2,fileM);
					switch(arg2)
					{
						case 0x39:strcpy(buf2,"WANT_REMATCH");break;
						case 0x84:strcpy(buf2,"COUNT_POKEMON");break;
						default:sprintf(buf2,"0x%X",arg2);
					}
					switch(arg1)
					{
						case 0x800D:strcpy(buf,"LASTRESULT");break;
						default:sprintf(buf,"0x%X",arg1);
					}
					lastdata=arg2;
					lastdata2=arg1;
					fprintf(fsend,"special2 %s %s\r\n",buf,buf2);
					break;
				case CMD_SETFLAG:
					fread(&arg1,1,2,fileM);
					if(GetFlagName(arg1))
						strcpy(buf,GetFlagName(arg1));
					else
						sprintf(buf,"0x%X",arg1);
					fprintf(fsend,"setflag %s\r\n",buf);
					break;
				case CMD_LOCK:
					GENERIC("lock");
					break;
				case CMD_FACEPLAYER:
					GENERIC("faceplayer");
					break;
				case CMD_C3:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"CMD_C3 0x%X\r\n",arg1);
					break;
				case CMD_JUMPIF:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,4,fileM);
					strcpy(buf,"");
					if(comparetype)
					switch(arg1)
					{
						case 0:strcpy(buf,"Flag is unset");strcpy(buf2,"false");break;
						case 1:strcpy(buf,"Flag is set");strcpy(buf2,"true");break;
						default:strcpy(buf,"Unknown Checkflag/IF condition");sprintf(buf2,"0x%X",arg1);
					}
					else
					switch(arg1)
					{
					case 0:
						strcpy(buf,"Smaller Than");
						strcpy(buf2,"<");
						break;
					case 1:
						strcpy(buf,"Equal To");
						strcpy(buf2,"==");
						break;
					case 2:
						strcpy(buf,"Larger Than");
						strcpy(buf2,">");
						break;
					case 3:
						strcpy(buf,"Smaller Than or Equal To");
						strcpy(buf2,"<=");
						break;
					case 4:
						strcpy(buf,"Larger Than or Equal To");
						strcpy(buf2,">=");
						break;
					case 5:
						strcpy(buf,"Not Equal To");
						strcpy(buf2,"!=");
						break;
					default:
						strcpy(buf,"Unknown Compare/IF condition");
						sprintf(buf2,"0x%X",arg1);
					}
					if ((arg2&0xF8000000)==0x08000000)
					{
						if(dyndec)
						{
							if(!Defined2(arg2|0x08000000))
							{
								codenum++;
								sprintf(buf3,"@code%u",codenum);
								Define2(arg2|0x08000000,buf3);
							}
							fprintf(fsend,"if %s jump %s ' %s\r\n",buf2,WhatIs2(arg2|0x08000000),buf);
						}
						else
							fprintf(fsend,"if %s jump 0x%X ' %s\r\n",buf2,arg2,buf);
						Do(arg2);
					}
					else
						fprintf(fsend,"if %s jump 0x%X ' %s\r\n          'Jump not in ROM area\r\n",buf2,arg2,buf);
					break;
				case CMD_WARP:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fread(&arg3,1,1,fileM);
					if (arg3==0xFF)
					{
						fread(&arg4,1,2,fileM);
						fread(&arg5,1,2,fileM);
						fprintf(fsend,"warp 0x%X 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4,arg5);
					}
					else
					{
						fprintf(fsend,"warp 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
					}
					break;
				case CMD_FB:
					fread(&arg1,1,4,fileM);
					fprintf(fsend,"CMD_FB 0x%X\r\n",arg1);
					break;
				case CMD_COPYSCRIPTBANKS:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fprintf(fsend,"copyscriptbanks 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case CMD_COMPAREVARTOFARBYTE:
					fread(&arg1,1,2,fileM);
					fread(&arg2,1,4,fileM);
					comparetype=0;
					fprintf(fsend,"comparevartofarbyte 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case CMD_COMPAREFARBYTETOVAR:
					fread(&arg1,1,4,fileM);
					fread(&arg2,1,2,fileM);
					comparetype=0;
					fprintf(fsend,"comparefarbytetovar 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case CMD_COMPAREFARBYTETOBYTE:
					fread(&arg1,1,4,fileM);
					fread(&arg2,1,1,fileM);
					comparetype=0;
					fprintf(fsend,"comparefarbytetobyte 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case CMD_COPYBYTE:
					fread(&arg1,1,4,fileM);
					fread(&arg2,1,4,fileM);
					fprintf(fsend,"copybyte 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case CMD_SETVAR:
					fread(&arg1,1,2,fileM);
					fread(&arg2,1,2,fileM);
					sprintf(buf,"0x%X",arg1);
					if (arg1==0x800D) {
						strcpy(buf,"LASTRESULT");
					}
					else if (arg1==0x800C) {
						strcpy(buf,"PLAYERFACING");
					}
					else if (arg1==0x800F) {
						strcpy(buf,"LASTTALKED");
					}
					fprintf(fsend,"setvar %s 0x%X\r\n",buf,arg2);
					break;
				case CMD_MESSAGE:
					fread(&arg1,1,4,fileM);
					if ((arg1&0xF8000000)!=0x08000000)
					{
						fprintf(fsend,"message 0x%X 'Text not in ROM area\r\n",arg1);
					}
					else
					{
						transtxt(arg1,filename);
						if (trans[20]!=0)
						{
							strcpy(trans+20,"...");
						}
						if(dyndec)
						{
							if(!Defined2(arg1|0x08000000))
							{
								textnum++;
								sprintf(buf,"@text%u",textnum);
								Define2(arg1|0x08000000,buf);
							}
							fprintf(fsend,"message %s ' %s\r\n",WhatIs2(arg1|0x08000000),trans);
						}
						else
							fprintf(fsend,"message 0x%X ' %s\r\n",arg1,trans);
						DoText(arg1);
					}
					break;
				case CMD_CALLIF:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,4,fileM);
					strcpy(buf,"");
					if(comparetype)
					switch(arg1)
					{
						case 0:strcpy(buf,"Flag is unset");strcpy(buf2,"false");break;
						case 1:strcpy(buf,"Flag is set");strcpy(buf2,"true");break;
						default:strcpy(buf,"Unknown Checkflag/IF condition");sprintf(buf2,"0x%X",arg1);
					}
					else
					switch(arg1)
					{
					case 0:
						strcpy(buf,"Smaller Than");
						strcpy(buf2,"<");
						break;
					case 1:
						strcpy(buf,"Equal To");
						strcpy(buf2,"==");
						break;
					case 2:
						strcpy(buf,"Larger Than");
						strcpy(buf2,">");
						break;
					case 3:
						strcpy(buf,"Smaller Than or Equal To");
						strcpy(buf2,"<=");
						break;
					case 4:
						strcpy(buf,"Larger Than or Equal To");
						strcpy(buf2,">=");
						break;
					case 5:
						strcpy(buf,"Not Equal To");
						strcpy(buf2,"!=");
						break;
					default:
						strcpy(buf,"Unknown Compare/IF condition");
						sprintf(buf2,"0x%X",arg1);
					}
					if ((arg2&0xF8000000)==0x08000000)
					{
						if(dyndec)
						{
							if(!Defined2(arg2|0x08000000))
							{
								codenum++;
								sprintf(buf3,"@code%u",codenum);
								Define2(arg2|0x08000000,buf3);
							}
							fprintf(fsend,"if %s call %s ' %s\r\n",buf2,WhatIs2(arg2|0x08000000),buf);
						}
						else
							fprintf(fsend,"if %s call 0x%X ' %s\r\n",buf2,arg2,buf);
						Do(arg2);
					}
					else
						fprintf(fsend,"if %s call 0x%X ' %s\r\n          'Call not in ROM area\r\n",buf2,arg2,buf);
					break;
				case CMD_CHECKGENDER:
					GENERIC("checkgender");
					break;
				case CMD_COMPARE:
					fread(&arg1,1,2,fileM);
					fread(&arg2,1,2,fileM);
					comparetype=0;
					sprintf(buf,"0x%X",arg1);
					sprintf(buf2,"0x%X",arg2);
					if (arg1==0x800D) {
						strcpy(buf,"LASTRESULT");
						if(lastcmd==CMD_CHECKGENDER)
						{
							if(arg2==0)strcpy(buf2,"BOY");
							else if(arg2==1)strcpy(buf2,"GIRL");
						}
						else if(lastcmd==CMD_CALLSTD)
						{
							if(arg2==0)strcpy(buf2,"NO");
							else if(arg2==1)strcpy(buf2,"YES");
						}
					}
					else if (arg1==0x800C) {
						strcpy(buf,"PLAYERFACING");
						switch(arg2)
						{
							case 1:strcpy(buf2,"DOWN");break;
							case 2:strcpy(buf2,"UP");break;
							case 3:strcpy(buf2,"LEFT");break;
							case 4:strcpy(buf2,"RIGHT");break;
						}
					}
					else if (arg1==0x800F) {
						strcpy(buf,"LASTTALKED");
					}
					if((lastcmd==CMD_SPECIAL2&&arg1==lastdata2)||(lastcmd==CMD_SPECIAL&&arg1==0x800D))
					{
						if(lastdata==0x39)
						{
							switch(arg2)
							{
								case 0:strcpy(buf2,"FALSE");break;
								case 1:strcpy(buf2,"TRUE");break;
							}
						}
					}
					fprintf(fsend,"compare %s %s\r\n",buf,buf2);
					break;
				case CMD_APPLYMOVEMENT:
					fread(&arg1,1,2,fileM);
					fread(&arg2,1,4,fileM);
					switch(arg1)
					{
						case 0x007F:strcpy(buf,"CAMERA");break;
						case 0x00FF:strcpy(buf,"PLAYER");break;
						case 0x800D:strcpy(buf,"LASTRESULT");break;
						case 0x800F:strcpy(buf,"LASTTALKED");break;
						default:sprintf(buf,"0x%X",arg1);
					}
					if ((arg2&0xF8000000)==0x08000000)
					{
						transmove(arg2,filename);
						if (trans[20]!=0)
						{
							strcpy(trans+20,"...");
						}
						if(dyndec)
						{
							if(!Defined2(arg2|0x08000000))
							{
								movenum++;
								sprintf(buf2,"@move%u",movenum);
								Define2(arg2|0x08000000,buf2);
							}
							fprintf(fsend,"applymovement %s %s ' %s\r\n",buf,WhatIs2(arg2|0x08000000),trans);
						}
						else
							fprintf(fsend,"applymovement %s 0x%X ' %s\r\n",buf,arg2,trans);
						DoMove(arg2);
					}
					else
					{
						fprintf(fsend,"applymovement %s 0x%X ' Not in ROM area\r\n",buf,arg2);
					}
					break;
				case CMD_RELEASE:
					GENERIC("release");
					if(lastcmd==CMD_CALLSTD&&lastdata==4)fprintf(fsend,"     ' Release commands close any open messages\r\n");
					break;
				case CMD_PAUSE:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"pause 0x%X\r\n",arg1);
					break;
				case CMD_PAUSEEVENT:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"pauseevent 0x%X\r\n",arg1);
					break;
				case CMD_CALL:
					fread(&arg1,1,4,fileM);
					if ((arg1&0xF8000000)==0x08000000)
					{
						if(dyndec)
						{
							if(!Defined2(arg1|0x08000000))
							{
								codenum++;
								sprintf(buf,"@code%u",codenum);
								Define2(arg1|0x08000000,buf);
							}
							fprintf(fsend,"call %s\r\n",WhatIs2(arg1|0x08000000));
						}
						else
							fprintf(fsend,"call 0x%X\r\n",arg1);
						Do(arg1);
					}
					else
						fprintf(fsend,"call 0x%X\r\n          'Call not in ROM area\r\n",arg1);
					break;
				case CMD_CALLSTD:
					fread(&arg1,1,1,fileM);
					lastdata=arg1;
					switch (arg1)
					{
					case 0:
						strcpy(buf2,"MSG_OBTAIN");
						strcpy(buf,"Obtained the XXXXXX!");
						break;
					case 1:
						strcpy(buf2,"MSG_FIND");
						strcpy(buf,"PLAYER found one XXXXXX!");
						break;
					case 2:
						strcpy(buf2,"MSG_FACEPLAYER");
						strcpy(buf,"Builtin lock/faceplayer");
						break;
					case 3:
						strcpy(buf2,"MSG_SIGN");
						strcpy(buf,"Signpost-style message");
						break;
					case 4:
						strcpy(buf2,"MSG_NOCLOSE");
						strcpy(buf,"Non-closing message");
						break;
					case 5:
						strcpy(buf2,"MSG_YESNO");
						strcpy(buf,"Yes/No message");
						break;
					case 6:
						strcpy(buf2,"MSG_LOCK");
						strcpy(buf,"Built-in lock command");
						break;
					case 9:
						strcpy(buf2,"MSG_ITEM");
						strcpy(buf,"Shows a message, then... 'Player put the yyy in the zzz!'");
						break;
					case 0xA:
						strcpy(buf2,"MSG_POKENAV");
						strcpy(buf,"Pokenav-style message");
						break;
					default:
						strcpy(buf,"");
						sprintf(buf2,"0x%X",arg1);
					}
					fprintf(fsend,"callstd %s ' %s\r\n",buf2,buf);
					break;
				case CMD_MSGBOX:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,4,fileM);
					if (arg1==0)
					{
						if ((arg2&0xF8000000)!=0x08000000)
						{
							fprintf(fsend,"msgbox 0x%X\r\n       ' Not in ROM area\r\n",arg2);
						}
						else
						{
							transtxt(arg2,filename);
							if (trans[20]!=0)
							{
								strcpy(trans+20,"...");
							}
							if(dyndec)
							{
								if(!Defined2(arg2|0x08000000))
								{
									textnum++;
									sprintf(buf,"@text%u",textnum);
									Define2(arg2|0x08000000,buf);
								}
								fprintf(fsend,"msgbox %s ' %s\r\n",WhatIs2(arg2|0x08000000),trans);
							}
							else
								fprintf(fsend,"msgbox 0x%X ' %s\r\n",arg2,trans);
							DoText(arg2);
						}
					}
					else
					{
						fprintf(fsend,"loadpointer 0x%X 0x%X\r\n",arg1,arg2);
						if ((arg2&0xF8000000)!=0x08000000)
						{
							fprintf(fsend,"          'Pointer not in ROM area\r\n");
						}
					}
					break;
				case CMD_STOREPOKEMON:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,2,fileM);
					if(!GetPokeName(arg2))
						sprintf(buf,"0x%X",arg2);
					else
						strcpy(buf,GetPokeName(arg2));
					fprintf(fsend,"storepokemon 0x%X %s\r\n",arg1,buf);
					break;
				case CMD_SPECIAL:
					fread(&arg1,1,2,fileM);
					switch (arg1)
					{
						case 0x000:strcpy(buf,"HEAL_POKEMON");break;
						case 0x09C:strcpy(buf,"CHOOSE_POKEMON");break;
						case 0x09D:strcpy(buf,"CATCH_POKEMON");break;
						case 0x09E:strcpy(buf,"NAME_POKEMON");break;
						case 0x137:strcpy(buf,"BATTLE_WIPE");break;
						case 0x138:strcpy(buf,"BATTLE_PIXELLATE");break;
						case 0x139:strcpy(buf,"BATTLE_PIXELLATE2");break;
						case 0x143:strcpy(buf,"BATTLE_INTERLACE");break;
						case 0x156:strcpy(buf,"BATTLE_UNCATCHABLE");break;
						case 0x163:strcpy(buf,"SEEN_POKEMON");break;
						case 0x16F:strcpy(buf,"FR_NATIONAL_DEX");break;
						case 0x18D:strcpy(buf,"TEACH_ATTACK");break;
						case 0x1F3:strcpy(buf,"EM_NATIONAL_DEX");break;
						case 0x113:strcpy(buf,"CAMERA_START");break;
						case 0x114:strcpy(buf,"CAMERA_END");break;
						case 0x116:strcpy(buf,"EM_CAMERA_START");break;
						case 0x117:strcpy(buf,"EM_CAMERA_END");break;
						case 0x173:strcpy(buf,"FAME_CHECKER");break;
						case 0x174:strcpy(buf,"FAME_CHECKER_NEW");break;
						case 0x187:strcpy(buf,"INIT_STEPCOUNT");break;
						case 0x188:strcpy(buf,"GET_STEPCOUNT");break;
						case 0x197:strcpy(buf,"RESET_STEPCOUNT");break;
						default:
							sprintf(buf,"0x%X",arg1);
					}
					lastdata=arg1;
					fprintf(fsend,"special %s\r\n",buf);
					break;
				case CMD_FADESCREEN:
					fread(&arg1,1,1,fileM);
					switch (arg1)
					{
					case 0:
						strcpy(buf,"FADEIN_BLACK");
						break;
					case 1:
						strcpy(buf,"FADEOUT_BLACK");
						break;
					case 2:
						strcpy(buf,"FADEIN_WHITE");
						break;
					case 3:
						strcpy(buf,"FADEOUT_WHITE");
						break;
					default:
						sprintf(buf,"0x%X",arg1);
					}
					fprintf(fsend,"fadescreen %s\r\n",buf);
					break;
				case CMD_FANFARE:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"fanfare 0x%X\r\n",arg1);
					break;
				case CMD_WAITFANFARE:
					GENERIC("waitfanfare");
					break;
				case CMD_BRAILLE:
					fread(&arg1,1,4,fileM);
					fprintf(fsend,"braille 0x%X\r\n",arg1);
					if ((arg1&0xF8000000)==0x08000000)
					{
						transbrl(arg1,filename
#ifndef DLL
						,fsend
#endif
						);
					}
					else
					{
						fprintf(fsend,"          'Braille not in ROM area\r\n");
					}
					break;
				case CMD_D3:
					fread(&arg1,1,4,fileM);
					fprintf(fsend,"CMD_D3 0x%X\r\n",arg1);
					if ((arg1&0xF8000000)==0x08000000)
					{
						transbrl(arg1,filename
#ifndef DLL
						,fsend
#endif
						);
					}
					else
					{
						fprintf(fsend,"          'Braille not in ROM area\r\n");
					}
					break;
				case CMD_WAITBUTTON:
					GENERIC("waitbutton");
					break;
				case CMD_RELEASEALL:
					GENERIC("releaseall");
					if(lastcmd==CMD_CALLSTD&&lastdata==4)fprintf(fsend,"     ' Release commands close any open messages\r\n");
					break;
				case CMD_LOCKALL:
					GENERIC("lockall");
					break;
				case CMD_PICTURE:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"picture 0x%X\r\n",arg1);
					break;
				case CMD_NOP0:
					fprintf(fsend,"nop0 ' #raw 0x0\r\n");
					break;
				case CMD_NOP1:
					fprintf(fsend,"nop1 ' #raw 0x1\r\n");
					break;
				case CMD_JUMP:
					fread(&arg1,1,4,fileM);
					if ((arg1&0xF8000000)==0x08000000)
					{
						if(dyndec)
						{
							if(!Defined2(arg1|0x08000000))
							{
								codenum++;
								sprintf(buf,"@code%u",codenum);
								Define2(arg1|0x08000000,buf);
							}
							fprintf(fsend,"jump %s\r\n",WhatIs2(arg1|0x08000000));
						}
						else
							fprintf(fsend,"jump 0x%X\r\n",arg1);
						Do(arg1);
					}
					else
						fprintf(fsend,"jump 0x%X\r\n          'Jump not in ROM area\r\n",arg1);
					still_going=0;
					break;
				case CMD_JUMPSTD:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"jumpstd 0x%X\r\n",arg1);
					still_going=0;
					break;
				case CMD_KILLSCRIPT:
					GENERIC("killscript");
					break;
				case CMD_SETBYTE:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"setbyte 0x%X\r\n",arg1);
					break;
				case CMD_COPYVARIFNOTZERO:
					fread(&arg1,1,2,fileM);
					fread(&arg2,1,2,fileM);
					sprintf(buf,"0x%X",arg1);
					sprintf(buf2,"0x%X",arg2);
					if (arg1==0x800D) {
						strcpy(buf,"LASTRESULT");
					}
					else if (arg1==0x800C) {
						strcpy(buf,"PLAYERFACING");
					}
					else if (arg1==0x800F) {
						strcpy(buf,"LASTTALKED");
					}
					else if (arg1==0x8000) {
						if(GetItemName(arg2))
							strcpy(buf2,GetItemName(arg2));
					}
					fprintf(fsend,"copyvarifnotzero %s %s\r\n",buf,buf2);
					break;
				case CMD_COPYVAR:
					fread(&arg1,1,2,fileM);
					fread(&arg2,1,2,fileM);
					sprintf(buf,"0x%X",arg1);
					sprintf(buf2,"0x%X",arg2);
					if (arg1==0x800D) {
						strcpy(buf,"LASTRESULT");
					}
					else if (arg1==0x800C) {
						strcpy(buf,"PLAYERFACING");
					}
					else if (arg1==0x800F) {
						strcpy(buf,"LASTTALKED");
					}
					if (arg2==0x800D) {
						strcpy(buf2,"LASTRESULT");
					}
					else if (arg2==0x800C) {
						strcpy(buf,"PLAYERFACING");
					}
					else if (arg2==0x800F) {
						strcpy(buf,"LASTTALKED");
					}
					fprintf(fsend,"copyvar %s %s\r\n",buf,buf2);
					break;
				case CMD_COMPAREVARTOBYTE:
					fread(&arg1,1,2,fileM);
					fread(&arg2,1,1,fileM);
					comparetype=0;
					fprintf(fsend,"comparevartobyte 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case CMD_LOADBYTEFROMPOINTER:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,4,fileM);
					fprintf(fsend,"loadbytefrompointer 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case CMD_REMOVEITEM:
					fread(&arg1,1,2,fileM);
					fread(&arg2,1,2,fileM);
					if(!GetItemName(arg1))
						sprintf(buf,"0x%X",arg1);
					else
						strcpy(buf,GetItemName(arg1));
					fprintf(fsend,"removeitem %s %u\r\n",buf,arg2);
					break;
				case CMD_WAITSPECIAL:
					GENERIC("waitspecial");
					break;
				case CMD_WAITCRY:
					GENERIC("waitcry");
					break;
				case CMD_CRYFR:
					fread(&arg1,1,2,fileM);
					fread(&arg2,1,2,fileM);
					if(GetPokeName(arg1))
						strcpy(buf,GetPokeName(arg1));
					else sprintf(buf,"0x%X",arg1);
					fprintf(fsend,"cry %s 0x%X\r\n",buf,arg2);
					break;
				case CMD_CRY:
					if (mode!=FIRE_RED)
					{
						fread(&arg1,1,1,fileM);
						fread(&arg2,1,2,fileM);
						fprintf(fsend,"cry 0x%X 0x%X\r\n",arg1,arg2);
					}
					else
						GENERIC("checksound");
					break;
				case CMD_PLAYSOUND:
					fread(&arg1,1,2,fileM);
					if (mode==FIRE_RED)
					{
						fread(&arg2,1,1,fileM);
						fprintf(fsend,"playsound 0x%X 0x%X\r\n",arg1,arg2);
					}
					else
					{
						fprintf(fsend,"playsound 0x%X\r\n",arg1);
					}
					break;
				case CMD_FADEDEFAULT:
					if (mode==RUBY)
					{
						fread(&arg1,1,2,fileM);
						fprintf(fsend,"fadesound 0x%X\r\n",arg1);
					}
					else if (mode==FIRE_RED)
					{
						fprintf(fsend,"fadedefault\r\n");
					}
					break;
				case CMD_FADEOUT:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"fadeout 0x%X\r\n",arg1);
					break;
				case CMD_FADEIN:
					GENERIC("fadein");
					break;
				case CMD_COUNTPOKEMON:
					GENERIC("countpokemon");
					break;
				case CMD_CHECKITEM:
					fread(&arg1,1,2,fileM);
					fread(&arg2,1,2,fileM);
					if(!GetItemName(arg1))
						sprintf(buf,"0x%X",arg1);
					else
						strcpy(buf,GetItemName(arg1));
					fprintf(fsend,"checkitem %s %u\r\n",buf,arg2);
					break;
				case CMD_CHECKITEMSPACEINBAG:
					fread(&arg1,1,2,fileM);
					fread(&arg2,1,2,fileM);
					if(!GetItemName(arg1))
						sprintf(buf,"0x%X",arg1);
					else
						strcpy(buf,GetItemName(arg1));
					fprintf(fsend,"checkitemspaceinbag %s %u\r\n",buf,arg2);
					break;
				case CMD_CHECKATTACK:
					fread(&arg1,1,2,fileM);
					if(GetAttackName(arg1))
						strcpy(buf,GetAttackName(arg1));
					else
						sprintf(buf,"0x%X",arg1);
					fprintf(fsend,"checkattack %s\r\n",buf);
					break;
				case CMD_TRAINERBATTLE:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,2,fileM);
					fread(&arg3,1,2,fileM);
					fread(&arg4,1,4,fileM);
					if (arg1!=3)
					{
						fread(&arg5,1,4,fileM);
					}
					if (arg1==1||arg1==2) {
						fread(&arg6,1,4,fileM);
						if(dyndec)
						{
							if((arg4&0xF8000000)==0x08000000)
							{
								if(!Defined2(arg4|0x08000000))
								{
									textnum++;
									sprintf(buf,"@text%u",textnum);
									Define2(arg4|0x08000000,buf);
								}
							}
							if((arg5&0xF8000000)==0x08000000)
							{
								if(!Defined2(arg5|0x08000000))
								{
									textnum++;
									sprintf(buf,"@text%u",textnum);
									Define2(arg5|0x08000000,buf);
								}
							}
							if((arg6&0xF8000000)==0x08000000)
							{
								if(!Defined2(arg6|0x08000000))
								{
									codenum++;
									sprintf(buf,"@code%u",codenum);
									Define2(arg6|0x08000000,buf);
								}
							}
							if(!Defined2(arg4|0x08000000))
								sprintf(buf3,"0x%X",arg4);
							else
								strcpy(buf3,WhatIs2(arg4|0x08000000));
							strcat(buf3," ");
							if(!Defined2(arg5|0x08000000))
								sprintf(buf3+strlen(buf3),"0x%X",arg5);
							else
								strcat(buf3,WhatIs2(arg5|0x08000000));
							strcat(buf3," ");
							if(!Defined2(arg6|0x08000000))
								sprintf(buf3+strlen(buf3),"0x%X",arg6);
							else
								strcat(buf3,WhatIs2(arg6|0x08000000));
						}
						else
						{
							sprintf(buf3,"0x%X 0x%X 0x%X",arg4,arg5,arg6);
						}
						fprintf(fsend,"trainerbattle 0x%X 0x%X 0x%X %s\r\n",arg1,arg2,arg3,buf3);
						if ((arg6&0xF8000000)==0x08000000)
						{
							Do(arg6);
						}
					}
					else if (arg1==3)
					{
						if(dyndec&&(arg4&0xF8000000)==0x08000000)
						{
							if(!Defined2(arg4|0x08000000))
							{
								textnum++;
								sprintf(buf,"@text%u",textnum);
								Define2(arg4|0x08000000,buf);
							}
							fprintf(fsend,"trainerbattle 0x%X 0x%X 0x%X %s\r\n",arg1,arg2,arg3,WhatIs2(arg4|0x08000000));
						}
						else
							fprintf(fsend,"trainerbattle 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4);
					}
					else
					{
						if(dyndec)
						{
							if((arg4&0xF8000000)==0x08000000)
							{
								if(!Defined2(arg4|0x08000000))
								{
									textnum++;
									sprintf(buf,"@text%u",textnum);
									Define2(arg4|0x08000000,buf);
								}
							}
							if((arg5&0xF8000000)==0x08000000)
							{
								if(!Defined2(arg5|0x08000000))
								{
									textnum++;
									sprintf(buf,"@text%u",textnum);
									Define2(arg5|0x08000000,buf);
								}
							}
							if(!Defined2(arg4|0x08000000))
								sprintf(buf3,"0x%X",arg4);
							else
								strcpy(buf3,WhatIs2(arg4|0x08000000));
							strcat(buf3," ");
							if(!Defined2(arg5|0x08000000))
								sprintf(buf3+strlen(buf3),"0x%X",arg5);
							else
								strcat(buf3,WhatIs2(arg5|0x08000000));
						}
						else
							sprintf(buf3,"0x%X 0x%X",arg4,arg5);
						fprintf(fsend,"trainerbattle 0x%X 0x%X 0x%X %s\r\n",arg1,arg2,arg3,buf3);
					}
					if ((arg4&0xF8000000)==0x08000000)
					{
						DoText(arg4);
					}
					if ((arg5&0xF8000000)==0x08000000&&arg1!=3)
					{
						DoText(arg5);
					}
					break;
				case CMD_MULTICHOICE:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fread(&arg3,1,1,fileM);
					fread(&arg4,1,1,fileM);
					fprintf(fsend,"multichoice 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4);
					break;
				case CMD_ADDPOKEMON:
					fread(&arg1,1,2,fileM);
					fread(&arg2,1,1,fileM);
					fread(&arg3,1,2,fileM);
					fread(&arg4,1,1,fileM);
					fread(&arg5,1,4,fileM);
					fread(&arg6,1,4,fileM);
					if(!GetPokeName(arg1))
						sprintf(buf,"0x%X",arg1);
					else
						strcpy(buf,GetPokeName(arg1));
					if(!GetItemName(arg3))
						sprintf(buf2,"0x%X",arg3);
					else
						strcpy(buf2,GetItemName(arg3));
					fprintf(fsend,"addpokemon %s %u %s 0x%X 0x%X 0x%X\r\n",buf,arg2,buf2,arg4,arg5,arg6);
					break;
				case CMD_CALLASM:
					fread(&arg1,1,4,fileM);
					if(dyndec&&(arg1&1)&&((arg1&0xF8000000)==0x08000000))
					{
						if(!Defined2(arg1|0x08000000))
						{
							thumbnum++;
							sprintf(buf,"@thumb%u",thumbnum);
							Define2(arg1|0x08000000,buf);
						}
						fprintf(fsend,"callasm %s\r\n",WhatIs2(arg1|0x08000000));
					}
					else
						fprintf(fsend,"callasm 0x%X\r\n",arg1);
					if ((arg1&1)&&((arg1&0xF8000000)==0x08000000))
						DoThumb(arg1&0xFFFFFFFE);
					break;
				case CMD_CALLASM2:
					fread(&arg1,1,4,fileM);
					if(dyndec&&(arg1&1)&&((arg1&0xF8000000)==0x08000000))
					{
						if(!Defined2(arg1|0x08000000))
						{
							thumbnum++;
							sprintf(buf,"@thumb%u",thumbnum);
							Define2(arg1|0x08000000,buf);
						}
						fprintf(fsend,"callasm2 %s\r\n",WhatIs2(arg1|0x08000000));
					}
					else
						fprintf(fsend,"callasm2 0x%X\r\n",arg1);
					if ((arg1&1)&&((arg1&0xF8000000)==0x08000000))
						DoThumb(arg1&0xFFFFFFFE);
					break;
				case CMD_CLEARFLAG:
					fread(&arg1,1,2,fileM);
					if(GetFlagName(arg1))
						strcpy(buf,GetFlagName(arg1));
					else
						sprintf(buf,"0x%X",arg1);
					fprintf(fsend,"clearflag %s\r\n",buf);
					break;
				case CMD_COMPAREFARBYTETOFARBYTE:
					fread(&arg1,1,4,fileM);
					fread(&arg2,1,4,fileM);
					comparetype=0;
					fprintf(fsend,"comparefarbytetofarbyte 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case CMD_CLOSEMSG:
					GENERIC("closemsg");
					break;
				default:
					fprintf(fsend,"#raw 0x%X\r\n",command);
					if (command==0xFF)
					{
						failsafe++;
						if (failsafe>=100)
						{
							fprintf(fsend,"'Decompiler stopped due to failsafe mechanism (too many #RAW 0xFF commands)\r\n");
							still_going=0;
						}
					}
					break;
				}
			}
			else
			{
				puts("'--EOF--");
				still_going=0;
			}
		}
	}
	else if (mode!=CRYSTAL)
	{
		while (still_going)
		{
			read=(signed int)fread(&command,1,1,fileM);
			if (read>0)
			{
#define GENERIC(x) fprintf(fsend,"%s\r\n",x)
				arg1=arg2=arg3=arg4=arg5=arg6=arg7=0;
				switch (command)
				{
				case GLD_END:
					GENERIC("end");
					still_going=0;
					break;
				case GLD_RETURN:
					GENERIC("return");
					still_going=0;
					break;
				case GLD_RELOADANDRETURN:
					GENERIC("reloadandreturn");
					still_going=0;
					break;
				case GLD_2CALL:
					fread(&arg1,1,2,fileM);
					arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
					fprintf(fsend,"2call 0x%X ' 0x%X\r\n",arg1,arg2);
					Do(arg2);
					break;
				case GLD_3CALL:
					fread(&arg1,1,3,fileM);
					arg2=PointerToOffset(arg1);
					fprintf(fsend,"3call 0x%X ' 0x%X\r\n",arg1,arg2);
					Do(arg2);
					break;
				case GLD_2PTCALL:
					fread(&arg1,1,2,fileM);
					arg3=ftell(fileM);
					arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
					if (arg2!=0xFFFFFFFF)
					{
						fseek(fileM,arg2,SEEK_SET);
						fread(&arg4,1,3,fileM);
						fseek(fileM,arg3,SEEK_SET);
					}
					fprintf(fsend,"2ptcall 0x%X ' 0x%X->0x%X = 0x%X\r\n",arg1,arg2,arg4,PointerToOffset(arg4));
					Do(arg4);
					break;
				case GLD_2JUMP:
					fread(&arg1,1,2,fileM);
					arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
					fprintf(fsend,"2jump 0x%X ' 0x%X\r\n",arg1,arg2);
					still_going=0;
					Do(arg2);
					break;
				case GLD_PRIORITYJUMP:
					fread(&arg1,1,2,fileM);
					arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
					fprintf(fsend,"priorityjump 0x%X ' 0x%X\r\n",arg1,arg2);
					still_going=0;
					Do(arg2);
					break;
				case GLD_3JUMP:
					fread(&arg1,1,3,fileM);
					arg2=PointerToOffset(arg1);
					fprintf(fsend,"3jump 0x%X ' 0x%X\r\n",arg1,arg2);
					still_going=0;
					Do(arg2);
					break;
				case GLD_2PTJUMP:
					fread(&arg1,1,2,fileM);
					arg3=ftell(fileM);
					arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
					if (arg2!=0xFFFFFFFF)
					{
						fseek(fileM,arg2,SEEK_SET);
						fread(&arg4,1,3,fileM);
						fseek(fileM,arg3,SEEK_SET);
					}
					fprintf(fsend,"2ptjump 0x%X ' 0x%X->0x%X = 0x%X\r\n",arg1,arg2,arg4,PointerToOffset(arg4));
					still_going=0;
					Do(arg4);
					break;
				case GLD_PTPRIORITYJUMP:
					fread(&arg1,1,2,fileM);
					arg3=ftell(fileM);
					arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
					if (arg2!=0xFFFFFFFF)
					{
						fseek(fileM,arg2,SEEK_SET);
						fread(&arg4,1,3,fileM);
						fseek(fileM,arg3,SEEK_SET);
					}
					fprintf(fsend,"ptpriorityjump 0x%X ' 0x%X->0x%X = 0x%X\r\n",arg1,arg2,arg4,PointerToOffset(arg4));
					still_going=0;
					Do(arg4);
					break;
				case GLD_EQBYTE:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,2,fileM);
					arg3=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg2<<8));
					fprintf(fsend,"if == 0x%X 0x%X ' 0x%X\r\n",arg1,arg2,arg3);
					Do(arg3);
					break;
				case GLD_NEQBYTE:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,2,fileM);
					arg3=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg2<<8));
					fprintf(fsend,"if != 0x%X 0x%X ' 0x%X\r\n",arg1,arg2,arg3);
					Do(arg3);
					break;
				case GLD_EQZERO:
					fread(&arg1,1,2,fileM);
					arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
					fprintf(fsend,"if false 0x%X ' 0x%X\r\n",arg1,arg2);
					Do(arg2);
					break;
				case GLD_NEQZERO:
					fread(&arg1,1,2,fileM);
					arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
					fprintf(fsend,"if true 0x%X ' 0x%X\r\n",arg1,arg2);
					Do(arg2);
					break;
				case GLD_LTBYTE:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,2,fileM);
					arg3=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg2<<8));
					fprintf(fsend,"if < 0x%X 0x%X ' 0x%X\r\n",arg1,arg2,arg3);
					Do(arg3);
					break;
				case GLD_GTBYTE:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,2,fileM);
					arg3=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg2<<8));
					fprintf(fsend,"if > 0x%X 0x%X ' 0x%X\r\n",arg1,arg2,arg3);
					Do(arg3);
					break;
				case GLD_JUMPSTD:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"jumpstd 0x%X\r\n",arg1);
					still_going=0;
					break;
				case GLD_CALLSTD:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"callstd 0x%X\r\n",arg1);
					break;
				case GLD_3CALLASM:
					fread(&arg1,1,3,fileM);
					fprintf(fsend,"3callasm 0x%X\r\n",arg1);
					break;
				case GLD_SPECIAL:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"special 0x%X\r\n",arg1);
					break;
				case GLD_2PTCALLASM:
					fread(&arg1,1,2,fileM);
					arg3=ftell(fileM);
					arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
					if (arg2!=0xFFFFFFFF)
					{
						fseek(fileM,arg2,SEEK_SET);
						fread(&arg4,1,3,fileM);
						fseek(fileM,arg3,SEEK_SET);
					}
					fprintf(fsend,"2ptcallasm 0x%X ' 0x%X->0x%X = 0x%X\r\n",arg1,arg2,arg4,PointerToOffset(arg4));
					break;
				case GLD_CHECKMAPTRIGGERS:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fprintf(fsend,"checkmaptriggers 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case GLD_DOMAPTRIGGER:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fread(&arg3,1,1,fileM);
					fprintf(fsend,"domaptrigger 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
					break;
				case GLD_DOTRIGGER:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"dotrigger 0x%X\r\n",arg1);
					break;
				case GLD_CHECKTRIGGERS:
					GENERIC("checktriggers");
					break;
				case GLD_LOADPIKADATA:
					GENERIC("loadpikadata");
					break;
				case GLD_LOADTRAINERSEEN:
					GENERIC("loadtrainerseen");
					break;
				case GLD_INTERPRETMENU:
					GENERIC("interpretmenu");
					break;
				case GLD_INTERPRETMENU2:
					GENERIC("interpretmenu2");
					break;
				case GLD_POKEPICYESORNO:
					GENERIC("pokepicyesorno");
					break;
				case GLD_HALLOFFAME:
					GENERIC("halloffame");
					break;
				case GLD_CREDITS:
					GENERIC("credits");
					break;
				case GLD_CHECKPHONECALL:
					GENERIC("checkphonecall");
					break;
				case GLD_HANGUP:
					GENERIC("hangup");
					break;
				case GLD_RESETFUNCS:
					GENERIC("resetfuncs");
					break;
				case GLD_FACEPLAYER:
					GENERIC("faceplayer");
					break;
				case GLD_LOADFONT:
					GENERIC("loadfont");
					break;
				case GLD_PLAYRAMMUSIC:
					GENERIC("playrammusic");
					break;
				case GLD_PLAYMAPMUSIC:
					GENERIC("playmapmusic");
					break;
				case GLD_WARPSOUND:
					GENERIC("warpsound");
					break;
				case GLD_SPECIALSOUND:
					GENERIC("specialsound");
					break;
				case GLD_RELOADMAPMUSIC:
					GENERIC("reloadmapmusic");
					break;
				case GLD_KEEPTEXTOPEN:
					GENERIC("keeptextopen");
					break;
				case GLD_PASSTOENGINE:
					fread(&arg1,1,3,fileM);
					arg2=PointerToOffset(arg1);
					fprintf(fsend,"passtoengine 0x%X ' 0x%X\r\n",arg1,arg2);
					break;
				case GLD_CHECKBIT1:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"checkbit1 0x%X\r\n",arg1);
					break;
				case GLD_SETBIT1:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"setbit1 0x%X\r\n",arg1);
					break;
				case GLD_SETBIT2:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"setbit2 0x%X\r\n",arg1);
					break;
				case GLD_CHECKBIT2:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"checkbit2 0x%X\r\n",arg1);
					break;
				case GLD_CLEARBIT2:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"clearbit2 0x%X\r\n",arg1);
					break;
				case GLD_CLEARBIT1:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"clearbit1 0x%X\r\n",arg1);
					break;
				case GLD_2WRITETEXT:
					fread(&arg1,1,2,fileM);
					arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
					fprintf(fsend,"2writetext 0x%X ' 0x%X\r\n",arg1,arg2);
					if (arg2!=0xFFFFFFFF)
						DoText(arg2);
					break;
				case GLD_3WRITETEXT:
					fread(&arg1,1,3,fileM);
					arg2=PointerToOffset(arg1);
					fprintf(fsend,"3writetext 0x%X ' 0x%X\r\n",arg1,arg2);
					if (arg2!=0xFFFFFFFF)
						DoText(arg2);
					break;
				case GLD_REPEATTEXT:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"repeattext 0x%X\r\n",arg1);
					break;
				case GLD_POKEPIC:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"pokepic 0x%X\r\n",arg1);
					break;
				case GLD_DESCRIBEDECORATION:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"describedecoration 0x%X\r\n",arg1);
					break;
				case GLD_FRUITTREE:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"fruittree 0x%X\r\n",arg1);
					break;
				case GLD_SPECIALPHONECALL:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"specialphonecall 0x%X\r\n",arg1);
					break;
				case GLD_FACEPERSON:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fprintf(fsend,"faceperson 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case GLD_VARIABLESPRITE:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fprintf(fsend,"variablesprite 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case GLD_APPLYMOVEMENT:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,2,fileM);
					arg3=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg2<<8));
					fprintf(fsend,"applymovement 0x%X 0x%X ' 0x%X\r\n",arg1,arg2,arg3);
					if (arg3!=-1)DoMove(arg3);
					break;
				case GLD_APPLYMOVEOTHER:
					fread(&arg1,1,2,fileM);
					arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
					fprintf(fsend,"applymoveother 0x%X ' 0x%X Applies movement to last talked\r\n",arg1,arg2);
					if (arg2!=-1)DoMove(arg2);
					break;
				case GLD_VERBOSEGIVEITEM:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					if(!GetItemName(arg1))
						sprintf(buf,"0x%X",arg1);
					else
						strcpy(buf,GetItemName(arg1));
					fprintf(fsend,"verbosegiveitem %s %u\r\n",buf,arg2);
					break;
				case GLD_LOADWILDDATA:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fprintf(fsend,"loadwilddata 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case GLD_LOADTRAINER:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fprintf(fsend,"loadtrainer 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case GLD_CATCHTUTORIAL:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"catchtutorial 0x%X\r\n",arg1);
					break;
				case GLD_TRAINERTEXT:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"trainertext 0x%X\r\n",arg1);
					break;
				case GLD_TRAINERSTATUS:
					fread(&arg1,1,1,fileM);
					*buf2=0;
					switch (arg1) {
					case 0:
						strcpy(buf2," ' Deactivate Trainer");
						break;
					case 1:
						strcpy(buf2," ' Activate Trainer");
						break;
					case 2:
						strcpy(buf2," ' Check if Trainer is Activated");
						break;
					}
					fprintf(fsend,"trainerstatus 0x%X%s\r\n",arg1,buf2);
					break;
				case GLD_CLOSETEXT:
					GENERIC("closetext");
					break;
				case GLD_LOADMOVESPRITES:
					GENERIC("loadmovesprites");
					break;
				case GLD_POKEMART:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,2,fileM);
					fprintf(fsend,"pokemart 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case GLD_ELEVATOR:
					fread(&arg1,1,2,fileM);
					arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
					fprintf(fsend,"elevator 0x%X ' 0x%X\r\n",arg1,arg2);
					break;
				case GLD_YESORNO:
					GENERIC("yesorno");
					break;
				case GLD_RETURNAFTERBATTLE:
					GENERIC("returnafterbattle");
					break;
				case GLD_WAITBUTTON:
					GENERIC("waitbutton");
					break;
				case GLD_WRITEBACKUP:
					GENERIC("writebackup");
					break;
				case GLD_WILDOFF:
					GENERIC("wildoff");
					break;
				case GLD_WILDON:
					GENERIC("wildon");
					break;
				case GLD_ITEMNOTIFY:
					GENERIC("itemnotify");
					break;
				case GLD_POCKETISFULL:
					GENERIC("pocketisfull");
					break;
				case GLD_STARTBATTLE:
					GENERIC("startbattle");
					break;
				case GLD_CLEARFIGHT:
					GENERIC("clearfight");
					break;
				case GLD_TALKAFTERCANCEL:
					GENERIC("talkaftercancel");
					break;
				case GLD_TALKAFTER:
					GENERIC("talkafter");
					break;
				case GLD_TALKAFTERCHECK:
					GENERIC("talkaftercheck");
					break;
				case GLD_CHECKVER:
					GENERIC("checkver");
					break;
				case GLD_RELOADMAPPART:
					GENERIC("reloadmappart");
					break;
				case GLD_RELOADMAP:
					GENERIC("reloadmap");
					break;
				case GLD_DEACTIVATEFACING:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"deactivatefacing 0x%X\r\n",arg1);
					break;
				case GLD_NEWLOADMAP:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"newloadmap 0x%X\r\n",arg1);
					break;
				case GLD_WARPCHECK:
					GENERIC("warpcheck");
					break;
				case GLD_GIVEITEM:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fprintf(fsend,"giveitem 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case GLD_REFRESHSCREEN:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"refreshscreen 0x%X\r\n",arg1);
					break;
				case GLD_C1CELOADBYTE:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"C1CEloadbyte 0x%X ' Apparently useless.\r\n",arg1);
					break;
				case GLD_SETLASTTALKED:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"setlasttalked 0x%X\r\n",arg1);
					break;
				case GLD_EARTHQUAKE:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"earthquake 0x%X\r\n",arg1);
					break;
				case GLD_LOADVAR:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"loadvar 0x%X\r\n",arg1);
					break;
				case GLD_ADDVAR:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"addvar 0x%X\r\n",arg1);
					break;
				case GLD_RANDOM:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"random 0x%X\r\n",arg1);
					break;
				case GLD_COPYBYTETOVAR:
					fread(&arg1,1,2,fileM);
					arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
					fprintf(fsend,"copybytetovar 0x%X ' 0x%X\r\n",arg1,arg2);
					break;
				case GLD_COPYVARTOBYTE:
					fread(&arg1,1,2,fileM);
					arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
					fprintf(fsend,"copyvartobyte 0x%X ' 0x%X\r\n",arg1,arg2);
					break;
				case GLD_CHANGEBLOCK:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fread(&arg3,1,1,fileM);
					fprintf(fsend,"changeblock 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
					break;
				case GLD_SHOWEMOTE:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fread(&arg3,1,1,fileM);
					fprintf(fsend,"showemote 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
					break;
				case GLD_FOLLOW:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fprintf(fsend,"follow 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case GLD_FOLLOWNOTEXACT:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fprintf(fsend,"follownotexact 0x%X 0x%X ' Follows, but without mindlessly copying movements.\r\n",arg1,arg2);
					break;
				case GLD_SWAPMAPS:
					fread(&arg1,1,3,fileM);
					fprintf(fsend,"swapmaps 0x%X ' 0x%X\r\n",arg1,PointerToOffset(arg1));
					break;
				case GLD_SPRITEFACE:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fprintf(fsend,"spriteface 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case GLD_MOVEPERSON:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fread(&arg3,1,1,fileM);
					fprintf(fsend,"moveperson 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
					break;
				case GLD_WRITEPERSONLOC:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"writepersonloc 0x%X\r\n",arg1);
					break;
				case GLD_LOADEMOTE:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"loademote 0x%X\r\n",arg1);
					break;
				case GLD_STOPFOLLOW:
					GENERIC("stopfollow");
					break;
				case GLD_PLAYSOUND:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"playsound 0x%X\r\n",arg1);
					break;
				case GLD_PLAYMUSIC:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"playmusic 0x%X\r\n",arg1);
					break;
				case GLD_CRY:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"cry 0x%X\r\n",arg1);
					break;
				case GLD_PAUSE:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"pause 0x%X\r\n",arg1);
					break;
				case GLD_WARPMOD:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fread(&arg3,1,1,fileM);
					fprintf(fsend,"warpmod 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
					break;
				case GLD_WARP:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fread(&arg3,1,1,fileM);
					fread(&arg4,1,1,fileM);
					fprintf(fsend,"warp 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4);
					break;
				case GLD_WARPFACING:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fread(&arg3,1,1,fileM);
					fread(&arg4,1,1,fileM);
					fread(&arg5,1,1,fileM);
					fprintf(fsend,"warpfacing 0x%X 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4,arg5);
					break;
				case GLD_BLACKOUTMOD:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fprintf(fsend,"blackoutmod 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case GLD_LOCATIONTOTEXT:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"locationtotext 0x%X\r\n",arg1);
					break;
				case GLD_DISPLAYLOCATION:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"displaylocation 0x%X\r\n",arg1);
					break;
				case GLD_MONEYTOTEXT:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fprintf(fsend,"moneytotext 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case GLD_COINSTOTEXT:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"coinstotext 0x%X\r\n",arg1);
					break;
				case GLD_VARTOTEXT:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"vartotext 0x%X\r\n",arg1);
					break;
				case GLD_POKETOTEXT:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fprintf(fsend,"poketotext 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case GLD_ITEMTOTEXT:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fprintf(fsend,"itemtotext 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case GLD_TRAINERTOTEXT:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fread(&arg3,1,1,fileM);
					fprintf(fsend,"trainertotext 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
					break;
				case GLD_STRINGTOTEXT:
					fread(&arg1,1,2,fileM);
					fread(&arg2,1,1,fileM);
					arg3=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
					fprintf(fsend,"stringtotext 0x%X 0x%X ' 0x%X\r\n",arg1,arg2,arg3);
					if (arg3!=-1)
					{
						DoText(arg3);
					}
					break;
				case GLD_STORETEXT:
					fread(&arg1,1,2,fileM);
					fread(&arg2,1,1,fileM);
					fread(&arg3,1,1,fileM);
					arg4=PointerToOffset((arg1<<8)|arg2);
					fprintf(fsend,"storetext 0x%X 0x%X 0x%X ' 0x%X\r\n",arg1,arg2,arg3,arg4);
					if (arg4!=-1)
					{
						DoText(arg4);
					}
					break;
				case GLD_MUSICFADEOUT:
					fread(&arg1,1,2,fileM);
					fread(&arg2,1,1,fileM);
					fprintf(fsend,"musicfadeout 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case GLD_WRITECMDQUEUE:
					fread(&arg1,1,2,fileM);
					arg3=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
					fprintf(fsend,"writecmdqueue 0x%X ' 0x%X\r\n",arg1,arg3);
					break;
				case GLD_DELCMDQUEUE:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"delcmdqueue 0x%X\r\n",arg1);
					break;
				case GLD_JUMPTEXTFACEPLAYER:
					fread(&arg1,1,2,fileM);
					arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
					fprintf(fsend,"jumptextfaceplayer 0x%X ' 0x%X\r\n",arg1,arg2);
					if (arg2!=-1)
						DoText(arg2);
					still_going=0;
					break;
				case GLD_JUMPTEXT:
					fread(&arg1,1,2,fileM);
					arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
					fprintf(fsend,"jumptext 0x%X ' 0x%X\r\n",arg1,arg2);
					if (arg2!=-1)
						DoText(arg2);
					still_going=0;
					break;
				case GLD_WINLOSSTEXT:
					fread(&arg1,1,2,fileM);
					fread(&arg2,1,2,fileM);
					arg3=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
					arg4=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg2<<8));
					fprintf(fsend,"winlosstext 0x%X 0x%X ' 0x%X,0x%X\r\n",arg1,arg2,arg3,arg4);
					if (arg3!=-1)
						DoText(arg3);
					if (arg4!=-1)
						DoText(arg4);
					break;
				case GLD_APPEAR:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"appear 0x%X\r\n",arg1);
					break;
				case GLD_DISAPPEAR:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"disappear 0x%X\r\n",arg1);
					break;
				case GLD_ASKFORPHONENUMBER:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"askforphonenumber 0x%X\r\n",arg1);
					break;
				case GLD_PHONECALL:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"phonecall 0x%X\r\n",arg1);
					break;
				case GLD_TRADE:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"trade 0x%X\r\n",arg1);
					break;
				case GLD_TAKEITEM:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fprintf(fsend,"takeitem 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case GLD_GIVEMONEY:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,3,fileM);
					fprintf(fsend,"givemoney 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case GLD_TAKEMONEY:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,3,fileM);
					fprintf(fsend,"takemoney 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case GLD_CHECKMONEY:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,3,fileM);
					fprintf(fsend,"checkmoney 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case GLD_GIVECOINS:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"givecoins 0x%X\r\n",arg1);
					break;
				case GLD_TAKECOINS:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"takecoins 0x%X\r\n",arg1);
					break;
				case GLD_CHECKCOINS:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"checkcoins 0x%X\r\n",arg1);
					break;
				case GLD_GIVEPHONENUMBER:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"givephonenumber 0x%X\r\n",arg1);
					break;
				case GLD_TAKEPHONENUMBER:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"takephonenumber 0x%X\r\n",arg1);
					break;
				case GLD_CHECKPHONENUMBER:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"checkphonenumber 0x%X\r\n",arg1);
					break;
				case GLD_CHECKITEM:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"checkitem 0x%X\r\n",arg1);
					break;
				case GLD_CHECKTIME:
					fread(&arg1,1,1,fileM);
					arg2=rand()%10;
					if (arg2==0)
						fprintf(fsend,"checktime 0x%X ' Look, an acronym of CheckItem!\r\n",arg1); //Easter Egg
					else
						fprintf(fsend,"checktime 0x%X\r\n",arg1);
					break;
				case GLD_CHECKPOKE:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"checkpoke 0x%X\r\n",arg1);
					break;
				case GLD_GIVEEGG:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fprintf(fsend,"giveegg 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case GLD_GIVEPOKEITEM:
					fread(&arg1,1,2,fileM);
					arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
					fprintf(fsend,"givepokeitem 0x%X ' 0x%X\r\n",arg1,arg2);
					break;
				case GLD_TAKEIFLETTER:
					fread(&arg1,1,2,fileM);
					arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg2<<8));
					fprintf(fsend,"takeifletter 0x%X ' 0x%X\r\n",arg1,arg2);
					break;
				case GLD_XYCOMPARE:
					fread(&arg1,1,2,fileM);
					arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg2<<8));
					fprintf(fsend,"xycompare 0x%X ' 0x%X\r\n",arg1,arg2);
					break;
				case GLD_GIVEPOKE:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fread(&arg3,1,1,fileM);
					fread(&arg4,1,1,fileM);
					if (arg4==1)
					{
						fread(&arg5,1,2,fileM);
						fread(&arg6,1,2,fileM);
						arg7=(OffsetToPointer(FileZoomPos)&0xFF);
						fprintf(fsend,"givepoke 0x%X 0x%X 0x%X 0x%X ' 0x%X,0x%X\r\n",arg1,arg2,arg3,arg4,arg5,arg6,
										PointerToOffset(arg7|(arg5<<8)),
										PointerToOffset(arg7|(arg6<<8)));
					}
					else
					{
						fprintf(fsend,"givepoke 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4);
					}
					break;
				case GLD_LOADMENUDATA:
					fread(&arg1,1,2,fileM);
					arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
					fprintf(fsend,"loadmenudata 0x%X ' 0x%X\r\n",arg1,arg2);
					break;
				case GLD_LOADPOKEDATA:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fprintf(fsend,"loadpokedata 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case GLD_CHECKCODE:
					fread(&arg1,1,1,fileM);
					//fread(&arg2,1,1,fileM);
					//fprintf(fsend,"checkcode 0x%X 0x%X\r\n",arg1,arg2);
					fprintf(fsend,"checkcode 0x%X\r\n",arg1);
					break;
				case GLD_WRITEBYTE:
					fread(&arg1,1,2,fileM);
					fread(&arg2,1,1,fileM);
					fprintf(fsend,"writebyte 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case GLD_WRITEVARCODE:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"writevarcode 0x%X\r\n",arg1);
					break;
				case GLD_WRITECODE:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					if (arg1==03)
					{
						*buf2=0;
						switch (arg2)
						{
						case 0:
						case 1:
						case 5:
						case 9:
							strcpy(buf2," ' Normal fight");
							break;
						case 2:
							strcpy(buf2," ' Fight with HIRO's backpic");
							break;
						case 3:
							strcpy(buf2," ' Fight with DUDE's backpic");
							break;
						case 4:
							strcpy(buf2," ' Fight pokemon caught with rod");
							break;
						case 6:
							strcpy(buf2," ' Instant win fight");
							break;
						case 7:
							strcpy(buf2," ' Shiny pokemon fight");
							break;
						case 8:
							strcpy(buf2," ' Tree pokemon fight");
							break;
						}
						fprintf(fsend,"writecode 0x3 0x%X%s\r\n",arg2,buf2);
					}
					else
					{
						fprintf(fsend,"writecode 0x%X 0x%X\r\n",arg1,arg2);
					}
					break;
				default:
					fprintf(fsend,"#raw 0x%X\r\n",command);
					break;
				}
			}
			else
			{
				puts("'--EOF--");
				still_going=0;
			}
		}
	}
	else
	{
		while (still_going)
		{
			read=(signed int)fread(&command,1,1,fileM);
			if (read>0)
			{
#define GENERIC(x) fprintf(fsend,"%s\r\n",x)
				arg1=arg2=arg3=arg4=arg5=arg6=arg7=0;
				switch (command)
				{
				case CRY_END:
					GENERIC("end");
					still_going=0;
					break;
				case CRY_RETURN:
					GENERIC("return");
					still_going=0;
					break;
				case CRY_RELOADANDRETURN:
					GENERIC("reloadandreturn");
					still_going=0;
					break;
				case CRY_2CALL:
					fread(&arg1,1,2,fileM);
					arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
					fprintf(fsend,"2call 0x%X ' 0x%X\r\n",arg1,arg2);
					Do(arg2);
					break;
				case CRY_3CALL:
					fread(&arg1,1,3,fileM);
					arg2=PointerToOffset(arg1);
					fprintf(fsend,"3call 0x%X ' 0x%X\r\n",arg1,arg2);
					Do(arg2);
					break;
				case CRY_2PTCALL:
					fread(&arg1,1,2,fileM);
					arg3=ftell(fileM);
					arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
					if (arg2!=0xFFFFFFFF)
					{
						fseek(fileM,arg2,SEEK_SET);
						fread(&arg4,1,3,fileM);
						fseek(fileM,arg3,SEEK_SET);
					}
					fprintf(fsend,"2ptcall 0x%X ' 0x%X->0x%X = 0x%X\r\n",arg1,arg2,arg4,PointerToOffset(arg4));
					Do(arg4);
					break;
				case CRY_2JUMP:
					fread(&arg1,1,2,fileM);
					arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
					fprintf(fsend,"2jump 0x%X ' 0x%X\r\n",arg1,arg2);
					still_going=0;
					Do(arg2);
					break;
				case CRY_PRIORITYJUMP:
					fread(&arg1,1,2,fileM);
					arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
					fprintf(fsend,"priorityjump 0x%X ' 0x%X\r\n",arg1,arg2);
					still_going=0;
					Do(arg2);
					break;
				case CRY_3JUMP:
					fread(&arg1,1,3,fileM);
					arg2=PointerToOffset(arg1);
					fprintf(fsend,"3jump 0x%X ' 0x%X\r\n",arg1,arg2);
					still_going=0;
					Do(arg2);
					break;
				case CRY_2PTJUMP:
					fread(&arg1,1,2,fileM);
					arg3=ftell(fileM);
					arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
					if (arg2!=0xFFFFFFFF)
					{
						fseek(fileM,arg2,SEEK_SET);
						fread(&arg4,1,3,fileM);
						fseek(fileM,arg3,SEEK_SET);
					}
					fprintf(fsend,"2ptjump 0x%X ' 0x%X->0x%X = 0x%X\r\n",arg1,arg2,arg4,PointerToOffset(arg4));
					still_going=0;
					Do(arg4);
					break;
				case CRY_PTPRIORITYJUMP:
					fread(&arg1,1,2,fileM);
					arg3=ftell(fileM);
					arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
					if (arg2!=0xFFFFFFFF)
					{
						fseek(fileM,arg2,SEEK_SET);
						fread(&arg4,1,3,fileM);
						fseek(fileM,arg3,SEEK_SET);
					}
					fprintf(fsend,"ptpriorityjump 0x%X ' 0x%X->0x%X = 0x%X\r\n",arg1,arg2,arg4,PointerToOffset(arg4));
					still_going=0;
					Do(arg4);
					break;
				case CRY_EQBYTE:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,2,fileM);
					arg3=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg2<<8));
					fprintf(fsend,"if == 0x%X 0x%X ' 0x%X\r\n",arg1,arg2,arg3);
					Do(arg3);
					break;
				case CRY_NEQBYTE:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,2,fileM);
					arg3=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg2<<8));
					fprintf(fsend,"if != 0x%X 0x%X ' 0x%X\r\n",arg1,arg2,arg3);
					Do(arg3);
					break;
				case CRY_EQZERO:
					fread(&arg1,1,2,fileM);
					arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
					fprintf(fsend,"if false 0x%X ' 0x%X\r\n",arg1,arg2);
					Do(arg2);
					break;
				case CRY_NEQZERO:
					fread(&arg1,1,2,fileM);
					arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
					fprintf(fsend,"if true 0x%X ' 0x%X\r\n",arg1,arg2);
					Do(arg2);
					break;
				case CRY_LTBYTE:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,2,fileM);
					arg3=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg2<<8));
					fprintf(fsend,"if < 0x%X 0x%X ' 0x%X\r\n",arg1,arg2,arg3);
					Do(arg3);
					break;
				case CRY_GTBYTE:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,2,fileM);
					arg3=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg2<<8));
					fprintf(fsend,"if > 0x%X 0x%X ' 0x%X\r\n",arg1,arg2,arg3);
					Do(arg3);
					break;
				case CRY_JUMPSTD:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"jumpstd 0x%X\r\n",arg1);
					still_going=0;
					break;
				case CRY_CALLSTD:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"callstd 0x%X\r\n",arg1);
					break;
				case CRY_3CALLASM:
					fread(&arg1,1,3,fileM);
					fprintf(fsend,"3callasm 0x%X\r\n",arg1);
					break;
				case CRY_SPECIAL:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"special 0x%X\r\n",arg1);
					break;
				case CRY_2PTCALLASM:
					fread(&arg1,1,2,fileM);
					arg3=ftell(fileM);
					arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
					if (arg2!=0xFFFFFFFF)
					{
						fseek(fileM,arg2,SEEK_SET);
						fread(&arg4,1,3,fileM);
						fseek(fileM,arg3,SEEK_SET);
					}
					fprintf(fsend,"2ptcallasm 0x%X ' 0x%X->0x%X = 0x%X\r\n",arg1,arg2,arg4,PointerToOffset(arg4));
					break;
				case CRY_CHECKMAPTRIGGERS:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fprintf(fsend,"checkmaptriggers 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case CRY_DOMAPTRIGGER:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fread(&arg3,1,1,fileM);
					fprintf(fsend,"domaptrigger 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
					break;
				case CRY_DOTRIGGER:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"dotrigger 0x%X\r\n",arg1);
					break;
				case CRY_CHECKTRIGGERS:
					GENERIC("checktriggers");
					break;
				case CRY_LOADPIKADATA:
					GENERIC("loadpikadata");
					break;
				case CRY_LOADTRAINERSEEN:
					GENERIC("loadtrainerseen");
					break;
				case CRY_INTERPRETMENU:
					GENERIC("interpretmenu");
					break;
				case CRY_INTERPRETMENU2:
					GENERIC("interpretmenu2");
					break;
				case CRY_POKEPICYESORNO:
					GENERIC("pokepicyesorno");
					break;
				case CRY_HALLOFFAME:
					GENERIC("halloffame");
					break;
				case CRY_CREDITS:
					GENERIC("credits");
					break;
				case CRY_CHECKPHONECALL:
					GENERIC("checkphonecall");
					break;
				case CRY_HANGUP:
					GENERIC("hangup");
					break;
				case CRY_RESETFUNCS:
					GENERIC("resetfuncs");
					break;
				case CRY_FACEPLAYER:
					GENERIC("faceplayer");
					break;
				case CRY_LOADFONT:
					GENERIC("loadfont");
					break;
				case CRY_PLAYRAMMUSIC:
					GENERIC("playrammusic");
					break;
				case CRY_PLAYMAPMUSIC:
					GENERIC("playmapmusic");
					break;
				case CRY_WARPSOUND:
					GENERIC("warpsound");
					break;
				case CRY_SPECIALSOUND:
					GENERIC("specialsound");
					break;
				case CRY_RELOADMAPMUSIC:
					GENERIC("reloadmapmusic");
					break;
				case CRY_KEEPTEXTOPEN:
					GENERIC("keeptextopen");
					break;
				case CRY_PASSTOENGINE:
					fread(&arg1,1,3,fileM);
					arg2=PointerToOffset(arg1);
					fprintf(fsend,"passtoengine 0x%X ' 0x%X\r\n",arg1,arg2);
					break;
				case CRY_CHECKBIT1:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"checkbit1 0x%X\r\n",arg1);
					break;
				case CRY_SETBIT1:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"setbit1 0x%X\r\n",arg1);
					break;
				case CRY_SETBIT2:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"setbit2 0x%X\r\n",arg1);
					break;
				case CRY_CHECKBIT2:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"checkbit2 0x%X\r\n",arg1);
					break;
				case CRY_CLEARBIT2:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"clearbit2 0x%X\r\n",arg1);
					break;
				case CRY_CLEARBIT1:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"clearbit1 0x%X\r\n",arg1);
					break;
				case CRY_2WRITETEXT:
					fread(&arg1,1,2,fileM);
					arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
					fprintf(fsend,"2writetext 0x%X ' 0x%X\r\n",arg1,arg2);
					if (arg2!=0xFFFFFFFF)
						DoText(arg2);
					break;
				case CRY_3WRITETEXT:
					fread(&arg1,1,3,fileM);
					arg2=PointerToOffset(arg1);
					fprintf(fsend,"3writetext 0x%X ' 0x%X\r\n",arg1,arg2);
					if (arg2!=0xFFFFFFFF)
						DoText(arg2);
					break;
				case CRY_REPEATTEXT:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"repeattext 0x%X\r\n",arg1);
					break;
				case CRY_POKEPIC:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"pokepic 0x%X\r\n",arg1);
					break;
				case CRY_DESCRIBEDECORATION:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"describedecoration 0x%X\r\n",arg1);
					break;
				case CRY_FRUITTREE:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"fruittree 0x%X\r\n",arg1);
					break;
				case CRY_SPECIALPHONECALL:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"specialphonecall 0x%X\r\n",arg1);
					break;
				case CRY_FACEPERSON:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fprintf(fsend,"faceperson 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case CRY_VARIABLESPRITE:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fprintf(fsend,"variablesprite 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case CRY_APPLYMOVEMENT:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,2,fileM);
					arg3=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg2<<8));
					fprintf(fsend,"applymovement 0x%X 0x%X ' 0x%X\r\n",arg1,arg2,arg3);
					if (arg3!=-1)DoMove(arg3);
					break;
				case CRY_APPLYMOVEOTHER:
					fread(&arg1,1,2,fileM);
					arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
					fprintf(fsend,"applymoveother 0x%X ' 0x%X Applies movement to last talked\r\n",arg1,arg2);
					if (arg2!=-1)DoMove(arg2);
					break;
				case CRY_VERBOSEGIVEITEM:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fprintf(fsend,"verbosegiveitem 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case CRY_LOADWILDDATA:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fprintf(fsend,"loadwilddata 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case CRY_LOADTRAINER:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fprintf(fsend,"loadtrainer 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case CRY_CATCHTUTORIAL:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"catchtutorial 0x%X\r\n",arg1);
					break;
				case CRY_TRAINERTEXT:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"trainertext 0x%X\r\n",arg1);
					break;
				case CRY_TRAINERSTATUS:
					fread(&arg1,1,1,fileM);
					*buf2=0;
					switch (arg1) {
					case 0:
						strcpy(buf2," ' Deactivate Trainer");
						break;
					case 1:
						strcpy(buf2," ' Activate Trainer");
						break;
					case 2:
						strcpy(buf2," ' Check if Trainer is Activated");
						break;
					}
					fprintf(fsend,"trainerstatus 0x%X%s\r\n",arg1,buf2);
					break;
				case CRY_CLOSETEXT:
					GENERIC("closetext");
					break;
				case CRY_LOADMOVESPRITES:
					GENERIC("loadmovesprites");
					break;
				case CRY_POKEMART:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,2,fileM);
					fprintf(fsend,"pokemart 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case CRY_ELEVATOR:
					fread(&arg1,1,2,fileM);
					arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
					fprintf(fsend,"elevator 0x%X ' 0x%X\r\n",arg1,arg2);
					break;
				case CRY_YESORNO:
					GENERIC("yesorno");
					break;
				case CRY_RETURNAFTERBATTLE:
					GENERIC("returnafterbattle");
					break;
				case CRY_WAITBUTTON:
					GENERIC("waitbutton");
					break;
				case CRY_WRITEBACKUP:
					GENERIC("writebackup");
					break;
				case CRY_WILDOFF:
					GENERIC("wildoff");
					break;
				case CRY_WILDON:
					GENERIC("wildon");
					break;
				case CRY_ITEMNOTIFY:
					GENERIC("itemnotify");
					break;
				case CRY_POCKETISFULL:
					GENERIC("pocketisfull");
					break;
				case CRY_STARTBATTLE:
					GENERIC("startbattle");
					break;
				case CRY_CLEARFIGHT:
					GENERIC("clearfight");
					break;
				case CRY_TALKAFTERCANCEL:
					GENERIC("talkaftercancel");
					break;
				case CRY_TALKAFTER:
					GENERIC("talkafter");
					break;
				case CRY_TALKAFTERCHECK:
					GENERIC("talkaftercheck");
					break;
				case CRY_CHECKVER:
					GENERIC("checkver");
					break;
				case CRY_RELOADMAPPART:
					GENERIC("reloadmappart");
					break;
				case CRY_RELOADMAP:
					GENERIC("reloadmap");
					break;
				case CRY_DEACTIVATEFACING:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"deactivatefacing 0x%X\r\n",arg1);
					break;
				case CRY_NEWLOADMAP:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"newloadmap 0x%X\r\n",arg1);
					break;
				case CRY_WARPCHECK:
					GENERIC("warpcheck");
					break;
				case CRY_GIVEITEM:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fprintf(fsend,"giveitem 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case CRY_REFRESHSCREEN:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"refreshscreen 0x%X\r\n",arg1);
					break;
				case CRY_C1CELOADBYTE:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"C1CEloadbyte 0x%X ' Apparently useless.\r\n",arg1);
					break;
				case CRY_SETLASTTALKED:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"setlasttalked 0x%X\r\n",arg1);
					break;
				case CRY_EARTHQUAKE:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"earthquake 0x%X\r\n",arg1);
					break;
				case CRY_LOADVAR:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"loadvar 0x%X\r\n",arg1);
					break;
				case CRY_ADDVAR:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"addvar 0x%X\r\n",arg1);
					break;
				case CRY_RANDOM:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"random 0x%X\r\n",arg1);
					break;
				case CRY_COPYBYTETOVAR:
					fread(&arg1,1,2,fileM);
					arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
					fprintf(fsend,"copybytetovar 0x%X ' 0x%X\r\n",arg1,arg2);
					break;
				case CRY_COPYVARTOBYTE:
					fread(&arg1,1,2,fileM);
					arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
					fprintf(fsend,"copyvartobyte 0x%X ' 0x%X\r\n",arg1,arg2);
					break;
				case CRY_CHANGEBLOCK:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fread(&arg3,1,1,fileM);
					fprintf(fsend,"changeblock 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
					break;
				case CRY_SHOWEMOTE:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fread(&arg3,1,1,fileM);
					fprintf(fsend,"showemote 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
					break;
				case CRY_FOLLOW:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fprintf(fsend,"follow 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case CRY_FOLLOWNOTEXACT:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fprintf(fsend,"follownotexact 0x%X 0x%X ' Follows, but without mindlessly copying movements.\r\n",arg1,arg2);
					break;
				case CRY_SWAPMAPS:
					fread(&arg1,1,3,fileM);
					fprintf(fsend,"swapmaps 0x%X ' 0x%X\r\n",arg1,PointerToOffset(arg1));
					break;
				case CRY_SPRITEFACE:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fprintf(fsend,"spriteface 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case CRY_MOVEPERSON:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fread(&arg3,1,1,fileM);
					fprintf(fsend,"moveperson 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
					break;
				case CRY_WRITEPERSONLOC:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"writepersonloc 0x%X\r\n",arg1);
					break;
				case CRY_LOADEMOTE:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"loademote 0x%X\r\n",arg1);
					break;
				case CRY_STOPFOLLOW:
					GENERIC("stopfollow");
					break;
				case CRY_PLAYSOUND:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"playsound 0x%X\r\n",arg1);
					break;
				case CRY_PLAYMUSIC:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"playmusic 0x%X\r\n",arg1);
					break;
				case CRY_CRY:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"cry 0x%X\r\n",arg1);
					break;
				case CRY_PAUSE:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"pause 0x%X\r\n",arg1);
					break;
				case CRY_WARPMOD:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fread(&arg3,1,1,fileM);
					fprintf(fsend,"warpmod 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
					break;
				case CRY_WARP:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fread(&arg3,1,1,fileM);
					fread(&arg4,1,1,fileM);
					fprintf(fsend,"warp 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4);
					break;
				case CRY_WARPFACING:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fread(&arg3,1,1,fileM);
					fread(&arg4,1,1,fileM);
					fread(&arg5,1,1,fileM);
					fprintf(fsend,"warpfacing 0x%X 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4,arg5);
					break;
				case CRY_BLACKOUTMOD:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fprintf(fsend,"blackoutmod 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case CRY_LOCATIONTOTEXT:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"locationtotext 0x%X\r\n",arg1);
					break;
				case CRY_DISPLAYLOCATION:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"displaylocation 0x%X\r\n",arg1);
					break;
				case CRY_MONEYTOTEXT:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fprintf(fsend,"moneytotext 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case CRY_COINSTOTEXT:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"coinstotext 0x%X\r\n",arg1);
					break;
				case CRY_VARTOTEXT:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"vartotext 0x%X\r\n",arg1);
					break;
				case CRY_POKETOTEXT:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fprintf(fsend,"poketotext 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case CRY_ITEMTOTEXT:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fprintf(fsend,"itemtotext 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case CRY_TRAINERTOTEXT:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fread(&arg3,1,1,fileM);
					fprintf(fsend,"trainertotext 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3);
					break;
				case CRY_STRINGTOTEXT:
					fread(&arg1,1,2,fileM);
					fread(&arg2,1,1,fileM);
					arg3=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
					fprintf(fsend,"stringtotext 0x%X 0x%X ' 0x%X\r\n",arg1,arg2,arg3);
					if (arg3!=-1)
					{
						DoText(arg3);
					}
					break;
				case CRY_STORETEXT:
					fread(&arg1,1,2,fileM);
					fread(&arg2,1,1,fileM);
					fread(&arg3,1,1,fileM);
					arg4=PointerToOffset((arg1<<8)|arg2);
					fprintf(fsend,"storetext 0x%X 0x%X 0x%X ' 0x%X\r\n",arg1,arg2,arg3,arg4);
					if (arg4!=-1)
					{
						DoText(arg4);
					}
					break;
				case CRY_MUSICFADEOUT:
					fread(&arg1,1,2,fileM);
					fread(&arg2,1,1,fileM);
					fprintf(fsend,"musicfadeout 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case CRY_WRITECMDQUEUE:
					fread(&arg1,1,2,fileM);
					arg3=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
					fprintf(fsend,"writecmdqueue 0x%X ' 0x%X\r\n",arg1,arg3);
					break;
				case CRY_DELCMDQUEUE:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"delcmdqueue 0x%X\r\n",arg1);
					break;
				case CRY_JUMPTEXTFACEPLAYER:
					fread(&arg1,1,2,fileM);
					arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
					fprintf(fsend,"jumptextfaceplayer 0x%X ' 0x%X\r\n",arg1,arg2);
					if (arg2!=-1)
						DoText(arg2);
					still_going=0;
					break;
				case CRY_JUMPTEXT:
					fread(&arg1,1,2,fileM);
					arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
					fprintf(fsend,"jumptext 0x%X ' 0x%X\r\n",arg1,arg2);
					if (arg2!=-1)
						DoText(arg2);
					still_going=0;
					break;
				case CRY_WINLOSSTEXT:
					fread(&arg1,1,2,fileM);
					fread(&arg2,1,2,fileM);
					arg3=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
					arg4=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg2<<8));
					fprintf(fsend,"winlosstext 0x%X 0x%X ' 0x%X,0x%X\r\n",arg1,arg2,arg3,arg4);
					if (arg3!=-1)
						DoText(arg3);
					if (arg4!=-1)
						DoText(arg4);
					break;
				case CRY_APPEAR:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"appear 0x%X\r\n",arg1);
					break;
				case CRY_DISAPPEAR:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"disappear 0x%X\r\n",arg1);
					break;
				case CRY_ASKFORPHONENUMBER:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"askforphonenumber 0x%X\r\n",arg1);
					break;
				case CRY_PHONECALL:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"phonecall 0x%X\r\n",arg1);
					break;
				case CRY_TRADE:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"trade 0x%X\r\n",arg1);
					break;
				case CRY_TAKEITEM:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fprintf(fsend,"takeitem 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case CRY_GIVEMONEY:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,3,fileM);
					fprintf(fsend,"givemoney 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case CRY_TAKEMONEY:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,3,fileM);
					fprintf(fsend,"takemoney 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case CRY_CHECKMONEY:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,3,fileM);
					fprintf(fsend,"checkmoney 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case CRY_GIVECOINS:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"givecoins 0x%X\r\n",arg1);
					break;
				case CRY_TAKECOINS:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"takecoins 0x%X\r\n",arg1);
					break;
				case CRY_CHECKCOINS:
					fread(&arg1,1,2,fileM);
					fprintf(fsend,"checkcoins 0x%X\r\n",arg1);
					break;
				case CRY_GIVEPHONENUMBER:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"givephonenumber 0x%X\r\n",arg1);
					break;
				case CRY_TAKEPHONENUMBER:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"takephonenumber 0x%X\r\n",arg1);
					break;
				case CRY_CHECKPHONENUMBER:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"checkphonenumber 0x%X\r\n",arg1);
					break;
				case CRY_CHECKITEM:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"checkitem 0x%X\r\n",arg1);
					break;
				case CRY_CHECKTIME:
					fread(&arg1,1,1,fileM);
					arg2=rand()%10;
					if (arg2==0)
						fprintf(fsend,"checktime 0x%X ' Look, an acronym of CheckItem!\r\n",arg1); //Easter Egg
					else
						fprintf(fsend,"checktime 0x%X\r\n",arg1);
					break;
				case CRY_CHECKPOKE:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"checkpoke 0x%X\r\n",arg1);
					break;
				case CRY_GIVEEGG:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fprintf(fsend,"giveegg 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case CRY_GIVEPOKEITEM:
					fread(&arg1,1,2,fileM);
					arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
					fprintf(fsend,"givepokeitem 0x%X ' 0x%X\r\n",arg1,arg2);
					break;
				case CRY_TAKEIFLETTER:
					fread(&arg1,1,2,fileM);
					arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg2<<8));
					fprintf(fsend,"takeifletter 0x%X ' 0x%X\r\n",arg1,arg2);
					break;
				case CRY_XYCOMPARE:
					fread(&arg1,1,2,fileM);
					arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg2<<8));
					fprintf(fsend,"xycompare 0x%X ' 0x%X\r\n",arg1,arg2);
					break;
				case CRY_GIVEPOKE:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fread(&arg3,1,1,fileM);
					fread(&arg4,1,1,fileM);
					if (arg4==1)
					{
						fread(&arg5,1,2,fileM);
						fread(&arg6,1,2,fileM);
						arg7=(OffsetToPointer(FileZoomPos)&0xFF);
						fprintf(fsend,"givepoke 0x%X 0x%X 0x%X 0x%X ' 0x%X,0x%X\r\n",arg1,arg2,arg3,arg4,arg5,arg6,
										PointerToOffset(arg7|(arg5<<8)),
										PointerToOffset(arg7|(arg6<<8)));
					}
					else
					{
						fprintf(fsend,"givepoke 0x%X 0x%X 0x%X 0x%X\r\n",arg1,arg2,arg3,arg4);
					}
					break;
				case CRY_LOADMENUDATA:
					fread(&arg1,1,2,fileM);
					arg2=PointerToOffset((OffsetToPointer(FileZoomPos)&0xFF)|(arg1<<8));
					fprintf(fsend,"loadmenudata 0x%X ' 0x%X\r\n",arg1,arg2);
					break;
				case CRY_LOADPOKEDATA:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					fprintf(fsend,"loadpokedata 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case CRY_CHECKCODE:
					fread(&arg1,1,1,fileM);
					//fread(&arg2,1,1,fileM);
					//fprintf(fsend,"checkcode 0x%X 0x%X\r\n",arg1,arg2);
					fprintf(fsend,"checkcode 0x%X\r\n",arg1);
					break;
				case CRY_WRITEBYTE:
					fread(&arg1,1,2,fileM);
					fread(&arg2,1,1,fileM);
					fprintf(fsend,"writebyte 0x%X 0x%X\r\n",arg1,arg2);
					break;
				case CRY_WRITEVARCODE:
					fread(&arg1,1,1,fileM);
					fprintf(fsend,"writevarcode 0x%X\r\n",arg1);
					break;
				case CRY_WRITECODE:
					fread(&arg1,1,1,fileM);
					fread(&arg2,1,1,fileM);
					if (arg1==03)
					{
						*buf2=0;
						switch (arg2)
						{
						case 0:
						case 1:
						case 5:
						case 9:
							strcpy(buf2," ' Normal fight");
							break;
						case 2:
							strcpy(buf2," ' Fight with HIRO's backpic");
							break;
						case 3:
							strcpy(buf2," ' Fight with DUDE's backpic");
							break;
						case 4:
							strcpy(buf2," ' Fight pokemon caught with rod");
							break;
						case 6:
							strcpy(buf2," ' Instant win fight");
							break;
						case 7:
							strcpy(buf2," ' Shiny pokemon fight");
							break;
						case 8:
							strcpy(buf2," ' Tree pokemon fight");
							break;
						}
						fprintf(fsend,"writecode 0x3 0x%X%s\r\n",arg2,buf2);
					}
					else
					{
						fprintf(fsend,"writecode 0x%X 0x%X\r\n",arg1,arg2);
					}
					break;
				default:
					fprintf(fsend,"#raw 0x%X\r\n",command);
					break;
				}
			}
			else
			{
				puts("'--EOF--");
				still_going=0;
			}
		}
	}
	fprintf(fsend,"\r\n");
}

void DecodeProcLevel(FILE*fileM,
										 unsigned int FileZoomPos,
										 char*fname
#ifndef DLL
										 ,FILE*fsend
#endif
										 )
{
	register unsigned int arg1,failsafe;
	unsigned int arg2,arg3;
	register char*m;
	char buf[1024];
	initDoneProcs();
	if(dyndec)fprintf(fsend,"#dynamic 0x%X\r\n",dynplace);
	if (VersionOverride)
	{
		switch (mode)
		{
		case RUBY:
			fprintf(fsend,"#rse\r\n\r\n");
			break;
		case FIRE_RED:
			fprintf(fsend,"#frlg\r\n\r\n");
			break;
		case DIAMOND:
			fprintf(fsend,"#dp\r\n\r\n");
			break;
		case CRYSTAL:
			fprintf(fsend,"#c\r\n\r\n");
			break;
		case GOLD:
			fprintf(fsend,"#gs\r\n\r\n");
			break;
		default:
			fprintf(fsend,"#frlg 'Unknown mode override ID 0x%X\r\n\r\n",mode);
		}
	}
	fseek(fileM,FileZoomPos&0x07FFFFFF,SEEK_SET);
	arg2=1;
	if(dyndec)
	{
		codenum=1;
		Define2(FileZoomPos|0x08000000,"@start");
		fprintf(fsend,"#org @start\r\n'-----------------------------------\r\n");
	}
	else
		fprintf(fsend,"#org 0x%X\r\n'-----------------------------------\r\n",FileZoomPos&0x07FFFFFF);
	while (arg2)
	{
		fread(&arg2,1,1,fileM);
		if (arg2)
		{
			fread(&arg3,1,4,fileM);
			fprintf(fsend,"#raw 0x%02X\r\n#dword 0x%X\r\n",arg2,arg3);
			if (arg2&1)//hack? I don't know. Only time will tell.
			{
				Do(arg3);
			}
			else
			{
				DoLevel(arg3);
			}
		}
		else
		{
			fprintf(fsend,"#raw 0x00\r\n");
		}
	}
	fprintf(fsend,"\r\n");
	while (!AllDoneLevel())
	{
		arg1=DoneLevel(FindNotDoneLevel());
		fseek(fileM,arg1&0x07FFFFFF,SEEK_SET);
		if(dyndec)
		{
			if(!Defined2(arg1|0x08000000))
			{
				levelnum++;
				sprintf(buf,"@level%u",levelnum);
				Define2(arg1|0x08000000,buf);
			}
			fprintf(fsend,"#org %s\r\n'-----------------------------------\r\n",WhatIs2(arg1|0x08000000));
		}
		else
			fprintf(fsend,"#org 0x%X\r\n'-----------------------------------\r\n",arg1);
		arg2=arg3=0;
		fread(&arg2,1,2,fileM);
		fread(&arg3,1,2,fileM);
		fprintf(fsend,"#word 0x%X\r\n#word 0x%X\r\n",arg2,arg3);
		fread(&arg2,1,4,fileM);
		fread(&arg3,1,2,fileM);
		fprintf(fsend,"#dword 0x%X\r\n#word 0x%X\r\n\r\n",arg2,arg3);
		Do(arg2);
	}
	while (!AllDone())
	{
		DecodeProc2(fileM,0,Done(FindNotDone()),fname
#ifndef DLL
								,fsend
#endif
							 );
	}
#define nl() fprintf(fsend,"\r\n")
	while (!AllDoneText())
	{
		nl();
		arg1=DoneText(FindNotDoneText());
		if (mode!=GOLD)
		{
			if(dyndec)
			{
				if(!Defined2(arg1|0x08000000))
				{
					textnum++;
					sprintf(buf,"@text%u",textnum);
					Define2(arg1|0x08000000,buf);
				}
				fprintf(fsend,"#org %s\r\n= %s\r\n",WhatIs2(arg1|0x08000000),transtxt(arg1&0x07ffffff,fname));
			}
			else
				fprintf(fsend,"#org 0x%X\r\n= %s\r\n",arg1,transtxt(arg1&0x07ffffff,fname));
		}
		else
			fprintf(fsend,"#org 0x%X\r\n%s\r\n",arg1,transtxt(arg1,fname));
	}
	while (!AllDoneMove())
	{
		nl();
		arg1=DoneMove(FindNotDoneMove());
		if(dyndec)
		{
			if(!Defined2(arg1|0x08000000))
			{
				movenum++;
				sprintf(buf,"@move%u",movenum);
				Define2(arg1|0x08000000,buf);
			}
			fprintf(fsend,"#org %s\r\nM %s\r\n",WhatIs2(arg1|0x08000000),transmove(arg1&0x07ffffff,fname));
		}
		else
			fprintf(fsend,"#org 0x%X\r\nM %s\r\n",arg1,transmove(arg1&0x07ffffff,fname));
	}
	while (!AllDoneMart())
	{
		nl();
		arg1=DoneMart(FindNotDoneMart());
		fseek(fileM,arg1&0x07FFFFFF,SEEK_SET);
		if(dyndec)
		{
			if(!Defined2(arg1|0x08000000))
			{
				martnum++;
				sprintf(buf,"@mart%u",martnum);
				Define2(arg1|0x08000000,buf);
			}
			fprintf(fsend,"#org %s\r\n",WhatIs2(arg1|0x08000000));
		}
		else
			fprintf(fsend,"#org 0x%X\r\n",arg1);
		arg2=1;
		while (arg2!=0)
		{
			fread(&arg2,1,2,fileM);
			if (arg2==0)
				fprintf(fsend,"endmart\r\n");
			else
			{
				m=GetItemName(arg2);
				if (!m)
					fprintf(fsend,"item 0x%X\r\n",arg2);
				else
					fprintf(fsend,"item %s\r\n",m);
			}
		}
		fprintf(fsend,"\r\n");
	}
	while (!AllDoneThumb())
	{
		nl();
		arg1=DoneThumb(FindNotDoneThumb());
		if(dyndec)
		{
			if(!Defined2(arg1|0x08000000))
			{
				thumbnum++;
				sprintf(buf,"@thumb%u",thumbnum);
				Define2(arg1|0x08000000,buf);
			}
			fprintf(fsend,"#org %s\r\n#thumb\r\n",WhatIs2(arg1|0x08000000));
		}
		else
			fprintf(fsend,"#org 0x%X\r\n#thumb\r\n",arg1);
		fseek(fileM,arg1&0x07FFFFFF,SEEK_SET);
		arg2=1;
		arg3=0;
		failsafe=0;
		while (arg2)
		{
			fread(&arg3,1,2,fileM);
			if(arg3==0||arg3==0xFF){failsafe++;
			if(failsafe>=100){fprintf(fsend,"'Failsafe\r\n");break;}}else failsafe=0;
			arg2=dec_thumb(arg3,arg1);
			fprintf(fsend,"%s\r\n",asm_buf);
			arg1+=2;
		}
		fprintf(fsend,"\r\n");
	}
	while (!AllDoneDword())
	{
		nl();
		arg1=DoneDword(FindNotDoneDword());
		if(dyndec)
		{
			if(!Defined2(arg1|0x08000000))
			{
				dwordnum++;
				sprintf(buf,"@dword%u",dwordnum);
				Define2(arg1|0x08000000,buf);
			}
			fprintf(fsend,"#org %s\r\n",WhatIs2(arg1|0x08000000));
		}
		else
			fprintf(fsend,"#org 0x%X\r\n",arg1);
		fseek(fileM,arg1&0x07FFFFFF,SEEK_SET);
		fread(&arg2,1,4,fileM);
		fprintf(fsend,"#dword 0x%X\r\n",arg2);
	}
}

void DecodeProc(FILE*fileM,
								unsigned int narc,
								unsigned int FileZoomPos,
								char*fname
#ifndef DLL
								,FILE*fsend
#endif
								)
{
	register unsigned int arg1,failsafe;
	unsigned int arg2,arg3;
	register char*m;
	char buf[1024];
	initDoneProcs();
	if(dyndec)fprintf(fsend,"#dynamic 0x%X\r\n",dynplace);
	if (VersionOverride)
	{
		switch (mode)
		{
		case RUBY:
			fprintf(fsend,"#rse\r\n\r\n");
			break;
		case FIRE_RED:
			fprintf(fsend,"#frlg\r\n\r\n");
			break;
		case DIAMOND:
			fprintf(fsend,"#dp\r\n\r\n");
			break;
		case CRYSTAL:
			fprintf(fsend,"#c\r\n\r\n");
			break;
		case GOLD:
			fprintf(fsend,"#gs\r\n\r\n");
			break;
		default:
			fprintf(fsend,"#frlg 'Unknown mode override ID 0x%X\r\n\r\n",mode);
		}
	}
	if(dyndec)
		fprintf(fsend,"#org 0x%X\r\njump @start ' This redirects the script to your dynamic-offset version. Remove this line if you don't want this.\r\n\r\n",FileZoomPos);
	DecodeProc2(fileM,narc,FileZoomPos,fname
#ifndef DLL
							,fsend
#endif
						 );
	while (!AllDone())
	{
		DecodeProc2(fileM,0,Done(FindNotDone()),fname
#ifndef DLL
								,fsend
#endif
							 );
	}
#define nl() fprintf(fsend,"\r\n")
	while (!AllDoneText())
	{
		nl();
		arg1=DoneText(FindNotDoneText());
		if (mode!=GOLD)
		{
			if(dyndec)
			{
				if(!Defined2(arg1|0x08000000))
				{
					textnum++;
					sprintf(buf,"@text%u",textnum);
					Define2(arg1|0x08000000,buf);
				}
				fprintf(fsend,"#org %s\r\n= %s\r\n",WhatIs2(arg1|0x08000000),transtxt(arg1&0x07ffffff,fname));
			}
			else
				fprintf(fsend,"#org 0x%X\r\n= %s\r\n",arg1,transtxt(arg1&0x07ffffff,fname));
		}
		else
			fprintf(fsend,"#org 0x%X\r\n%s\r\n",arg1,transtxt(arg1,fname));
	}
	while (!AllDoneMove())
	{
		nl();
		arg1=DoneMove(FindNotDoneMove());
		if(dyndec)
		{
			if(!Defined2(arg1|0x08000000))
			{
				movenum++;
				sprintf(buf,"@move%u",movenum);
				Define2(arg1|0x08000000,buf);
			}
			fprintf(fsend,"#org %s\r\nM %s\r\n",WhatIs2(arg1|0x08000000),transmove(arg1&0x07ffffff,fname));
		}
		else
			fprintf(fsend,"#org 0x%X\r\nM %s\r\n",arg1,transmove(arg1&0x07ffffff,fname));
	}
	while (!AllDoneMart())
	{
		nl();
		arg1=DoneMart(FindNotDoneMart());
		fseek(fileM,arg1&0x07FFFFFF,SEEK_SET);
		if(dyndec)
		{
			if(!Defined2(arg1|0x08000000))
			{
				martnum++;
				sprintf(buf,"@mart%u",martnum);
				Define2(arg1|0x08000000,buf);
			}
			fprintf(fsend,"#org %s\r\n",WhatIs2(arg1|0x08000000));
		}
		else
			fprintf(fsend,"#org 0x%X\r\n",arg1);
		arg2=1;
		while (arg2!=0)
		{
			fread(&arg2,1,2,fileM);
			if (arg2==0)
				fprintf(fsend,"endmart\r\n");
			else
			{
				m=GetItemName(arg2);
				if (!m)
					fprintf(fsend,"item 0x%X\r\n",arg2);
				else
					fprintf(fsend,"item %s\r\n",m);
			}
		}
		fprintf(fsend,"\r\n");
	}
	while (!AllDoneThumb())
	{
		nl();
		arg1=DoneThumb(FindNotDoneThumb());
		if(dyndec)
		{
			if(!Defined2(arg1|0x08000000))
			{
				thumbnum++;
				sprintf(buf,"@thumb%u",thumbnum);
				Define2(arg1|0x08000000,buf);
			}
			fprintf(fsend,"#org %s\r\n#thumb\r\n",WhatIs2(arg1|0x08000000));
		}
		else
			fprintf(fsend,"#org 0x%X\r\n#thumb\r\n",arg1);
		fseek(fileM,arg1&0x07FFFFFF,SEEK_SET);
		arg2=1;
		arg3=0;
		failsafe=0;
		while (arg2)
		{
			fread(&arg3,1,2,fileM);
			if(arg3==0||arg3==0xFF){failsafe++;
			if(failsafe>=100){fprintf(fsend,"'Failsafe\r\n");break;}}else failsafe=0;
			arg2=dec_thumb(arg3,arg1);
			fprintf(fsend,"%s\r\n",asm_buf);
			arg1+=2;
		}
		fprintf(fsend,"\r\n");
	}
	while (!AllDoneDword())
	{
		nl();
		arg1=DoneDword(FindNotDoneDword());
		if(dyndec)
		{
			if(!Defined2(arg1|0x08000000))
			{
				dwordnum++;
				sprintf(buf,"@dword%u",dwordnum);
				Define2(arg1|0x08000000,buf);
			}
			fprintf(fsend,"#org %s\r\n",WhatIs2(arg1|0x08000000));
		}
		else
			fprintf(fsend,"#org 0x%X\r\n",arg1);
		fseek(fileM,arg1&0x07FFFFFF,SEEK_SET);
		fread(&arg2,1,4,fileM);
		fprintf(fsend,"#dword 0x%X\r\n",arg2);
	}
}

void DecodeProcASM(FILE*fileM,
									 unsigned int FileZoomPos,
									 char*fname
#ifndef DLL
									 ,FILE*fsend
#endif
									 )
{
	register unsigned int arg1,failsafe;
	unsigned int arg2,arg3;
	char buf[1024];
	initDoneProcs();
	if(dyndec)fprintf(fsend,"#dynamic 0x%X\r\n",dynplace);
	if (VersionOverride)
	{
		switch (mode)
		{
		case RUBY:
			fprintf(fsend,"#rse\r\n\r\n");
			break;
		case FIRE_RED:
			fprintf(fsend,"#frlg\r\n\r\n");
			break;
		case DIAMOND:
			fprintf(fsend,"#dp\r\n\r\n");
			break;
		case CRYSTAL:
			fprintf(fsend,"#c\r\n\r\n");
			break;
		case GOLD:
			fprintf(fsend,"#gs\r\n\r\n");
			break;
		default:
			fprintf(fsend,"#frlg 'Unknown mode override ID 0x%X\r\n\r\n",mode);
		}
	}
	if (FileZoomPos&1)DoThumb(FileZoomPos&0xFFFFFFFE);
#ifdef WIN32
	else MessageBox(NULL,"Only THUMB has been implemented. Please add 1 to your offset to decompile as THUMB.","Warning",0x30);//DoARM(FileZoomPos);
#endif
	while (!AllDoneThumb())
	{
		nl();
		arg1=DoneThumb(FindNotDoneThumb());
		if(dyndec)
		{
			if(!Defined2(arg1|0x08000000))
			{
				thumbnum++;
				sprintf(buf,"@thumb%u",thumbnum);
				Define2(arg1|0x08000000,buf);
			}
			fprintf(fsend,"#org %s\r\n#thumb\r\n",WhatIs2(arg1|0x08000000));
		}
		else
			fprintf(fsend,"#org 0x%X\r\n#thumb\r\n",arg1);
		fseek(fileM,arg1&0x07FFFFFF,SEEK_SET);
		arg2=1;
		arg3=0;
		failsafe=0;
		while (arg2)
		{
			fread(&arg3,1,2,fileM);
			if(arg3==0||arg3==0xFF){failsafe++;
			if(failsafe>=100){fprintf(fsend,"'Failsafe\r\n");break;}}else failsafe=0;
			arg2=dec_thumb(arg3,arg1);
			fprintf(fsend,"%s\r\n",asm_buf);
			arg1+=2;
		}
		fprintf(fsend,"\r\n");
	}
	while (!AllDoneDword())
	{
		nl();
		arg1=DoneDword(FindNotDoneDword());
		if(dyndec)
		{
			if(!Defined2(arg1|0x08000000))
			{
				dwordnum++;
				sprintf(buf,"@dword%u",dwordnum);
				Define2(arg1|0x08000000,buf);
			}
			fprintf(fsend,"#org %s\r\n",WhatIs2(arg1|0x08000000));
		}
		else
			fprintf(fsend,"#org 0x%X\r\n",arg1);
		fseek(fileM,arg1&0x07FFFFFF,SEEK_SET);
		fread(&arg2,1,4,fileM);
		fprintf(fsend,"#dword 0x%X\r\n",arg2);
	}
}

void DecodeProcText(FILE*fileM,
										unsigned int FileZoomPos,
										char*fname
#ifndef DLL
										,FILE*fsend
#endif
										)
{
	initDoneProcs();
	if(dyndec)fprintf(fsend,"#dynamic 0x%X\r\n",dynplace);
	if (VersionOverride)
	{
		switch (mode)
		{
		case RUBY:
			fprintf(fsend,"#rse\r\n\r\n");
			break;
		case FIRE_RED:
			fprintf(fsend,"#frlg\r\n\r\n");
			break;
		case DIAMOND:
			fprintf(fsend,"#dp\r\n\r\n");
			break;
		case CRYSTAL:
			fprintf(fsend,"#c\r\n\r\n");
			break;
		case GOLD:
			fprintf(fsend,"#gs\r\n\r\n");
			break;
		default:
			fprintf(fsend,"#frlg 'Unknown mode override ID 0x%X\r\n\r\n",mode);
		}
	}
	if (mode!=GOLD)
		fprintf(fsend,"#org 0x%X\r\n= %s\r\n",FileZoomPos,transtxt(FileZoomPos&0x07ffffff,fname));
	else
		fprintf(fsend,"#org 0x%X\r\n%s\r\n",FileZoomPos,transtxt(FileZoomPos&0x07ffffff,fname));
}

void DecodeProcPointer(FILE*fileM,
										unsigned int FileZoomPos,
										char*fname
#ifndef DLL
										,FILE*fsend
#endif
										)
{
	unsigned int arg2;
	initDoneProcs();
	if(dyndec)fprintf(fsend,"#dynamic 0x%X\r\n",dynplace);
	if (VersionOverride)
	{
		switch (mode)
		{
		case RUBY:
			fprintf(fsend,"#rse\r\n\r\n");
			break;
		case FIRE_RED:
			fprintf(fsend,"#frlg\r\n\r\n");
			break;
		case DIAMOND:
			fprintf(fsend,"#dp\r\n\r\n");
			break;
		case CRYSTAL:
			fprintf(fsend,"#c\r\n\r\n");
			break;
		case GOLD:
			fprintf(fsend,"#gs\r\n\r\n");
			break;
		default:
			fprintf(fsend,"#frlg 'Unknown mode override ID 0x%X\r\n\r\n",mode);
		}
	}
	fseek(fileM,FileZoomPos&0x07FFFFFF,SEEK_SET);
	fread(&arg2,1,4,fileM);
	if(arg2&0x08000000)
		fprintf(fsend,"#org 0x%X\r\n#ptr 0x%X\r\n",FileZoomPos,arg2);
	else
		fprintf(fsend,"#org 0x%X\r\n#dword 0x%X\r\n",FileZoomPos,arg2);
}

void DecodeProcMoves(FILE*fileM,
										 unsigned int FileZoomPos,
										 char*fname
#ifndef DLL
										 ,FILE*fsend
#endif
										 )
{
	initDoneProcs();
	if(dyndec)fprintf(fsend,"#dynamic 0x%X\r\n",dynplace);
	if (VersionOverride)
	{
		switch (mode)
		{
		case RUBY:
			fprintf(fsend,"#rse\r\n\r\n");
			break;
		case FIRE_RED:
			fprintf(fsend,"#frlg\r\n\r\n");
			break;
		case DIAMOND:
			fprintf(fsend,"#dp\r\n\r\n");
			break;
		case CRYSTAL:
			fprintf(fsend,"#c\r\n\r\n");
			break;
		case GOLD:
			fprintf(fsend,"#gs\r\n\r\n");
			break;
		default:
			fprintf(fsend,"#frlg 'Unknown mode override ID 0x%X\r\n\r\n",mode);
		}
	}
	if(dyndec)
		fprintf(fsend,"#org @moves\r\nM %s\r\n",transmove(FileZoomPos&0x07ffffff,fname));
	else
		fprintf(fsend,"#org 0x%X\r\nM %s\r\n",FileZoomPos,transmove(FileZoomPos&0x07ffffff,fname));
}

void DecodeProcMart(FILE*fileM,
										 unsigned int FileZoomPos,
										 char*fname
#ifndef DLL
										 ,FILE*fsend
#endif
										 )
{
	register char*m;
	int arg2;
	initDoneProcs();
	if(dyndec)fprintf(fsend,"#dynamic 0x%X\r\n",dynplace);
	if (VersionOverride)
	{
		switch (mode)
		{
		case RUBY:
			fprintf(fsend,"#rse\r\n\r\n");
			break;
		case FIRE_RED:
			fprintf(fsend,"#frlg\r\n\r\n");
			break;
		case DIAMOND:
			fprintf(fsend,"#dp\r\n\r\n");
			break;
		case CRYSTAL:
			fprintf(fsend,"#c\r\n\r\n");
			break;
		case GOLD:
			fprintf(fsend,"#gs\r\n\r\n");
			break;
		default:
			fprintf(fsend,"#frlg 'Unknown mode override ID 0x%X\r\n\r\n",mode);
		}
	}
	fseek(fileM,FileZoomPos&0x07FFFFFF,SEEK_SET);
	if(dyndec)
		fprintf(fsend,"#org @mart\r\n");
	else
		fprintf(fsend,"#org 0x%X\r\n",FileZoomPos);
	arg2=1;
	while (arg2!=0)
	{
		fread(&arg2,1,2,fileM);
		if (arg2==0)
			fprintf(fsend,"endmart\r\n");
		else
		{
			m=GetItemName(arg2);
			if (!m)
				fprintf(fsend,"item 0x%X\r\n",arg2);
			else
				fprintf(fsend,"item %s\r\n",m);
		}
	}
	fprintf(fsend,"\r\n");
}

void DecodeProcAttacks(FILE*fileM,
											 unsigned int FileZoomPos,
											 char*fname
#ifndef DLL
											 ,FILE*fsend
#endif
											 )
{
	register char*m;
	int arg2;
	initDoneProcs();
	if (VersionOverride)
	{
		switch (mode)
		{
		case RUBY:
			fprintf(fsend,"#rse\r\n\r\n");
			break;
		case FIRE_RED:
			fprintf(fsend,"#frlg\r\n\r\n");
			break;
		case DIAMOND:
			fprintf(fsend,"#dp\r\n\r\n");
			break;
		case CRYSTAL:
			fprintf(fsend,"#c\r\n\r\n");
			break;
		case GOLD:
			fprintf(fsend,"#gs\r\n\r\n");
			break;
		default:
			fprintf(fsend,"#frlg 'Unknown mode override ID 0x%X\r\n\r\n",mode);
		}
	}
	fseek(fileM,FileZoomPos&0x07FFFFFF,SEEK_SET);
	if(dyndec)
		fprintf(fsend,"#org @attacks\r\n");
	else
		fprintf(fsend,"#org 0x%X\r\n",FileZoomPos);
	while (1)
	{
		fread(&arg2,1,2,fileM);
		m=GetAttackName(arg2);
		if (!m)
		{
			fprintf(fsend,"item 0x%X\r\n",arg2);
			break;
		}
		else
			fprintf(fsend,"item %s\r\n",m);
	}
	fprintf(fsend,"\r\n");
}

