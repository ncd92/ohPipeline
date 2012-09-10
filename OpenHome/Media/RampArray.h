#ifndef HEADER_RAMP_ARRAY
#define HEADER_RAMP_ARRAY
//16 bit signed fractional numbers representing a ramp down curve over 0 to
//-60dB

namespace OpenHome {
namespace Media {

const TUint kRampArray[] = {
    0x7fffffff, 0x7f603bfb, 0x7ec0efd8, 0x7e221b79, 0x7d83bec0, 0x7ce5d98e, 0x7c486bc6, 0x7bab754a,
    0x7b0ef5fb, 0x7a72edbb, 0x79d75c6c, 0x793c41ef, 0x78a19e27, 0x780770f4, 0x776dba39, 0x76d479d7,
    0x763bafaf, 0x75a35ba4, 0x750b7d96, 0x74741568, 0x73dd22fa, 0x7346a62f, 0x72b09ee6, 0x721b0d03,
    0x7185f065, 0x70f148ef, 0x705d1681, 0x6fc958fe, 0x6f361045, 0x6ea33c39, 0x6e10dcb9, 0x6d7ef1a9,
    0x6ced7ae7, 0x6c5c7857, 0x6bcbe9d7, 0x6b3bcf4b, 0x6aac2891, 0x6a1cf58c, 0x698e361c, 0x68ffea22,
    0x6872117f, 0x67e4ac14, 0x6757b9c1, 0x66cb3a67, 0x663f2de6, 0x65b39420, 0x65286cf5, 0x649db846,
    0x641375f2, 0x6389a5db, 0x630047e2, 0x62775be5, 0x61eee1c7, 0x6166d966, 0x60df42a5, 0x60581d62,
    0x5fd1697e, 0x5f4b26da, 0x5ec55555, 0x5e3ff4d0, 0x5dbb052b, 0x5d368645, 0x5cb27800, 0x5c2eda3b,
    0x5babacd5, 0x5b28efb0, 0x5aa6a2aa, 0x5a24c5a4, 0x59a3587e, 0x59225b17, 0x58a1cd4f, 0x5821af06,
    0x57a2001c, 0x5722c06f, 0x56a3efe1, 0x56258e50, 0x55a79b9c, 0x552a17a4, 0x54ad0249, 0x54305b68,
    0x53b422e3, 0x53385897, 0x52bcfc65, 0x52420e2c, 0x51c78dca, 0x514d7b20, 0x50d3d60c, 0x505a9e6e,
    0x4fe1d424, 0x4f69770e, 0x4ef1870b, 0x4e7a03f9, 0x4e02edb7, 0x4d8c4425, 0x4d160722, 0x4ca0368b,
    0x4c2ad241, 0x4bb5da21, 0x4b414e0a, 0x4acd2ddb, 0x4a597973, 0x49e630b0, 0x49735371, 0x4900e194,
    0x488edaf8, 0x481d3f7b, 0x47ac0efb, 0x473b4957, 0x46caee6d, 0x465afe1b, 0x45eb7840, 0x457c5cba,
    0x450dab66, 0x449f6424, 0x443186d0, 0x43c41349, 0x4357096d, 0x42ea691a, 0x427e322e, 0x42126486,
    0x41a70000, 0x413c047a, 0x40d171d2, 0x406747e6, 0x3ffd8692, 0x3f942db5, 0x3f2b3d2b, 0x3ec2b4d4,
    0x3e5a948b, 0x3df2dc2e, 0x3d8b8b9b, 0x3d24a2af, 0x3cbe2148, 0x3c580741, 0x3bf25479, 0x3b8d08cd,
    0x3b282419, 0x3ac3a63c, 0x3a5f8f10, 0x39fbde75, 0x39989446, 0x3935b060, 0x38d332a0, 0x38711ae3,
    0x380f6906, 0x37ae1ce5, 0x374d365c, 0x36ecb549, 0x368c9987, 0x362ce2f4, 0x35cd916b, 0x356ea4ca,
    0x35101ceb, 0x34b1f9ac, 0x34543ae9, 0x33f6e07e, 0x3399ea46, 0x333d581f, 0x32e129e3, 0x32855f70,
    0x3229f8a0, 0x31cef550, 0x3174555b, 0x311a189e, 0x30c03ef3, 0x3066c837, 0x300db446, 0x2fb502f9,
    0x2f5cb42e, 0x2f04c7c0, 0x2ead3d89, 0x2e561566, 0x2dff4f31, 0x2da8eac6, 0x2d52e800, 0x2cfd46ba,
    0x2ca806cf, 0x2c53281a, 0x2bfeaa76, 0x2baa8dbd, 0x2b56d1cc, 0x2b03767c, 0x2ab07ba8, 0x2a5de12b,
    0x2a0ba6df, 0x29b9cca0, 0x29685246, 0x291737ae, 0x28c67cb1, 0x2876212a, 0x282624f3, 0x27d687e5,
    0x278749dc, 0x27386ab1, 0x26e9ea3e, 0x269bc85e, 0x264e04ea, 0x26009fbc, 0x25b398ae, 0x2566ef99,
    0x251aa457, 0x24ceb6c2, 0x248326b4, 0x2437f405, 0x23ed1e90, 0x23a2a62d, 0x23588ab6, 0x230ecc04,
    0x22c569f0, 0x227c6454, 0x2233bb08, 0x21eb6de5, 0x21a37cc5, 0x215be77f, 0x2114adee, 0x20cdcfe9,
    0x20874d4a, 0x204125e8, 0x1ffb599c, 0x1fb5e840, 0x1f70d1aa, 0x1f2c15b4, 0x1ee7b435, 0x1ea3ad07,
    0x1e600000, 0x1e1cacf9, 0x1dd9b3cb, 0x1d97144c, 0x1d54ce55, 0x1d12e1bd, 0x1cd14e5c, 0x1c90140a,
    0x1c4f329f, 0x1c0ea9f1, 0x1bce79d8, 0x1b8ea22c, 0x1b4f22c3, 0x1b0ffb76, 0x1ad12c1a, 0x1a92b487,
    0x1a549494, 0x1a16cc19, 0x19d95aea, 0x199c40e1, 0x195f7dd2, 0x19231195, 0x18e6fbff, 0x18ab3ce9,
    0x186fd427, 0x1834c191, 0x17fa04fb, 0x17bf9e3d, 0x17858d2d, 0x174bd1a0, 0x17126b6c, 0x16d95a67,
    0x16a09e66, 0x16683741, 0x163024cb, 0x15f866da, 0x15c0fd44, 0x1589e7dd, 0x1553267c, 0x151cb8f6,
    0x14e69f1e, 0x14b0d8cb, 0x147b65d0, 0x14464604, 0x1411793a, 0x13dcff47, 0x13a8d800, 0x13750339,
    0x134180c6, 0x130e507b, 0x12db722e, 0x12a8e5b1, 0x1276aad8, 0x1244c178, 0x12132965, 0x11e1e271,
    0x11b0ec71, 0x11804738, 0x114ff29a, 0x111fee69, 0x10f03a79, 0x10c0d69d, 0x1091c2a8, 0x1062fe6d,
    0x103489be, 0x1006646f, 0x0fd88e52, 0x0fab0739, 0x0f7dcef7, 0x0f50e55e, 0x0f244a41, 0x0ef7fd71,
    0x0ecbfec0, 0x0ea04e00, 0x0e74eb04, 0x0e49d59c, 0x0e1f0d9a, 0x0df492d0, 0x0dca650f, 0x0da08428,
    0x0d76efec, 0x0d4da82c, 0x0d24acba, 0x0cfbfd65, 0x0cd399ff, 0x0cab8258, 0x0c83b640, 0x0c5c3588,
    0x0c350000, 0x0c0e1578, 0x0be775c0, 0x0bc120a8, 0x0b9b15ff, 0x0b755595, 0x0b4fdf39, 0x0b2ab2bc,
    0x0b05cfeb, 0x0ae13697, 0x0abce68d, 0x0a98df9e, 0x0a752196, 0x0a51ac46, 0x0a2e7f7c, 0x0a0b9b05,
    0x09e8feb0, 0x09c6aa4b, 0x09a49da4, 0x0982d889, 0x09615ac7, 0x0940242c, 0x091f3484, 0x08fe8b9e,
    0x08de2946, 0x08be0d4a, 0x089e3775, 0x087ea795, 0x085f5d76, 0x084058e4, 0x082199ab, 0x08031f99,
    0x07e4ea77, 0x07c6fa13, 0x07a94e37, 0x078be6af, 0x076ec347, 0x0751e3c9, 0x07354800, 0x0718efb7,
    0x06fcdab9, 0x06e108cf, 0x06c579c5, 0x06aa2d65, 0x068f2377, 0x06745bc6, 0x0659d61c, 0x063f9242,
    0x06259001, 0x060bcf22, 0x05f24f6f, 0x05d910ae, 0x05c012aa, 0x05a7552b, 0x058ed7f8, 0x05769ad9,
    0x055e9d97, 0x0546dff8, 0x052f61c5, 0x051822c3, 0x050122bb, 0x04ea6173, 0x04d3deb1, 0x04bd9a3b,
    0x04a793d9, 0x0491cb4f, 0x047c4064, 0x0466f2dc, 0x0451e27e, 0x043d0f0d, 0x04287850, 0x04141e0a,
    0x04000000, 0x03ec1df6, 0x03d877b0, 0x03c50cf1, 0x03b1dd7d, 0x039ee918, 0x038c2f83, 0x0379b082,
    0x03676bd7, 0x03556145, 0x0343908c, 0x0331f96f, 0x03209bb0, 0x030f770e, 0x02fe8b4c, 0x02edd829,
    0x02dd5d67, 0x02cd1ac4, 0x02bd1001, 0x02ad3cdd, 0x029da117, 0x028e3c6e, 0x027f0ea1, 0x0270176e,
    0x02615692, 0x0252cbcc, 0x024476d8, 0x02365773, 0x02286d5b, 0x021ab84b, 0x020d3800, 0x01ffec35,
    0x01f2d4a4, 0x01e5f10a, 0x01d94121, 0x01ccc4a2, 0x01c07b48, 0x01b464cc, 0x01a880e7, 0x019ccf52,
    0x01914fc5, 0x018601f8, 0x017ae5a1, 0x016ffa7a, 0x01654036, 0x015ab68e, 0x01505d37, 0x014633e6,
    0x013c3a4f, 0x01327027, 0x0128d523, 0x011f68f4, 0x01162b50, 0x010d1be6, 0x01043a6a, 0x00fb868d,
    0x00f30000, 0x00eaa673, 0x00e27995, 0x00da7916, 0x00d2a4a5, 0x00cafbef, 0x00c37ea1, 0x00bc2c69,
    0x00b504f3, 0x00ae07ea, 0x00a734f9, 0x00a08bca, 0x009a0c06, 0x0093b557, 0x008d8764, 0x008781d4,
    0x0081a44e, 0x007bee78, 0x00765ff6, 0x0070f86d, 0x006bb77f, 0x00669cd0, 0x0061a800, 0x005cd8b0,
    0x00582e7f, 0x0053a90d, 0x004f47f6, 0x004b0ad6, 0x0046f14a, 0x0042faec, 0x003f2754, 0x003b761a,
    0x0037e6d6, 0x0034791c, 0x00312c80, 0x002e0095, 0x002af4ed, 0x00280916, 0x00253c9f, 0x00228f14,
    0x00200000, 0x001d8eec, 0x001b3b5f, 0x001904dd, 0x0016eaeb, 0x0014ed09, 0x00130ab5, 0x0011436b,
    0x000f96a5, 0x000e03da, 0x000c8a7e, 0x000b2a02, 0x0009e1d2, 0x0008b15a, 0x00079800, 0x00069525,
    0x0005a828, 0x0004d060, 0x00040d22, 0x00035dbc, 0x0002c174, 0x0002378a, 0x0001bf37, 0x000157a7,
    0x00010000, 0x0000b757, 0x00007cb5, 0x00004f0f, 0x00002d41, 0x0000160c, 0x00000800, 0x0000016a
};

const TUint kRampArrayCount = sizeof(kRampArray) / sizeof(kRampArray[0]);

} //namespace Media
} //namespace OpenHome

/*
    The above array values were created using the following matlab program:

        close all;
        k=0.9865;       % constant for modifying logarithmic slope
        n=512;          % number of steps in time vector
        t = 1:1:n;      % define time vector
 
        y = k.^t;                   % log ramp
        lin = ((n-(t-1))/n);        % linear ramp
        z = ((n-(t-1))/n).^2.5;     % somewhere in-between ramp

        % calculate ramp response in dB
        logy=20*log10(y);
        loglin=20*log10(lin);
        logz=20*log10(z);

        % plot all three ramps
        plot(t,logy,t,loglin,t,logz);
        grid on;
        axis([0 n -60 0]);

        % define a 32 bit quantiser
        fixed_32 = quantizer;
        set(fixed_32, 'mode', 'fixed', ...
            'roundmode', 'round', ...
            'overflowmode', 'saturate', ...
            'format', [32 31]);

        % define a 24 bit quantiser
        fixed_24 = quantizer;
        set(fixed_24, 'mode', 'fixed', ...
            'roundmode', 'round', ...
            'overflowmode', 'saturate', ...
            'format', [24 23]);
    
        % define a 16 bit quantiser
        fixed_16 = quantizer;
        set(fixed_16, 'mode', 'fixed', ...
            'roundmode', 'round', ...
            'overflowmode', 'saturate', ...
            'format', [16 15]);    

        % quantise the in-between slope to 32-bits
        yq = quantize(fixed_32, z);

        % scale to 1.31 fractional binary values
        yq_scaled = yq*(2^(31));

        %plot(20*log10(yq_scaled));

        % write 16-bit ramp coefficients to text file
        fid = fopen('ramp_32.txt', 'wt');
        fprintf(fid, '%12d\n', yq_scaled);
        fclose(fid);

    This outputs decimal values.  These were converted to hex using the following python script:

        fh = open('c:\\temp\\ramp_32.txt', 'rb')
        lines = fh.readlines()
        fh.close()
        fh = open('c:\\temp\\ramp_32_hex.txt', 'wb')
        count = 0
        fh.write("    ")
        for line in lines:
            fh.write("0x%08x," % int(line))
            count += 1
            if count % 8 == 0:
                fh.write("\n    ")
            else:
                fh.write(" ")
        fh.close()

*/

#endif // HEADER_RAMP_ARRAY
