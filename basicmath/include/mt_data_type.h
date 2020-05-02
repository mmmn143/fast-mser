#pragma once

#include <math.h>
#include <limits>

namespace basicmath {
	enum mt_Depth {
		mt_U8,
		mt_I8,
		mt_U16,
		mt_I16,
		mt_U32,
		mt_I32,
		mt_U64,
		mt_I64,
		mt_F32,
		mt_F64,

		mt_User,
	};

	enum mt_Memory_Type {
		mt_Memory_Type_CPU,
		mt_Memory_Type_GPU,
		mt_Memory_Type_Distributed_CPU,
		mt_Memory_Type_Distributed_GPU,
	};

	static const int mt_Mat_Normal_Support_Dim = 4;
	static const int mt_Depth_Size[] = {1, 1, 2, 2, 4, 4, 8, 8, 4, 8};

	static int mt_Depth_Mask = 0x0000ffff;
	static int mt_Channel_Mask = 0x7fff0000;
	static int mt_Channel_Align_Type_Mask = 0x00008000;

	class mt_Depth_Channel {
	public:

		mt_Depth_Channel() {
			m_mask = -1;
		}

		mt_Depth_Channel(mt_Depth depth, i32 channel = 1) {
			basiclog_assert2(depth <= mt_Depth_Mask && (channel - 1 <= (mt_Channel_Mask >> 16)));

			m_mask = (channel - 1) << 16 | depth;
		}

		i32 channel() const {
			return ((m_mask & mt_Channel_Mask) >> 16) + 1;
		}

		mt_Depth depth() const {
			return mt_Depth(m_mask & mt_Depth_Mask);
		}

		i32 depth_size() const {
			return mt_Depth_Size[depth()];
		}

		i32 size() const {
			return depth_size() * channel();
		}

		static string depth_str(mt_Depth depth);
		static mt_Depth depth_i32(const string& depth);

		static string depth_channel_str(mt_Depth_Channel depth_channel);
		static mt_Depth_Channel depth_channel_i32(const string& depth_channel);

		bool operator==(const mt_Depth_Channel& other) const {
			return m_mask == other.m_mask;
		}

		bool operator!=(const mt_Depth_Channel& other) const {
			return !(*this == other);
		}

	protected:

		i32 m_mask;
	};

	static const mt_Depth_Channel mt_U8C1 = mt_Depth_Channel(mt_U8, 1);
	static const mt_Depth_Channel mt_I8C1 = mt_Depth_Channel(mt_I8, 1);
	static const mt_Depth_Channel mt_U16C1 = mt_Depth_Channel(mt_U16, 1);
	static const mt_Depth_Channel mt_I16C1 = mt_Depth_Channel(mt_I16, 1);
	static const mt_Depth_Channel mt_U32C1 = mt_Depth_Channel(mt_U32, 1);
	static const mt_Depth_Channel mt_I32C1 = mt_Depth_Channel(mt_I32, 1);
	static const mt_Depth_Channel mt_U64C1 = mt_Depth_Channel(mt_U64, 1);
	static const mt_Depth_Channel mt_I64C1 = mt_Depth_Channel(mt_I64, 1);
	static const mt_Depth_Channel mt_F32C1 = mt_Depth_Channel(mt_F32, 1);
	static const mt_Depth_Channel mt_F64C1 = mt_Depth_Channel(mt_F64, 1);

	static const mt_Depth_Channel mt_U8C2 = mt_Depth_Channel(mt_U8, 2);
	static const mt_Depth_Channel mt_I8C2 = mt_Depth_Channel(mt_I8, 2);
	static const mt_Depth_Channel mt_U16C2 = mt_Depth_Channel(mt_U16, 2);
	static const mt_Depth_Channel mt_I16C2 = mt_Depth_Channel(mt_I16, 2);
	static const mt_Depth_Channel mt_U32C2 = mt_Depth_Channel(mt_U32, 2);
	static const mt_Depth_Channel mt_I32C2 = mt_Depth_Channel(mt_I32, 2);
	static const mt_Depth_Channel mt_U64C2 = mt_Depth_Channel(mt_U64, 2);
	static const mt_Depth_Channel mt_I64C2 = mt_Depth_Channel(mt_I64, 2);
	static const mt_Depth_Channel mt_F32C2 = mt_Depth_Channel(mt_F32, 2);
	static const mt_Depth_Channel mt_F64C2 = mt_Depth_Channel(mt_F64, 2);

	static const mt_Depth_Channel mt_U8C3 = mt_Depth_Channel(mt_U8, 3);
	static const mt_Depth_Channel mt_I8C3 = mt_Depth_Channel(mt_I8, 3);
	static const mt_Depth_Channel mt_U16C3 = mt_Depth_Channel(mt_U16, 3);
	static const mt_Depth_Channel mt_I16C3 = mt_Depth_Channel(mt_I16, 3);
	static const mt_Depth_Channel mt_U32C3 = mt_Depth_Channel(mt_U32, 3);
	static const mt_Depth_Channel mt_I32C3 = mt_Depth_Channel(mt_I32, 3);
	static const mt_Depth_Channel mt_U64C3 = mt_Depth_Channel(mt_U64, 3);
	static const mt_Depth_Channel mt_I64C3 = mt_Depth_Channel(mt_I64, 3);
	static const mt_Depth_Channel mt_F32C3 = mt_Depth_Channel(mt_F32, 3);
	static const mt_Depth_Channel mt_F64C3 = mt_Depth_Channel(mt_F64, 3);

	static const mt_Depth_Channel mt_U8C4 = mt_Depth_Channel(mt_U8, 4);
	static const mt_Depth_Channel mt_I8C4 = mt_Depth_Channel(mt_I8, 4);
	static const mt_Depth_Channel mt_U16C4 = mt_Depth_Channel(mt_U16, 4);
	static const mt_Depth_Channel mt_I16C4 = mt_Depth_Channel(mt_I16, 4);
	static const mt_Depth_Channel mt_U32C4 = mt_Depth_Channel(mt_U32, 4);
	static const mt_Depth_Channel mt_I32C4 = mt_Depth_Channel(mt_I32, 4);
	static const mt_Depth_Channel mt_U64C4 = mt_Depth_Channel(mt_U64, 4);
	static const mt_Depth_Channel mt_I64C4 = mt_Depth_Channel(mt_I64, 4);
	static const mt_Depth_Channel mt_F32C4 = mt_Depth_Channel(mt_F32, 4);
	static const mt_Depth_Channel mt_F64C4 = mt_Depth_Channel(mt_F64, 4);

	enum mt_Conv_Boundary_Type {
		mt_Conv_Boundary_Type_Valid,
		mt_Conv_Boundary_Type_Full,
		mt_Conv_Boundary_Type_Same,
	};

	enum mt_Conv_Kernel_Align_Type {
		mt_Conv_Kernel_Align_Type_Row,
		mt_Conv_Kernel_Align_Type_Col,
		mt_Conv_Kernel_Align_Type_Cross_Row,
		mt_Conv_Kernel_Align_Type_Cross_Col,
	};

	static const string mt_Conv_Boundary_Type_Descriptions[] = {"valid", "full", "same"};

	enum mt_Pooling_Type{
		mt_Pooling_Type_Mean,
		mt_Pooling_Type_Max,
		mt_Pooling_Type_Min,
		mt_Pooling_Type_Sum,
		mt_Pooling_Type_First_Value,		
		mt_Pooling_Type_Index,
	};

	static const string mt_Pooling_Type_Descriptions[] = {"mean", "max", "min", "first_value"};

	enum mt_Dist_Type {
		mt_Dist_Type_L1,
		mt_Dist_Type_L2,
	};

	static const f64 mt_PI = 3.1415926535898;
	static const f64 mt_E = exp(1);
	static const f64 mt_Nan = std::numeric_limits<f64>::quiet_NaN();
	static const f64 mt_Infinity = std::numeric_limits<f64>::infinity();
	static const i8 mt_S8_Max = 127;
	static const i8 mt_S8_Min = -128;
	static const u8 mt_U8_Max = 255;
	static const u8 mt_U8_Min = 0;
	static const i16 mt_I16_Max = 32767;
	static const i16 mt_I16_Min = -32768;
	static const i32 mt_I32_Max = 2147483647;
	static const i32 mt_I32_Min = (-2147483647 - 1);
	static const i64 mt_I64_Max = 9223372036854775807;
	static const i64 mt_I64_Min = (-9223372036854775807 - 1);
	static const u64 mt_U64_Max = 18446744073709551615;

	enum mt_Activate_Type {
		mt_Activate_Type_Sigmoid,
		mt_Activate_Type_Linear,
		mt_Activate_Type_Tanh,
		mt_Activate_Type_Softmax,
		mt_Activate_Type_Relu,
	};

	static const string mt_Activate_Type_Descriptions[] = {"sigmoid", "linear", "tanh", "softmax", "relu"};

	static const f64 mt_Tanh_Default_Alpha = 1.7159;
	static const f64 mt_Tanh_Default_Beta = 0.666667;

	enum mt_Loss_Type {
		mt_Loss_Type_0_1 = 0,
		mt_Loss_Type_Quadratic,
		mt_Loss_Type_Logarithmic,
	};

	static const string mt_Loss_Type_Descriptions[] = {"0_1", "quadratic", "logarithmic"};
}