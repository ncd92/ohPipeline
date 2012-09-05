#ifndef HEADER_RAMP_ARRAY
#define HEADER_RAMP_ARRAY
//16 bit signed fractional numbers representing a ramp down curve over 0 to
//-60dB

namespace OpenHome {
namespace Media {

const TInt16 kRampArray[] = {
0x7FFF, 0x7F60, 0x7EC1, 0x7E22, 
0x7D84, 0x7CE6, 0x7C48, 0x7BAB,
0x7B0F, 0x7A73, 0x79D7, 0x793C,
0x78A2, 0x7807, 0x776E, 0x76D4,
0x763C, 0x75A3, 0x750B, 0x7474,
0x73DD, 0x7347, 0x72B1, 0x721B,
0x7186, 0x70F1, 0x705D, 0x6FC9,
0x6F36, 0x6EA3, 0x6E11, 0x6D7F,
0x6CED, 0x6C5C, 0x6BCC, 0x6B3C,
0x6AAC, 0x6A1D, 0x698E, 0x6900,
0x6872, 0x67E5, 0x6758, 0x66CB,
0x663F, 0x65B4, 0x6528, 0x649E,
0x6413, 0x638A, 0x6300, 0x6277,
0x61EF, 0x6167, 0x60DF, 0x6058,
0x5FD1, 0x5F4B, 0x5EC5, 0x5E40,
0x5DBB, 0x5D37, 0x5CB2, 0x5C2F,
0x5BAC, 0x5B29, 0x5AA7, 0x5A25,
0x59A3, 0x5922, 0x58A2, 0x5822,
0x57A2, 0x5723, 0x56A4, 0x5626,
0x55A8, 0x552A, 0x54AD, 0x5430,
0x53B4, 0x5338, 0x52BD, 0x5242,
0x51C8, 0x514D, 0x50D4, 0x505B,
0x4FE2, 0x4F69, 0x4EF2, 0x4E7A,
0x4E03, 0x4D8C, 0x4D16, 0x4CA0,
0x4C2B, 0x4BB6, 0x4B41, 0x4ACD,
0x4A59, 0x49E6, 0x4973, 0x4901,
0x488F, 0x481D, 0x47AC, 0x473B,
0x46CB, 0x465B, 0x45EB, 0x457C,
0x450E, 0x449F, 0x4432, 0x43C4,
0x4357, 0x42EA, 0x427E, 0x4212,
0x41A7, 0x413C, 0x40D1, 0x4067,
0x3FFE, 0x3F94, 0x3F2B, 0x3EC3,
0x3E5B, 0x3DF3, 0x3D8C, 0x3D25,
0x3CBE, 0x3C58, 0x3BF2, 0x3B8D,
0x3B28, 0x3AC4, 0x3A60, 0x39FC,
0x3999, 0x3936, 0x38D3, 0x3871,
0x380F, 0x37AE, 0x374D, 0x36ED,
0x368D, 0x362D, 0x35CE, 0x356F,
0x3510, 0x34B2, 0x3454, 0x33F7,
0x339A, 0x333D, 0x32E1, 0x3285,
0x322A, 0x31CF, 0x3174, 0x311A,
0x30C0, 0x3067, 0x300E, 0x2FB5,
0x2F5D, 0x2F05, 0x2EAD, 0x2E56,
0x2DFF, 0x2DA9, 0x2D53, 0x2CFD,
0x2CA8, 0x2C53, 0x2BFF, 0x2BAB,
0x2B57, 0x2B03, 0x2AB0, 0x2A5E,
0x2A0C, 0x29BA, 0x2968, 0x2917,
0x28C6, 0x2876, 0x2826, 0x27D7,
0x2787, 0x2738, 0x26EA, 0x269C,
0x264E, 0x2601, 0x25B4, 0x2567,
0x251B, 0x24CF, 0x2483, 0x2438,
0x23ED, 0x23A3, 0x2359, 0x230F,
0x22C5, 0x227C, 0x2234, 0x21EB,
0x21A3, 0x215C, 0x2115, 0x20CE,
0x2087, 0x2041, 0x1FFB, 0x1FB6,
0x1F71, 0x1F2C, 0x1EE8, 0x1EA4,
0x1E60, 0x1E1D, 0x1DDA, 0x1D97,
0x1D55, 0x1D13, 0x1CD1, 0x1C90,
0x1C4F, 0x1C0F, 0x1BCE, 0x1B8F,
0x1B4F, 0x1B10, 0x1AD1, 0x1A93,
0x1A55, 0x1A17, 0x19D9, 0x199C,
0x195F, 0x1923, 0x18E7, 0x18AB,
0x1870, 0x1835, 0x17FA, 0x17C0,
0x1786, 0x174C, 0x1712, 0x16D9,
0x16A1, 0x1668, 0x1630, 0x15F8,
0x15C1, 0x158A, 0x1553, 0x151D,
0x14E7, 0x14B1, 0x147B, 0x1446,
0x1411, 0x13DD, 0x13A9, 0x1375,
0x1342, 0x130E, 0x12DB, 0x12A9,
0x1277, 0x1245, 0x1213, 0x11E2,
0x11B1, 0x1180, 0x1150, 0x1120,
0x10F0, 0x10C1, 0x1092, 0x1063,
0x1035, 0x1006, 0x0FD9, 0x0FAB,
0x0F7E, 0x0F51, 0x0F24, 0x0EF8,
0x0ECC, 0x0EA0, 0x0E75, 0x0E4A,
0x0E1F, 0x0DF5, 0x0DCA, 0x0DA1,
0x0D77, 0x0D4E, 0x0D25, 0x0CFC,
0x0CD4, 0x0CAC, 0x0C84, 0x0C5C,
0x0C35, 0x0C0E, 0x0BE7, 0x0BC1,
0x0B9B, 0x0B75, 0x0B50, 0x0B2B,
0x0B06, 0x0AE1, 0x0ABD, 0x0A99,
0x0A75, 0x0A52, 0x0A2E, 0x0A0C,
0x09E9, 0x09C7, 0x09A5, 0x0983,
0x0961, 0x0940, 0x091F, 0x08FF,
0x08DE, 0x08BE, 0x089E, 0x087F,
0x085F, 0x0840, 0x0822, 0x0803,
0x07E5, 0x07C7, 0x07A9, 0x078C,
0x076F, 0x0752, 0x0735, 0x0719,
0x06FD, 0x06E1, 0x06C5, 0x06AA,
0x068F, 0x0674, 0x065A, 0x0640,
0x0626, 0x060C, 0x05F2, 0x05D9,
0x05C0, 0x05A7, 0x058F, 0x0577,
0x055F, 0x0547, 0x052F, 0x0518,
0x0501, 0x04EA, 0x04D4, 0x04BE,
0x04A8, 0x0492, 0x047C, 0x0467,
0x0452, 0x043D, 0x0428, 0x0414,
0x0400, 0x03EC, 0x03D8, 0x03C5,
0x03B2, 0x039F, 0x038C, 0x037A,
0x0367, 0x0355, 0x0344, 0x0332,
0x0321, 0x030F, 0x02FF, 0x02EE,
0x02DD, 0x02CD, 0x02BD, 0x02AD,
0x029E, 0x028E, 0x027F, 0x0270,
0x0261, 0x0253, 0x0244, 0x0236,
0x0228, 0x021B, 0x020D, 0x0200,
0x01F3, 0x01E6, 0x01D9, 0x01CD,
0x01C0, 0x01B4, 0x01A9, 0x019D,
0x0191, 0x0186, 0x017B, 0x0170,
0x0165, 0x015B, 0x0150, 0x0146,
0x013C, 0x0132, 0x0129, 0x011F,
0x0116, 0x010D, 0x0104, 0x00FC,
0x00F3, 0x00EB, 0x00E2, 0x00DA,
0x00D3, 0x00CB, 0x00C3, 0x00BC,
0x00B5, 0x00AE, 0x00A7, 0x00A1,
0x009A, 0x0094, 0x008E, 0x0088,
0x0082, 0x007C, 0x0076, 0x0071,
0x006C, 0x0067, 0x0062, 0x005D,
0x0058, 0x0054, 0x004F, 0x004B,
0x0047, 0x0043, 0x003F, 0x003B,
0x0038, 0x0034, 0x0031, 0x002E,
0x002B, 0x0028, 0x0025, 0x0023,
0x0020, 0x001E, 0x001B, 0x0019,
0x0017, 0x0015, 0x0013, 0x0011,
0x0010, 0x000E, 0x000D, 0x000B,
0x000A, 0x0009, 0x0008, 0x0007,
0x0006, 0x0005, 0x0004, 0x0003,
0x0003, 0x0002, 0x0002, 0x0001,
0x0001, 0x0001, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000,
};

const TUint kRampArrayCount = sizeof(kRampArray) / sizeof(kRampArray[0]);

} //namespace Media
} //namespace OpenHome

#endif // HEADER_RAMP_ARRAY
