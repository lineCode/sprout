#pragma once

#include "simd.hpp"
#include "math/vector4.hpp"
#include "memory/const.hpp"

namespace simd {

//------------------------------------------------------------------------------
// Conversion types for constants

struct alignas(16) Vector_f32 {
	union {
		float f[4];
		Vector v;
	};

	inline operator Vector() const { return v; }
	inline operator __m128i() const { return _mm_castps_si128(v); }
	inline operator __m128d() const { return _mm_castps_pd(v); }
};

struct alignas(16) Vector_i32 {
	union {
		int32_t i[4];
		Vector v;
	};

	inline operator Vector() const { return v; }
	inline operator __m128i() const { return _mm_castps_si128(v); }
	inline operator __m128d() const { return _mm_castps_pd(v); }
};

struct alignas(16) Vector_u32 {
	union {
		uint32_t u[4];
		Vector v;
	};

	inline operator Vector() const { return v; }
	inline operator __m128i() const { return _mm_castps_si128(v); }
	inline operator __m128d() const { return _mm_castps_pd(v); }
};

//XMGLOBALCONST XMVECTORF32 g_XMSinCoefficients0    = {-0.16666667f, +0.0083333310f, -0.00019840874f, +2.7525562e-06f};
//XMGLOBALCONST XMVECTORF32 g_XMSinCoefficients1    = {-2.3889859e-08f, -0.16665852f /*Est1*/, +0.0083139502f /*Est2*/, -0.00018524670f /*Est3*/};
//XMGLOBALCONST XMVECTORF32 g_XMCosCoefficients0    = {-0.5f, +0.041666638f, -0.0013888378f, +2.4760495e-05f};
//XMGLOBALCONST XMVECTORF32 g_XMCosCoefficients1    = {-2.6051615e-07f, -0.49992746f /*Est1*/, +0.041493919f /*Est2*/, -0.0012712436f /*Est3*/};
//XMGLOBALCONST XMVECTORF32 g_XMTanCoefficients0    = {1.0f, 0.333333333f, 0.133333333f, 5.396825397e-2f};
//XMGLOBALCONST XMVECTORF32 g_XMTanCoefficients1    = {2.186948854e-2f, 8.863235530e-3f, 3.592128167e-3f, 1.455834485e-3f};
//XMGLOBALCONST XMVECTORF32 g_XMTanCoefficients2    = {5.900274264e-4f, 2.391290764e-4f, 9.691537707e-5f, 3.927832950e-5f};
//XMGLOBALCONST XMVECTORF32 g_XMArcCoefficients0    = {+1.5707963050f, -0.2145988016f, +0.0889789874f, -0.0501743046f};
//XMGLOBALCONST XMVECTORF32 g_XMArcCoefficients1    = {+0.0308918810f, -0.0170881256f, +0.0066700901f, -0.0012624911f};
//XMGLOBALCONST XMVECTORF32 g_XMATanCoefficients0   = {-0.3333314528f, +0.1999355085f, -0.1420889944f, +0.1065626393f};
//XMGLOBALCONST XMVECTORF32 g_XMATanCoefficients1   = {-0.0752896400f, +0.0429096138f, -0.0161657367f, +0.0028662257f};
//XMGLOBALCONST XMVECTORF32 g_XMATanEstCoefficients0 = {+0.999866f, +0.999866f, +0.999866f, +0.999866f};
//XMGLOBALCONST XMVECTORF32 g_XMATanEstCoefficients1 = {-0.3302995f, +0.180141f, -0.085133f, +0.0208351f};
//XMGLOBALCONST XMVECTORF32 g_XMTanEstCoefficients  = {2.484f, -1.954923183e-1f, 2.467401101f, XM_1DIVPI};
//XMGLOBALCONST XMVECTORF32 g_XMArcEstCoefficients  = {+1.5707288f,-0.2121144f,+0.0742610f,-0.0187293f};
//XMGLOBALCONST XMVECTORF32 g_XMPiConstants0        = {XM_PI, XM_2PI, XM_1DIVPI, XM_1DIV2PI};
//XMGLOBALCONST XMVECTORF32 g_XMIdentityR0          = {1.0f, 0.0f, 0.0f, 0.0f};
//XMGLOBALCONST XMVECTORF32 g_XMIdentityR1          = {0.0f, 1.0f, 0.0f, 0.0f};
//XMGLOBALCONST XMVECTORF32 g_XMIdentityR2          = {0.0f, 0.0f, 1.0f, 0.0f};
//XMGLOBALCONST XMVECTORF32 g_XMIdentityR3          = {0.0f, 0.0f, 0.0f, 1.0f};
//XMGLOBALCONST XMVECTORF32 g_XMNegIdentityR0       = {-1.0f,0.0f, 0.0f, 0.0f};
//XMGLOBALCONST XMVECTORF32 g_XMNegIdentityR1       = {0.0f,-1.0f, 0.0f, 0.0f};
//XMGLOBALCONST XMVECTORF32 g_XMNegIdentityR2       = {0.0f, 0.0f,-1.0f, 0.0f};
//XMGLOBALCONST XMVECTORF32 g_XMNegIdentityR3       = {0.0f, 0.0f, 0.0f,-1.0f};
//XMGLOBALCONST XMVECTORU32 g_XMNegativeZero      = {0x80000000, 0x80000000, 0x80000000, 0x80000000};
//XMGLOBALCONST XMVECTORU32 g_XMNegate3           = {0x80000000, 0x80000000, 0x80000000, 0x00000000};

SU_GLOBALCONST(Vector_u32) Mask3				= {{{ 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000 }}};

SU_GLOBALCONST(Vector_u32) Sign_mask			= {{{  0x80000000,  0x80000000,  0x80000000,  0x80000000 }}};
SU_GLOBALCONST(Vector_u32) Inverse_sign_mask	= {{{ ~0x80000000, ~0x80000000, ~0x80000000, ~0x80000000 }}};

//XMGLOBALCONST XMVECTORU32 g_XMMaskX             = {0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000};
//XMGLOBALCONST XMVECTORU32 g_XMMaskY             = {0x00000000, 0xFFFFFFFF, 0x00000000, 0x00000000};
//XMGLOBALCONST XMVECTORU32 g_XMMaskZ             = {0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000};
//XMGLOBALCONST XMVECTORU32 g_XMMaskW             = {0x00000000, 0x00000000, 0x00000000, 0xFFFFFFFF};
SU_GLOBALCONST(Vector_f32) One					= {{{ 1.f, 1.f, 1.f, 1.f }}};
SU_GLOBALCONST(Vector_i32) One_i				= {{{ 1, 1, 1, 1 }}};
SU_GLOBALCONST(Vector_i32) Inverse_one_i		= {{{ ~1, ~1, ~1, ~1 }}};
SU_GLOBALCONST(Vector_i32) Two_i				= {{{ 2, 2, 2, 2 }}};
SU_GLOBALCONST(Vector_i32) Four_i				= {{{ 4, 4, 4, 4 }}};
//XMGLOBALCONST XMVECTORF32 g_XMOne3              = { 1.0f, 1.0f, 1.0f, 0.0f};
SU_GLOBALCONST(Vector_f32) Zero					= {{{ 0.f, 0.f, 0.f, 0.f }}};
//XMGLOBALCONST XMVECTORF32 g_XMTwo               = { 2.f, 2.f, 2.f, 2.f };
//XMGLOBALCONST XMVECTORF32 g_XMFour              = { 4.f, 4.f, 4.f, 4.f };
//XMGLOBALCONST XMVECTORF32 g_XMSix               = { 6.f, 6.f, 6.f, 6.f };
//XMGLOBALCONST XMVECTORF32 g_XMNegativeOne       = {-1.0f,-1.0f,-1.0f,-1.0f};
SU_GLOBALCONST(Vector_f32) Half					= {{{ 0.5f, 0.5f, 0.5f, 0.5f }}};
//XMGLOBALCONST XMVECTORF32 g_XMNegativeOneHalf   = {-0.5f,-0.5f,-0.5f,-0.5f};
//XMGLOBALCONST XMVECTORF32 g_XMNegativeTwoPi     = {-XM_2PI, -XM_2PI, -XM_2PI, -XM_2PI};
//XMGLOBALCONST XMVECTORF32 g_XMNegativePi        = {-XM_PI, -XM_PI, -XM_PI, -XM_PI};
//XMGLOBALCONST XMVECTORF32 g_XMHalfPi            = {XM_PIDIV2, XM_PIDIV2, XM_PIDIV2, XM_PIDIV2};
//XMGLOBALCONST XMVECTORF32 g_XMPi                = {XM_PI, XM_PI, XM_PI, XM_PI};
//XMGLOBALCONST XMVECTORF32 g_XMReciprocalPi      = {XM_1DIVPI, XM_1DIVPI, XM_1DIVPI, XM_1DIVPI};
//XMGLOBALCONST XMVECTORF32 g_XMTwoPi             = {XM_2PI, XM_2PI, XM_2PI, XM_2PI};
//XMGLOBALCONST XMVECTORF32 g_XMReciprocalTwoPi   = {XM_1DIV2PI, XM_1DIV2PI, XM_1DIV2PI, XM_1DIV2PI};
//XMGLOBALCONST XMVECTORF32 g_XMEpsilon           = {1.192092896e-7f, 1.192092896e-7f, 1.192092896e-7f, 1.192092896e-7f};
SU_GLOBALCONST(Vector_i32) Infinity				= {{{ 0x7F800000, 0x7F800000, 0x7F800000, 0x7F800000 }}};
//XMGLOBALCONST XMVECTORI32 g_XMQNaN              = {0x7FC00000, 0x7FC00000, 0x7FC00000, 0x7FC00000};
//XMGLOBALCONST XMVECTORI32 g_XMQNaNTest          = {0x007FFFFF, 0x007FFFFF, 0x007FFFFF, 0x007FFFFF};
//XMGLOBALCONST XMVECTORI32 g_XMAbsMask           = {0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF};
//XMGLOBALCONST XMVECTORI32 g_XMFltMin            = {0x00800000, 0x00800000, 0x00800000, 0x00800000};
//XMGLOBALCONST XMVECTORI32 g_XMFltMax            = {0x7F7FFFFF, 0x7F7FFFFF, 0x7F7FFFFF, 0x7F7FFFFF};
//XMGLOBALCONST XMVECTORU32 g_XMNegOneMask        = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};
//XMGLOBALCONST XMVECTORU32 g_XMMaskA8R8G8B8      = {0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000};
//XMGLOBALCONST XMVECTORU32 g_XMFlipA8R8G8B8      = {0x00000000, 0x00000000, 0x00000000, 0x80000000};
//XMGLOBALCONST XMVECTORF32 g_XMFixAA8R8G8B8      = {0.0f,0.0f,0.0f,(float)(0x80000000U)};
//XMGLOBALCONST XMVECTORF32 g_XMNormalizeA8R8G8B8 = {1.0f/(255.0f*(float)(0x10000)),1.0f/(255.0f*(float)(0x100)),1.0f/255.0f,1.0f/(255.0f*(float)(0x1000000))};
//XMGLOBALCONST XMVECTORU32 g_XMMaskA2B10G10R10   = {0x000003FF, 0x000FFC00, 0x3FF00000, 0xC0000000};
//XMGLOBALCONST XMVECTORU32 g_XMFlipA2B10G10R10   = {0x00000200, 0x00080000, 0x20000000, 0x80000000};
//XMGLOBALCONST XMVECTORF32 g_XMFixAA2B10G10R10   = {-512.0f,-512.0f*(float)(0x400),-512.0f*(float)(0x100000),(float)(0x80000000U)};
//XMGLOBALCONST XMVECTORF32 g_XMNormalizeA2B10G10R10 = {1.0f/511.0f,1.0f/(511.0f*(float)(0x400)),1.0f/(511.0f*(float)(0x100000)),1.0f/(3.0f*(float)(0x40000000))};
//XMGLOBALCONST XMVECTORU32 g_XMMaskX16Y16        = {0x0000FFFF, 0xFFFF0000, 0x00000000, 0x00000000};
//XMGLOBALCONST XMVECTORI32 g_XMFlipX16Y16        = {0x00008000, 0x00000000, 0x00000000, 0x00000000};
//XMGLOBALCONST XMVECTORF32 g_XMFixX16Y16         = {-32768.0f,0.0f,0.0f,0.0f};
//XMGLOBALCONST XMVECTORF32 g_XMNormalizeX16Y16   = {1.0f/32767.0f,1.0f/(32767.0f*65536.0f),0.0f,0.0f};
//XMGLOBALCONST XMVECTORU32 g_XMMaskX16Y16Z16W16  = {0x0000FFFF, 0x0000FFFF, 0xFFFF0000, 0xFFFF0000};
//XMGLOBALCONST XMVECTORI32 g_XMFlipX16Y16Z16W16  = {0x00008000, 0x00008000, 0x00000000, 0x00000000};
//XMGLOBALCONST XMVECTORF32 g_XMFixX16Y16Z16W16   = {-32768.0f,-32768.0f,0.0f,0.0f};
//XMGLOBALCONST XMVECTORF32 g_XMNormalizeX16Y16Z16W16 = {1.0f/32767.0f,1.0f/32767.0f,1.0f/(32767.0f*65536.0f),1.0f/(32767.0f*65536.0f)};
//XMGLOBALCONST XMVECTORF32 g_XMNoFraction        = {8388608.0f,8388608.0f,8388608.0f,8388608.0f};
//XMGLOBALCONST XMVECTORI32 g_XMMaskByte          = {0x000000FF, 0x000000FF, 0x000000FF, 0x000000FF};

SU_GLOBALCONST(Vector_i32) Mask_0x7F			= {{{0x0000007F, 0x0000007F, 0x0000007F, 0x0000007F}}};

//XMGLOBALCONST XMVECTORF32 g_XMNegateX           = {-1.0f, 1.0f, 1.0f, 1.0f};
//XMGLOBALCONST XMVECTORF32 g_XMNegateY           = { 1.0f,-1.0f, 1.0f, 1.0f};
//XMGLOBALCONST XMVECTORF32 g_XMNegateZ           = { 1.0f, 1.0f,-1.0f, 1.0f};
//XMGLOBALCONST XMVECTORF32 g_XMNegateW           = { 1.0f, 1.0f, 1.0f,-1.0f};
//XMGLOBALCONST XMVECTORU32 g_XMSelect0101        = {XM_SELECT_0, XM_SELECT_1, XM_SELECT_0, XM_SELECT_1};
//XMGLOBALCONST XMVECTORU32 g_XMSelect1010        = {XM_SELECT_1, XM_SELECT_0, XM_SELECT_1, XM_SELECT_0};
//XMGLOBALCONST XMVECTORI32 g_XMOneHalfMinusEpsilon = { 0x3EFFFFFD, 0x3EFFFFFD, 0x3EFFFFFD, 0x3EFFFFFD};
//XMGLOBALCONST XMVECTORU32 g_XMSelect1000        = {XM_SELECT_1, XM_SELECT_0, XM_SELECT_0, XM_SELECT_0};
//XMGLOBALCONST XMVECTORU32 g_XMSelect1100        = {XM_SELECT_1, XM_SELECT_1, XM_SELECT_0, XM_SELECT_0};
//XMGLOBALCONST XMVECTORU32 g_XMSelect1110        = {XM_SELECT_1, XM_SELECT_1, XM_SELECT_1, XM_SELECT_0};
//XMGLOBALCONST XMVECTORU32 g_XMSelect1011          = { XM_SELECT_1, XM_SELECT_0, XM_SELECT_1, XM_SELECT_1 };
//XMGLOBALCONST XMVECTORF32 g_XMFixupY16          = {1.0f,1.0f/65536.0f,0.0f,0.0f};
//XMGLOBALCONST XMVECTORF32 g_XMFixupY16W16       = {1.0f,1.0f,1.0f/65536.0f,1.0f/65536.0f};
//XMGLOBALCONST XMVECTORU32 g_XMFlipY             = {0,0x80000000,0,0};
//XMGLOBALCONST XMVECTORU32 g_XMFlipZ             = {0,0,0x80000000,0};
//XMGLOBALCONST XMVECTORU32 g_XMFlipW             = {0,0,0,0x80000000};
//XMGLOBALCONST XMVECTORU32 g_XMFlipYZ            = {0,0x80000000,0x80000000,0};
//XMGLOBALCONST XMVECTORU32 g_XMFlipZW            = {0,0,0x80000000,0x80000000};
//XMGLOBALCONST XMVECTORU32 g_XMFlipYW            = {0,0x80000000,0,0x80000000};
//XMGLOBALCONST XMVECTORI32 g_XMMaskDec4          = {0x3FF,0x3FF<<10,0x3FF<<20,0x3<<30};
//XMGLOBALCONST XMVECTORI32 g_XMXorDec4           = {0x200,0x200<<10,0x200<<20,0};
//XMGLOBALCONST XMVECTORF32 g_XMAddUDec4          = {0,0,0,32768.0f*65536.0f};
//XMGLOBALCONST XMVECTORF32 g_XMAddDec4           = {-512.0f,-512.0f*1024.0f,-512.0f*1024.0f*1024.0f,0};
//XMGLOBALCONST XMVECTORF32 g_XMMulDec4           = {1.0f,1.0f/1024.0f,1.0f/(1024.0f*1024.0f),1.0f/(1024.0f*1024.0f*1024.0f)};
//XMGLOBALCONST XMVECTORU32 g_XMMaskByte4         = {0xFF,0xFF00,0xFF0000,0xFF000000};
//XMGLOBALCONST XMVECTORI32 g_XMXorByte4          = {0x80,0x8000,0x800000,0x00000000};
//XMGLOBALCONST XMVECTORF32 g_XMAddByte4          = {-128.0f,-128.0f*256.0f,-128.0f*65536.0f,0};
//XMGLOBALCONST XMVECTORF32 g_XMFixUnsigned       = {32768.0f*65536.0f,32768.0f*65536.0f,32768.0f*65536.0f,32768.0f*65536.0f};
//XMGLOBALCONST XMVECTORF32 g_XMMaxInt            = {65536.0f*32768.0f-128.0f,65536.0f*32768.0f-128.0f,65536.0f*32768.0f-128.0f,65536.0f*32768.0f-128.0f};
//XMGLOBALCONST XMVECTORF32 g_XMMaxUInt           = {65536.0f*65536.0f-256.0f,65536.0f*65536.0f-256.0f,65536.0f*65536.0f-256.0f,65536.0f*65536.0f-256.0f};
//XMGLOBALCONST XMVECTORF32 g_XMUnsignedFix       = {32768.0f*65536.0f,32768.0f*65536.0f,32768.0f*65536.0f,32768.0f*65536.0f};
//XMGLOBALCONST XMVECTORF32 g_XMsrgbScale         = { 12.92f, 12.92f, 12.92f, 1.0f };
//XMGLOBALCONST XMVECTORF32 g_XMsrgbA             = { 0.055f, 0.055f, 0.055f, 0.0f };
//XMGLOBALCONST XMVECTORF32 g_XMsrgbA1            = { 1.055f, 1.055f, 1.055f, 1.0f };
//XMGLOBALCONST XMVECTORI32 g_XMExponentBias      = {127, 127, 127, 127};
//XMGLOBALCONST XMVECTORI32 g_XMSubnormalExponent = {-126, -126, -126, -126};
//XMGLOBALCONST XMVECTORI32 g_XMNumTrailing       = {23, 23, 23, 23};
//XMGLOBALCONST XMVECTORI32 g_XMMinNormal         = {0x00800000, 0x00800000, 0x00800000, 0x00800000};
SU_GLOBALCONST(Vector_u32) NegInfinity			= {{{ 0xFF800000, 0xFF800000, 0xFF800000, 0xFF800000 }}};
//XMGLOBALCONST XMVECTORU32 g_XMNegQNaN           = {0xFFC00000, 0xFFC00000, 0xFFC00000, 0xFFC00000};
//XMGLOBALCONST XMVECTORI32 g_XMBin128            = {0x43000000, 0x43000000, 0x43000000, 0x43000000};
//XMGLOBALCONST XMVECTORU32 g_XMBinNeg150         = {0xC3160000, 0xC3160000, 0xC3160000, 0xC3160000};
//XMGLOBALCONST XMVECTORI32 g_XM253               = {253, 253, 253, 253};


//XMGLOBALCONST XMVECTORF32 g_XMExpEst1           = {-6.93147182e-1f, -6.93147182e-1f, -6.93147182e-1f, -6.93147182e-1f};
//XMGLOBALCONST XMVECTORF32 g_XMExpEst2           = {+2.40226462e-1f, +2.40226462e-1f, +2.40226462e-1f, +2.40226462e-1f};
//XMGLOBALCONST XMVECTORF32 g_XMExpEst3           = {-5.55036440e-2f, -5.55036440e-2f, -5.55036440e-2f, -5.55036440e-2f};
//XMGLOBALCONST XMVECTORF32 g_XMExpEst4           = {+9.61597636e-3f, +9.61597636e-3f, +9.61597636e-3f, +9.61597636e-3f};
//XMGLOBALCONST XMVECTORF32 g_XMExpEst5           = {-1.32823968e-3f, -1.32823968e-3f, -1.32823968e-3f, -1.32823968e-3f};
//XMGLOBALCONST XMVECTORF32 g_XMExpEst6           = {+1.47491097e-4f, +1.47491097e-4f, +1.47491097e-4f, +1.47491097e-4f};
//XMGLOBALCONST XMVECTORF32 g_XMExpEst7           = {-1.08635004e-5f, -1.08635004e-5f, -1.08635004e-5f, -1.08635004e-5f};


//XMGLOBALCONST XMVECTORF32 g_XMLogEst1           = {-0.721242f, -0.721242f, -0.721242f, -0.721242f};
//XMGLOBALCONST XMVECTORF32 g_XMLogEst2           = {+0.479384f, +0.479384f, +0.479384f, +0.479384f};
//XMGLOBALCONST XMVECTORF32 g_XMLogEst3           = {-0.350295f, -0.350295f, -0.350295f, -0.350295f};
//XMGLOBALCONST XMVECTORF32 g_XMLogEst4           = {+0.248590f, +0.248590f, +0.248590f, +0.248590f};
//XMGLOBALCONST XMVECTORF32 g_XMLogEst5           = {-0.145700f, -0.145700f, -0.145700f, -0.145700f};
//XMGLOBALCONST XMVECTORF32 g_XMLogEst6           = {+0.057148f, +0.057148f, +0.057148f, +0.057148f};
//XMGLOBALCONST XMVECTORF32 g_XMLogEst7           = {-0.010578f, -0.010578f, -0.010578f, -0.010578f};
//XMGLOBALCONST XMVECTORF32 g_XMLgE               = {+1.442695f, +1.442695f, +1.442695f, +1.442695f};
//XMGLOBALCONST XMVECTORF32 g_XMInvLgE            = {+6.93147182e-1f, +6.93147182e-1f, +6.93147182e-1f, +6.93147182e-1f};

/****************************************************************************
 *
 * Load operations
 *
 ****************************************************************************/

static inline Vector SU_CALLCONV load_float(float x) {
	return _mm_set1_ps(x);
}

static inline Vector SU_CALLCONV load_float3(const math::Vector3<float>& source) {
	__m128 x = _mm_load_ss(&source.v[0]);
	__m128 y = _mm_load_ss(&source.v[1]);
	__m128 z = _mm_load_ss(&source.v[2]);
	__m128 xy = _mm_unpacklo_ps(x, y);
	return _mm_movelh_ps(xy, z);
}

static inline Vector SU_CALLCONV load_float3(const float* source) {
	// Reads an extra float which is zero'd
	__m128 v = _mm_load_ps(source);
	return _mm_and_ps(v, simd::Mask3);
}

static inline Vector SU_CALLCONV load_float3(const math::Vector3f_a& source) {
	// Reads an extra float which is zero'd
	__m128 v = _mm_load_ps(source.v);
	return _mm_and_ps(v, simd::Mask3);
}

static inline Vector SU_CALLCONV load_float4(const math::Vector3f_a& source) {
	// Reads an extra float!!!
	return _mm_load_ps(source.v);
}

static inline Vector SU_CALLCONV load_float4(const math::Vector4f_a& source) {
	return _mm_load_ps(source.v);
}

static inline Vector SU_CALLCONV load_float4(const float* source) {
	return _mm_load_ps(source);
}

static inline Vector SU_CALLCONV load_unaligned_float4(const float* source) {
	return _mm_loadu_ps(source);
}

/****************************************************************************
 *
 * Store operations
 *
 ****************************************************************************/

static inline float SU_CALLCONV get_x(FVector v) {
	return _mm_cvtss_f32(v);
}

static inline void SU_CALLCONV store_float3(math::Vector3<float>& destination, FVector v) {
	Vector t1 = SU_PERMUTE_PS(v, _MM_SHUFFLE(1, 1, 1, 1));
	Vector t2 = SU_PERMUTE_PS(v, _MM_SHUFFLE(2, 2, 2, 2));
	_mm_store_ss(&destination.v[0], v);
	_mm_store_ss(&destination.v[1], t1);
	_mm_store_ss(&destination.v[2], t2);
}

//static inline void SU_CALLCONV store_float3(Vector3f_a& destination, FVector v) {
//	Vector t = SU_PERMUTE_PS(v, _MM_SHUFFLE(2, 2, 2, 2));
//	_mm_storel_epi64(reinterpret_cast<__m128i*>(&destination), _mm_castps_si128(v));
//	_mm_store_ss(&destination[2], t);
//}

static inline void SU_CALLCONV store_float4(math::Vector3f_a& destination, FVector v) {
	_mm_store_ps(destination.v, v);
}

static inline void SU_CALLCONV store_float4(math::Vector4f_a& destination, FVector v) {
	_mm_store_ps(destination.v, v);
}

static inline void SU_CALLCONV store_float4(float* destination, FVector v) {
	_mm_store_ps(destination, v);
}

}